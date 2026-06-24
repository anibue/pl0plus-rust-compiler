# 内容 3：PL/0 改造与 Rust 特性实现 — W3 借用检查器

---

## 4.1 借用检查器概述

### 4.1.1 设计目标

Rust 的核心创新之一是**所有权系统**（Ownership System），它通过编译时检查确保内存安全，无需垃圾回收。PL/0+ 实现了 Rust 所有权系统的简化版本：

- **所有权**：每个值有且仅有一个所有者
- **借用**：可以临时借用值，但不能超过所有者生命周期
- **可变性**：区分不可变借用（`&T`）和可变借用（`&mut T`）
- **互斥规则**：同一时刻只能有一个可变借用或多个不可变借用

### 4.1.2 实现架构

```
┌─────────────────────────────────────────┐
│         Parser (parser.cpp)             │
│  ┌──────────────────────────────────┐  │
│  │  handleLetDeclaration()          │  │
│  │  handleBorrowExpr()              │  │
│  │  handleDerefExpr()               │  │
│  └────────────┬─────────────────────┘  │
│               │ 调用                    │
│  ┌────────────▼─────────────────────┐  │
│  │    BorrowTracker                 │  │
│  │  - borrow_imm() / borrow_mut()   │  │
│  │  - check_rule0~4()               │  │
│  │  - enter_scope() / exit_scope()  │  │
│  │  - begin_statement()             │  │
│  │  - abort_statement()             │  │
│  └────────────┬─────────────────────┘  │
│               │ 依赖                    │
│  ┌────────────▼─────────────────────┐  │
│  │    RustSymbolTable               │  │
│  │  - declare() / lookup()          │  │
│  │  - borrow_imm() / borrow_mut()   │  │
│  │  - take_snapshot() / rollback()  │  │
│  └──────────────────────────────────┘  │
└─────────────────────────────────────────┘
```

---

## 4.2 核心数据结构

### 4.2.1 BorrowRecord

记录每次借用操作的详细信息：

```cpp
struct BorrowRecord {
    string borrower;      // 借用者变量名
    string owner;         // 所有者变量名
    bool   is_mut;        // 是否为可变借用
    int    borrow_line;   // 借用发生的行号
    int    owner_scope;   // 所有者作用域层级
    int    borrower_scope;// 借用者作用域层级
    bool   alive;         // 借用是否仍然有效
};
```

### 4.2.2 BorrowTracker

管理所有借用记录，提供借用检查和作用域管理：

```cpp
class BorrowTracker {
private:
    RustSymbolTable* sym_table;       // 依赖的符号表
    vector<BorrowRecord> records;     // 所有借用记录
    int current_scope_level;          // 当前作用域层级
    stack<Snapshot> snapshot_stack;   // 快照栈（用于回滚）

    struct Snapshot {
        vector<BorrowRecord> records_copy;
        int scope_level_copy;
    };

public:
    // 语句级别 API
    void begin_statement();   // 语句开始前拍快照
    void end_statement();     // 语句成功后提交
    void abort_statement();   // 语句失败后回滚

    // 作用域管理
    void enter_scope();
    void exit_scope();

    // 借用操作
    bool borrow_imm(string borrower, string owner, int line);
    bool borrow_mut(string borrower, string owner, int line);
    void release_borrow(string borrower);

    // 规则检查
    bool check_rule0(string owner);  // 借用者必须已初始化
    bool check_rule1(string owner);  // 不可变借用可多个
    bool check_rule2(string owner);  // 可变借用只能一个
    bool check_rule3(string owner);  // 可变与不可变互斥
    bool check_rule4(string owner, int borrower_scope);  // 防悬垂
};
```

### 4.2.3 RustSymbolTable 扩展

在原有符号表基础上，增加借用相关字段：

```cpp
struct Symbol {
    string name;
    string type;                  // "i8" | "i16" | "i32" | "&i32" | "&mut i32"
    int    addr;                  // 栈地址
    int    scope_level;           // 作用域层级
    int    decl_line;             // 声明行
    int    first_assign_line;     // 第一次赋值行（-1 = 未赋值）
    bool   is_initialized;        // 是否已初始化
    DeclKind decl_kind;           // CONST / VAR / LET / LET_MUT
    int    borrow_count_imm;      // 不可变借用计数
    int    borrow_count_mut;      // 可变借用计数
};
```

---

## 4.3 五条借用规则

### 4.3.1 Rule 0：借用者必须已初始化

**规则**：不能借用未初始化的变量。

**实现**：
```cpp
bool BorrowTracker::check_rule0(string owner) {
    RustSymbol* sym = sym_table->lookup(owner);
    if (!sym) return false;  // E0001: 未声明
    if (!sym->is_initialized) return false;  // E0030: 未初始化
    return true;
}
```

**示例**：
```rust
let x: i32;        // 未初始化
let r: &i32 = &x;  // ❌ E0030: 借用未初始化变量
```

### 4.3.2 Rule 1：不可变借用可多个

**规则**：可以同时存在多个不可变借用（`&T`），但不能有可变借用。

