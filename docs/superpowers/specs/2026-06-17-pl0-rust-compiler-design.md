# PL/0+ Rust 编译器课程设计 Spec

| 项目 | 信息 |
|------|------|
| **作者** | xwmsu |
| **创建日期** | 2026-06-17 |
| **课程** | 编译原理课程设计（人工智能与计科班）|
| **基础** | `PL0_compiler_with_LL1_and_QT` (C++ Qt) |
| **总工作量** | ~4 周 |
| **目标评级** | "优"（冲最高分）|

---

## 1. 项目目标

将 PL/0 编译器**增量式**改造为支持 Rust 风格特性，重点实现：

1. **必做内容 2**（报告）：Rust if 条件语句的词法/语法分析
2. **必做内容 3**（代码+报告）：
   - 栈内存管理
   - 借用检查（Rust 借用规则）

完整保留原 PL/0 功能（const/var/procedure/if/while/odd/begin/end...），新增 Rust 风格特性作为增量。

---

## 2. 设计理念

### 2.1 方案选择

采用**方案 A：PL/0 增量式 Rust 化**。

| 方案 | 选择 | 理由 |
|------|------|------|
| **A. 增量式** ⭐ | ✅ | 改动可控 / 兼容原测试用例 / 评分有保障 |
| B. 完全重写 | ❌ | 工作量极大 / 风险高 |
| C. 独立 Rust 模式 | ❌ | 概念割裂 |

### 2.2 兼容性策略

- 保留所有原 PL/0 关键字（35 个 Token）
- 新增 Rust 关键字（8 个 Token：let/mut/i8/i16/i32/&/&mut/:）
- 两种风格可混用
- 28 个测试用例中 6 个保证 PL/0 兼容

---

## 3. 系统架构

```
源代码 (.pl0)
   ↓
[Lexer] 词法分析
   • 35 个原 PL/0 Token + 8 个 Rust Token = 43 个
   ↓
Token 流
   ↓
[Parser] LL(1) 语法分析
   • 原 PL/0 语法 + 新增 let/借用/解引用
   ↓
AST
   ↓
[BorrowChecker] ⭐ 借用检查
   • 类型检查
   • 借用规则验证（5 条规则）
   • 作用域 + RAII
   ↓
[CodeGen] pcode 生成
   • 12 条指令（原 8 + LEA/LODI/STOI + REL）
   ↓
[VM] 虚拟机执行
   • 静态栈分配
   • 间接寻址
```

---

## 4. 词法层设计

### 4.1 Token 列表

#### 原 PL/0 Token（保留，35 个）
- 标识符/数字：`ID`, `NUM`
- 运算符：`PLUS MINUS TIMES DIVISION EQL NEQ LT LE GT GE`
- 赋值/括号：`BECOMES LPAREN RPAREN SEMICOLON PERIOD COMMA`
- 关键字：`ODD BEGIN END IF THEN ELSE WHILE WRITE READ DO CALL CONST VAR PROC REPEAT UNTIL`
- 错误：`ERRORSYM`

#### 新增 Rust Token（8 个）
| Token 枚举 | 名称 | 用途 |
|------------|------|------|
| `LETSYM` | `let` | Rust 风格声明 |
| `MUTSYM` | `mut` | 可变声明 |
| `I8SYM` `I16SYM` `I32SYM` | `i8` `i16` `i32` | 整数类型 |
| `AMPSYM` | `&` | 不可变借用 |
| `AMPMUTSYM` | `&mut` | 可变借用（单 token）|
| `COLONSYM` | `:` | 类型标注 |

### 4.2 状态机

`state` 枚举：
```
BLANK LETTER DIGIT COLON EQL LS GT PLUS MINUS TIMES DIVISION
LPAREN RPAREN COMMA SEMICOLON PERIOD
AMP NUL          (* ⭐ 新增 AMP 状态：识别 & / &mut 的 maximal-munch *)
```