**实现**：
```cpp
bool BorrowTracker::check_rule1(string owner) {
    // 不可变借用可以多个同时存在，但不能有可变借用
    for (const auto& r : records) {
        if (r.owner == owner && r.is_mut && r.alive) {
            return false;  // 已存在可变借用
        }
    }
    return true;
}
```

**示例**：
```rust
let x: i32 = 5;
let r1: &i32 = &x;  // ✅ 第一个不可变借用
let r2: &i32 = &x;  // ✅ 第二个不可变借用
```

### 4.3.3 Rule 2：可变借用只能一个

**规则**：同一时刻只能有一个可变借用（`&mut T`）。

**实现**：
```cpp
bool BorrowTracker::check_rule2(string owner) {
    // 可变借用只能有一个
    for (const auto& r : records) {
        if (r.owner == owner && r.is_mut && r.alive) {
            return false;  // 已存在可变借用
        }
    }
    return true;
}
```

**示例**：
```rust
let mut x: i32 = 5;
let m1: &mut i32 = &mut x;  // ✅ 第一个可变借用
let m2: &mut i32 = &mut x;  // ❌ E0022: 已有可变借用
```

### 4.3.4 Rule 3：可变与不可变互斥

**规则**：如果已有任何借用（不可变或可变），不能再创建可变借用。

**实现**：
```cpp
bool BorrowTracker::check_rule3(string owner) {
    // 如果要创建可变借用，不能有任何借用存在
    for (const auto& r : records) {
        if (r.owner == owner && r.alive) {
            return false;  // 已存在任何借用
        }
    }
    return true;
}
```

**示例**：
```rust
let x: i32 = 5;
let r: &i32 = &x;          // ✅ 不可变借用
let m: &mut i32 = &mut x;  // ❌ E0021: 已有不可变借用
```

### 4.3.5 Rule 4：防悬垂（借用不能超过所有者生命周期）

**规则**：借用者的作用域层级必须 >= 所有者的作用域层级。

**实现**：
```cpp
bool BorrowTracker::check_rule4(string owner, int borrower_scope) {
    RustSymbol* sym = sym_table->lookup(owner);
    if (!sym) return false;

    int owner_scope = sym->scope_level;

    // 借用者的作用域层级必须 >= 所有者的作用域层级
    // 如果借用者在更外层（数字更小），而所有者在更内层，则内层销毁时会导致悬垂
    if (borrower_scope < owner_scope) {
        return false;  // E0027: 悬垂借用
    }
    return true;
}
```

**示例**：
```rust
let r: &i32;
begin
    let x: i32 = 5;
    r = &x;  // ❌ E0027: 外层借用内层变量，x 离开作用域后 r 悬垂
end
```

---

## 4.4 作用域管理

### 4.4.1 作用域规则

PL/0+ 中创建作用域的关键字：

| 关键字 | 是否创建作用域 |
|--------|----------------|
| `const/var/let` | ❌ 不创建 |
| `begin...end` | ✅ 创建块级作用域 |
| `procedure...;` | ✅ 创建过程级作用域 |
| `if/while/repeat` 分支 | ✅ 创建分支作用域 |
| `program` | ✅ 创建程序级作用域 |

### 4.4.2 enter_scope / exit_scope

```cpp
void BorrowTracker::enter_scope() {
    current_scope_level++;
    sym_table->enter_scope();
}

void BorrowTracker::exit_scope() {
    // 释放当前作用域的所有借用
    for (auto& r : records) {
        if (r.borrower_scope == current_scope_level && r.alive) {
            r.alive = false;
            sym_table->release_borrow(r.owner, r.is_mut);
        }
    }
    sym_table->exit_scope(current_scope_level);
    current_scope_level--;
}
```

### 4.4.3 RAII 自动释放

当变量离开作用域时，自动释放其所有借用：

```rust
let x: i32 = 5;
begin
    let r: &i32 = &x;  // 借用 x
    write(*r);
end  // r 离开作用域，自动释放对 x 的借用
```

---

## 4.5 快照/回滚机制

### 4.5.1 设计动机

借用检查需要在语句级别进行错误恢复。如果一条语句导致借用规则违反，需要回滚到语句开始前的状态。

### 4.5.2 实现

```cpp
void BorrowTracker::begin_statement() {
    // 拍快照
    Snapshot snap;
    snap.records_copy = records;
    snap.scope_level_copy = current_scope_level;
    snapshot_stack.push(snap);
    
    // 符号表也拍快照
    sym_table->take_snapshot();
}

void BorrowTracker::end_statement() {
    // 提交快照
    if (!snapshot_stack.empty()) {
        snapshot_stack.pop();
    }
    sym_table->commit_snapshot();
}

void BorrowTracker::abort_statement() {
    // 回滚到快照状态
    if (snapshot_stack.empty()) return;
    Snapshot snap = snapshot_stack.top();
    snapshot_stack.pop();
    records = snap.records_copy;
    current_scope_level = snap.scope_level_copy;
    sym_table->rollback_snapshot();
}
```

### 4.5.3 使用示例