**AMP 状态的识别逻辑**：
```
看到 & 时进入 AMP 状态：
  - 前瞻 1 个字符
  - 跟 "mut" → 输出 AMPMUTSYM
  - 跟其他 → 输出 AMPSYM
```

这是 maximal-munch 原则的应用（最长匹配）。

---

## 5. 类型系统

### 5.1 类型定义

```cpp
enum class BaseType { VOID, I8, I16, I32 };

struct Type {
    BaseType base;
    bool is_ref;       // 是否借用
    bool is_mut_ref;   // 是否可变借用
    int  cell_size;    // 占用栈单元数（统一为 1）
};
```

### 5.2 类型映射

| 类型 | base | is_ref | is_mut_ref | cell_size |
|------|------|--------|------------|-----------|
| i8 | `BaseType::I8` | false | false | 1 |
| i16 | `BaseType::I16` | false | false | 1 |
| i32 | `BaseType::I32` | false | false | 1 |
| &i32 | `BaseType::I32` | true | false | 1 |
| &mut i32 | `BaseType::I32` | true | true | 1 |

> 注：`i8/i16/i32` 在 PL/0 栈上各占 1 cell（一个 word = 一个 int 槽位）。cell_size 不再硬编码 4 字节，与 PL/0 栈单元对齐。
> 命名严格按 `enum class BaseType` 定义，无 `BT_` 前缀。

## 6. 符号表与 DeclKind

### 6.1 DeclKind（4 种声明风格）

```cpp
enum class DeclKind {
    CONST,    // const x = 5;        — 不可变，立即初始化，不可借
    VAR,      // var x;              — 可变，不要求初始化，不可借
    LET,      // let x: i32 = 5;     — 不可变，立即初始化，可借 &
    LET_MUT,  // let mut x: i32 = 5; — 可变，立即初始化，可借 &mut
};
```

### 6.2 4 种风格的差异化语义

| 风格 | 类型（用户写）| 类型（内部）| 可变性 | 初始化 | 借用能力 |
|------|--------------|------------|--------|--------|----------|
| `const x = 5;` | integer | **I32** ⭐ | 不可变 | 必须立即 | ❌ |
| `var x;` | integer | **I32** ⭐ | 可变 | 不要求 | ❌ |
| `let x: i32 = 5;` | i32 | I32 | 不可变 | 必须立即 | ✅ & |
| `let mut x: i32 = 5;` | i32 | I32 | 可变 | 必须立即 | ✅ &mut |

> **关键映射**：原 PL/0 的 `integer` 类型在内部**统一映射为 `I32`**。
> 这是因为 `BaseType` 枚举只有 `VOID, I8, I16, I32`，没有 `INTEGER`。
> 词法层遇到未标注类型的 `const x = 5;` 时，自动按 `I32` 处理。
> 这样保证类型系统的一致性。

### 6.3 Symbol 结构

```cpp
struct Symbol {
    string name;
    Type   type;
    int    addr;
    int    scope_level;
    int    decl_line;
    int    first_assign_line;  // -1 = 未赋值
    bool   is_initialized;
    DeclKind decl_kind;
    int    borrow_count_imm;
    int    borrow_count_mut;
};
```

---

## 7. 语法层

### 7.1 完整 EBNF