```cpp
void Parser::handleStatement(AstNode* n, sTable* s) {
    borrowTracker->begin_statement();  // 拍快照
    
    try {
        // 处理语句...
        if (n->getType() == GrammarSymSpace::ASSIGHNSTATEMENT) {
            handleAssignStatement(n, s);
        } else if (n->getType() == GrammarSymSpace::LETDECLARATION) {
            handleLetDeclaration(n, s);
        }
        // ...
        
        borrowTracker->end_statement();  // 成功，提交
    } catch (BorrowError& e) {
        borrowTracker->abort_statement();  // 失败，回滚
        errorList.push(e.getErrorMessage());
    }
}
```

---

## 4.6 错误码表

| 错误码 | 含义 | 触发条件 |
|--------|------|----------|
| E0020 | 借用规则违反（通用）| 任何借用规则违反 |
| E0021 | 不可变借用冲突 | 已有不可变借用，尝试创建可变借用 |
| E0022 | 可变借用冲突 | 已有可变借用，尝试再创建可变借用 |
| E0023 | 跨过程借用 | 借用超过过程边界 |
| E0024 | 不可变变量被可变借用 | `let x: i32 = 5; &mut x` |
| E0025 | 解引用非引用类型 | `*x` 但 x 不是引用 |
| E0026 | 通过不可变借用赋值 | `*r := 10` 但 r 是 `&i32` |
| E0027 | 悬垂借用 | 借用者在外层，所有者在内层 |
| E0028 | 运行时非法地址 | LODI/STOI 越界 |
| E0030 | 未初始化变量使用 | 借用未初始化变量 |
| E0031 | 重复赋值 const | 对 const 变量赋值 |
| E0040 | const 被借用 | `const x = 5; &x` |
| E0041 | var 被借用 | `var x; &x` |

---

## 4.7 测试用例

### 4.7.1 T11：基础不可变借用

```rust
let x: i32 = 5;
let r: &i32 = &x;
write(*r);  // 输出 5
```

**预期**：✅ 通过

### 4.7.2 T12：多个不可变借用

```rust
let x: i32 = 5;
let r1: &i32 = &x;
let r2: &i32 = &x;
write(*r1);  // 输出 5
write(*r2);  // 输出 5
```

**预期**：✅ 通过

### 4.7.3 T13：可变借用

```rust
let mut x: i32 = 5;
let m: &mut i32 = &mut x;
*m := 10;
write(x);  // 输出 10
```

**预期**：✅ 通过

### 4.7.4 T14：可变借用冲突

```rust
let mut x: i32 = 5;
let m1: &mut i32 = &mut x;
let m2: &mut i32 = &mut x;  // ❌ E0022
```

**预期**：❌ E0022: 已有可变借用

### 4.7.5 T15：不可变与可变互斥

```rust
let x: i32 = 5;
let r: &i32 = &x;
let m: &mut i32 = &mut x;  // ❌ E0021
```

**预期**：❌ E0021: 已有不可变借用

### 4.7.6 T16：悬垂借用

```rust
let r: &i32;
begin
    let x: i32 = 5;
    r = &x;  // ❌ E0027
end
```

**预期**：❌ E0027: 悬垂借用

### 4.7.7 T17：const 被借用

```rust
const x = 5;
let r: &i32 = &x;  // ❌ E0040
```

**预期**：❌ E0040: const 不可借用

### 4.7.8 T18：var 被借用

```rust
var x;
let r: &i32 = &x;  // ❌ E0041
```

**预期**：❌ E0041: var 不可借用

### 4.7.9 T19：不可变变量被可变借用

```rust
let x: i32 = 5;
let m: &mut i32 = &mut x;  // ❌ E0024
```

**预期**：❌ E0024: 不可变变量不可被可变借用

### 4.7.10 T20：解引用非引用类型

```rust
let x: i32 = 5;
write(*x);  // ❌ E0025
```

**预期**：❌ E0025: x 不是引用类型

---

## 4.8 本章小结

### 4.8.1 实现亮点

1. **完整的借用检查**：实现了 Rust 的 5 条核心借用规则
2. **作用域管理**：支持块级、过程级、分支级作用域
3. **RAII 自动释放**：借用离开作用域自动释放
4. **快照/回滚**：语句级别错误恢复
5. **精确错误报告**：14 个错误码，精确定位问题

### 4.8.2 与 Rust 的差异

| 特性 | Rust | PL/0+ |
|------|------|-------|
| 生命周期标注 | `<'a>` | ❌ 不支持 |
| 多级解引用 | `**p` | ❌ 仅支持一级 |
| 借用检查时机 | 编译时 | 编译时 |
| 所有权转移 | `move` | ❌ 不支持 |
| 智能指针 | `Box`, `Rc`, `Arc` | ❌ 不支持 |

### 4.8.3 下一步

W4 将实现 pcode 扩展和虚拟机，支持借用和解引用的运行时执行。

---

## 参考文献

1. The Rust Programming Language. Chapter 4: Understanding Ownership.
2. Rust Borrow Checker Documentation. https://doc.rust-lang.org/nomicon/borrow-checker.html
3. PL/0 编译器实现（多版本）. https://github.com/topics/pl0