```ebnf
program        = block "." .
block          = [ constDecl ] [ varDecl ] [ procDecl ] statement .

(* 原有 PL/0 声明 *)
constDecl      = "const" ident "=" number {"," ident "=" number} ";" .
varDecl        = "var" ident {"," ident} ";" .
procDecl       = "procedure" ident ";" block ";" .

(* 新增 Rust 风格声明 *)
letDecl        = "let" [ "mut" ] ident ":" type "=" expression ";" .
type           = refType | baseType .
refType        = AMPSYM baseType | AMPMUTSYM baseType .
baseType       = "i8" | "i16" | "i32" .

(* 语句 *)
statement      = assignSt
               | callSt
               | compoundSt
               | ifSt
               | whileSt
               | repeatSt
               | readSt
               | writeSt
               | letDecl          (* 新增 *)
               .
assignSt       = lvalue ":=" expression .
lvalue         = "*" ident | ident .        (* ⭐ 仅一级解引用，文法层堵死多级 *)
callSt         = "call" ident .
compoundSt     = "begin" statement { ";" statement } "end" .
ifSt           = "if" condition "then" statement [ "else" statement ] .
whileSt        = "while" condition "do" statement .
repeatSt       = "repeat" statement { ";" statement } "until" condition .
readSt         = "read" "(" ident {"," ident} ")" .
writeSt        = "write" "(" expression {"," expression} ")" .

(* 条件 *)
condition      = "odd" expression
               | expression relop expression .
relop          = "=" | "<>" | "<" | "<=" | ">" | ">=" .

(* 表达式（含借用和解引用）*)
expression     = [ "+" | "-" ] term { ("+" | "-") term } .
term           = factor { ("*" | "/") factor } .
factor         = ident
               | number
               | "(" expression ")"
               | borrowExpr          (* 新增 *)
               | derefExpr           (* 新增 *)
               .
borrowExpr     = AMPSYM ident | AMPMUTSYM ident .
derefExpr      = "*" ( ident | "(" expression ")" ) .   (* ⭐ 仅一级，与 lvalue 对称 *)
```

### 7.2 文法关键决策

- `let` 作为**语句**（与 const/var 一致）
- `&mut` 是**单 token**（`AMPMUTSYM`）
- 借用**仅在 factor 层级**出现
- **lvalue 文法层堵死多级解引用**：`lvalue = "*" ident | ident`
  - `*ident` 合法（一级解引用）
  - `**ident` 非法（文法不通过 → 不需要 AST 遍历时额外拦截）
- **derefExpr（读侧）也对称限制**：`derefExpr = "*" ( ident | "(" expression ")" )`
  - 读侧和写侧保持一致：都只支持一级解引用
  - 避免 "`**r := 10` 报错但 `**r` 能读"的不对称陷阱
- 不支持多级解引用 `**p`（第一版，**读侧和写侧都不支持**）
- 引用类型标注：`let r: &i32 = &x;`

---

## 8. 借用检查

### 8.1 借用规则

```
规则 0：借用者必须已初始化
规则 1：不可变借用（&）可多个同时存在
规则 2：可变借用（&mut）只能一个
规则 3：可变与不可变借用互斥
规则 4：借用不能超过所有者生命周期（防悬垂）
```

### 8.2 关键算法

#### 8.2.1 双轨制

| 维度 | 粒度 | 原因 |
|------|------|------|
| snapshot/rollback | per-statement | 错误粒度 |
| borrow release | per-scope | Rust 语义要求 |

#### 8.2.2 作用域规则

| 关键字 | 创建作用域 |
|--------|-----------|
| `const/var/let` | ❌ |
| `begin...end` | ✅ |
| `procedure...;` | ✅（隔离借用）|
| `if/while/repeat` 分支 | ✅ |
| `program` | ✅ |

#### 8.2.3 BorrowRecord

```cpp
struct BorrowRecord {
    string borrower;
    string owner;
    bool   is_mut;
    int    borrow_line;
    int    owner_scope;     // 所有者作用域层级
    int    borrower_scope;  // 借用者作用域层级
    bool   alive;
};
```

#### 8.2.4 跨过程借用禁止

进入 procedure → 初始化干净 BorrowTracker；退出 → 强制清空并检查。

错误：`E0023: cannot borrow 'x' from inside procedure 'P'`

### 8.3 错误恢复策略

**恐慌模式**（panic mode）：
- 词法：跳过无效字符
- 语法：跳到同步点（`;` `end` `begin` `const` `var` `let` `procedure` `if` `while` `repeat` `read` `write` `call` `odd` `EOF`）
- 借用：per-statement rollback
- 最大错误数：50

### 8.4 错误码表

| 错误码 | 含义 |
|--------|------|
| E0001 | 未声明标识符 |
| E0002 | 重复声明 |
| E0010 | 类型不匹配 |
| E0011 | 缺少类型标注 |
| E0020 | 借用规则违反 |
| E0021 | 不可变借用冲突 |
| E0022 | 可变借用冲突 |
| E0023 | 跨过程借用 / 退出时未释放 |
| E0024 | 不可变变量被可变借用 |
| E0025 | 解引用非引用类型 |
| E0026 | 通过不可变借用赋值 |
| E0027 | 悬垂借用 |
| E0028 | 运行时非法地址（LODI/STOI 越界）|
| E0030 | 未初始化变量使用 |
| E0031 | 重复赋值 const |
| E0040 | const 被借用 |
| E0041 | var 被借用 |
| E0099 | 词法错误 |

### 8.5 借用检查的执行时机 ⭐

**两个执行时机**（必须明确）：

| 阶段 | 做什么 | 时机 |
|------|--------|------|
| **静态借用检查** | 编译时检查 5 条规则 | AST 构建后，pcode 生成前 |
| **运行时检查** | LODI/STOI 越界检查 | VM 执行时 |

**静态借用检查的拦截点**（4 个）：

| 拦截点 | 检查内容 | 错误码 |
|--------|---------|--------|
| `letDecl` 处理时 | 名字是否已借用 | E0021 |
| `borrowExpr` 处理时 | 借用规则 1-4 | E0020-E0022, E0024, E0027 |
| `assignSt` 处理时 | 被赋值变量是否在借用中 | E0021 |
| 作用域退出时 | 检测悬垂借用 | E0027 |

**为什么静态检查在 AST 阶段**：
- 借用是基于 AST 节点的名字和作用域
- 比 codegen 阶段更早发现错误
- 减少 pcode 生成时的回滚

### 8.6 E0028 详细说明

**触发条件**：
- `LODI` 指令执行时，栈顶地址 < 0 或 >= STACK_SIZE
- `STOI` 指令执行时，栈顶地址 < 0 或 >= STACK_SIZE

**为什么会出现**：
- 通常是借用检查的漏网之鱼（如借用了一个未初始化的变量）
- 也可能是 VM 自身的栈越界

**处理**：
- VM 抛出运行时错误并终止执行
- 报告错误码 E0028 + 出错指令地址

**示例**：
```
runtime error[E0028]: invalid memory access
  at instruction #42: LODI
  address = -1
execution halted
```

---

## 9. pcode 指令集

### 9.1 完整指令集（12 条）

原 PL/0 8 条 + 新增 3 条间接寻址 + 1 条 REL = 12 条

| 指令 | 含义 | 栈效果 | 用途 |
|------|------|--------|------|
| LIT 0 N | 常数 | push N | 字面量 |
| LOD L A | 直接取值 | push mem[base+A] | 读普通变量 |
| STO L A | 直接存值 | mem[base+A]=pop | 赋给普通变量 |
| **LEA L A** | **取地址** | **push (base+A)** | **`&x`** |
| **LODI** | **间接取值** | **addr=pop; push mem[addr]** | **`*m`** |
| **STOI** | **间接存值** | **val=pop; addr=pop; mem[addr]=val** | **`*m :=`** |
| OPR 0 N | 运算 | varies by N | 算术/比较/IO |
| CAL L A | 调用 | push frame | 过程调用 |
| INI 0 N | 分配 | t += N | 栈空间分配（入作用域）|
| **REL 0 N** | **释放** | **t = t - N** | **块结束时收缩（出作用域）** ⭐ |
| JMP 0 A | 无条件跳转 | pc = A | 循环/分支 |
| JPC 0 A | 条件跳转 | if pop==0 then pc=A | if 条件 |

### 9.2 OPR 子功能

| N | 含义 |
|---|------|
| 0 | return |
| 1-13 | 算术/比较（与原 PL/0 一致）|
| 14 | WRITE |
| 15 | 换行 |
| 16 | READ |

> **注意**：块结束的栈空间释放**不**复用 OPR。`OPR` 指令的 A 域用于区分操作类型（WRITE/READ/算术等），如果加 OPR 0 17 会出现"VM 知道要释放但不知道释放几个 cell"的歧义。
> 改为新增独立指令 `REL 0 N`（详见 9.1 节）。N 在编译时已知，由编译器直接发出。

### 9.3 关键场景 pcode

#### 场景：`*m := 10`

```rust
let mut x: i32 = 5;
let m: &mut i32 = &mut x;
*m := 10;
```

```
1. LIT 0 5     ; STO 0 x_addr     // x = 5
2. LEA 0 x_addr; STO 0 m_addr     // m = &x
3. LOD 0 m_addr; LIT 0 10 ; STOI  // *m = 10
```

---

## 10. 栈内存管理

### 10.1 分配方案

**方案 A：静态分配**（编译时定 cell 偏移）

### 10.2 栈帧布局

```
┌─────────────────┐ ← base
│ SL 静态链       │ 1 cell
│ DL 动态链       │ 1 cell
│ RA 返回地址     │ 1 cell
├─────────────────┤
│ const（不占栈） │ 0 cell
├─────────────────┤
│ var 变量        │ K2 cells
│ let/let mut     │ K3 cells
├─────────────────┤
│ 临时/求值栈     │ 不定
└─────────────────┘ ← t
```

### 10.3 块结束释放

`end` 触发：
1. 释放该层所有借用（per-scope release）
2. 检测悬垂借用（所有者在本层、借用者在外层）
3. **`REL 0 N`（Release N cells）释放 cell**

**为什么不复用 OPR**：
- OPR 的 A 域用于区分操作类型（WRITE=14、READ=16 等）
- 释放的 cell 数 N 是块内变量总数，编译时已知
- 用独立指令 `REL 0 N` 语义最清晰，VM 实现最简单

---

## 11. 测试策略

### 11.1 测试用例（28 个）

| 编号 | 场景 | 预期 |
|------|------|------|
| T1 | 基础借用 `let x: i32 = 5; let r: &i32 = &x;` | ✅ |
| T2 | 多个不可变借用 | ✅ |
| T3 | 可变借用 `let mut x: i32 = 5; let m: &mut i32 = &mut x;` | ✅ |
| T4 | 两个 `&mut` | ❌ E0022 |
| T5 | `&x` + `&mut x` | ❌ E0021 |
| T6 | `const` 被借用 | ❌ E0040 |
| T7 | `var` 被借用 | ❌ E0041 |
| T8-T9 | PL/0 兼容（T1.PL0, E01.PL0）| ✅ |
| T10 | 混合声明 | ✅ |
| T11 | 不可变变量被可变借用 | ❌ E0024 |
| T12 | 借用跨语句延续 | ✅ |
| T13 | begin/end RAII 释放 | ✅ |
| T14 | 借用期间不能改 owner | ❌ E0021 |
| T15 | 跨 procedure 借用 | ❌ E0023 |
| T16 | procedure 退出时未释放 | ❌ E0023 |
| T17 | 嵌套 begin/end | ✅ |
| T18 | 解引用 `*r` | ✅ |
| T19 | 解引用非引用类型 | ❌ E0025 |
| T20 | if 分支作用域 | ✅ |
| T21 | `*m := 10` 修改原值 | ✅ 5→10 |
| T22 | `*r := 10`（r 不可变）| ❌ E0026 |
| T23 | `*x := 10`（x 非引用）| ❌ E0025 |
| T24 | 修改+读取链 | ✅ |
| T25 | 借用冲突时阻止修改 | ❌ E0021 |
| T26 | 悬垂借用 | ❌ E0027 |
| T27 | 合法跨作用域借用 | ✅ |
| T28 | begin/end 内变量释放 | ✅ |

### 11.2 演示

Qt GUI 界面：
1. 加载测试用例
2. 显示 Token 列表
3. 显示语法分析过程
4. 显示 pcode
5. 报告借用检查结果
6. 执行并显示输出

---

## 12. 报告结构

```
1. 引言
   1.1 课程背景
   1.2 设计目标
   1.3 报告结构

2. PL/0 语言与编译器概述
   2.1 PL/0 简介
   2.2 Qt 版 PL/0 架构
   2.3 PL/0 局限性

3. 内容 2：Rust if 词法/语法分析
   3.1 词法分析原理 + Rust 编译器实现
   3.2 语法分析原理 + Rust 编译器实现
   3.3 if 语句 EBNF 文法

4. 内容 3：PL/0 改造与 Rust 实现
   4.1 改造目标
   4.2 类型系统扩展
   4.3 词法层改造
   4.4 语法层改造
   4.5 借用检查实现
   4.6 栈内存管理
   4.7 pcode 指令集扩展

5. 测试与演示
   5.1 测试策略
   5.2 28 个测试用例
   5.3 Qt GUI 演示

6. 总结
   6.1 工作总结
   6.2 心得体会
   6.3 未来工作

参考文献
附录 A：完整 EBNF
附录 B：完整 pcode
附录 C：测试源码
```

---

## 13. 项目目录

```
E:\rust_com\PL0Plus/                # 最终项目
├── compiler/                       # 编译器核心
│   ├── lexer.h/.cpp
│   ├── parser.h/.cpp
│   ├── grammar.h/.cpp
│   ├── symbolTable.h/.cpp
│   ├── borrow_checker.h/.cpp       # ⭐ 新增
│   ├── ast.h/.cpp
│   ├── token.h/.cpp
│   └── error.h/.cpp
├── vm/                             # 虚拟机
│   ├── pl0vm.h/.cpp
│   ├── instruction.h
│   └── stack.h/.cpp
├── ui/                             # Qt GUI
│   ├── mainwindow.h/.cpp
│   ├── codeeditor.h/.cpp
│   └── main.cpp
├── tests/                          # 28 个测试
├── docs/
│   ├── report.md
│   ├── ebnf.md
│   ├── pcode.md
│   └── design_decisions.md
├── README.md
└── application.pro
```

---

## 14. 实施时间表

| 周 | 任务 | 产出 |
|----|------|------|
| W1 | 内容 2 报告 + 读 mrustc | 报告 1-3 章 |
| W2 | 词法/语法扩展 | T1-T10 通过 |
| W3 | 借用检查器 | T11-T22 通过 |
| W4 | pcode + 栈管理 + 报告整合 | T23-T28 通过 + 最终报告 |

---

## 15. 设计决策记录（52 条）

详见附录 D（设计决策完整记录）。所有决策在 brainstorming 过程中与用户确认。

---

## 16. 验收标准

- ✅ 内容 2：完整的 EBNF + 词法/语法分析报告
- ✅ 内容 3：
  - 栈内存管理实现（栈帧布局 + INI/REL）
  - 借用检查实现（5 条规则 + RAII + 错误恢复）
- ✅ 28 个测试用例通过
- ✅ Qt GUI 可演示
- ✅ 与原 PL/0 完全兼容

---

## 17. 风险与缓解

| 风险 | 缓解策略 |
|------|----------|
| 借用检查边界条件复杂 | 充分测试 + 错误恢复 |
| pcode 新指令实现有 bug | 单元测试每条指令 |
| Qt GUI 集成问题 | 保留原 GUI 框架 |
| 报告质量 | 边写代码边记要点 |

---

**Spec 文档完成。等待用户批准。**
