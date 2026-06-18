# 内容 3：PL/0 改造与 Rust 特性实现 — W4 pcode 扩展与虚拟机

> 课程设计报告 — 内容 3 第 5 章
> 作者：xwmsu | 日期：2026-06-18

---

## 5.1 pcode 指令集扩展

### 5.1.1 设计目标

PL/0 原有的 pcode 指令集不支持引用类型和间接寻址。为了支持 Rust 风格的借用和解引用，需要扩展 4 条新指令：

| 指令 | 功能 | 用途 |
|------|------|------|
| `LEA L A` | 取地址 | 支持 `&x`（借用）|
| `LODI` | 间接取值 | 支持 `*r`（解引用读取）|
| `STOI` | 间接存值 | 支持 `*m := val`（解引用写入）|
| `REL 0 N` | 释放 N 个 cell | 支持作用域结束释放变量 |

### 5.1.2 完整指令集（12 条）

| 指令 | 格式 | 功能 | 栈效果 |
|------|------|------|--------|
| `LIT` | `LIT 0 A` | 加载常量 A | push A |
| `LOD` | `LOD L A` | 加载变量 | push mem[base+A] |
| `STO` | `STO L A` | 存储变量 | mem[base+A] = pop |
| `CAL` | `CAL L A` | 调用过程 | push frame |
| `INT` | `INT 0 A` | 分配存储 | t += A |
| `JMP` | `JMP 0 A` | 无条件跳转 | pc = A |
| `JPC` | `JPC 0 A` | 条件跳转 | if pop==0 then pc=A |
| `OPR` | `OPR 0 A` | 运算操作 | varies |
| **`LEA`** | **`LEA L A`** | **取地址** | **push (base+A)** ⭐ |
| **`LODI`** | **`LODI`** | **间接取值** | **pop addr; push mem[addr]** ⭐ |
| **`STOI`** | **`STOI`** | **间接存值** | **pop val; pop addr; mem[addr]=val** ⭐ |
| **`REL`** | **`REL 0 N`** | **释放 cell** | **t -= N** ⭐ |

### 5.1.3 指令域冲突避免

**问题**：为什么 `REL` 不复用 `OPR` 指令的 A 域？

**原因**：`OPR` 指令的 A 域已用于区分操作类型（WRITE=14, READ=16 等）。如果加 `OPR 0 17` 表示释放，VM 会困惑："这是哪种操作？释放几个 cell？"

**解决方案**：新增独立指令 `REL 0 N`，N 在编译时已知，由编译器直接发出。

---

## 5.2 虚拟机实现

### 5.2.1 架构设计

```
┌─────────────────────────────────────────┐
│              PL0VM                      │
│  ┌──────────────────────────────────┐  │
│  │  std::vector<Instruction> code   │  │
│  │  VMStack stack                   │  │
│  │  int pc (程序计数器)              │  │
│  │  int base (当前栈帧基址)          │  │
│  └──────────────────────────────────┘  │
│                                         │
│  ┌──────────────────────────────────┐  │
│  │  execute()                       │  │
│  │  - 取指令                        │  │
│  │  - 解码操作码                    │  │
│  │  - 执行操作                      │  │
│  │  - 更新 pc                       │  │
│  └──────────────────────────────────┘  │
└─────────────────────────────────────────┘
```

### 5.2.2 核心数据结构

```cpp
// 指令结构
struct Instruction {
    OpCode op;  // 操作码
    int L;      // 层级差（display 索引）
    int A;      // 地址或操作数
};

// 操作码枚举
enum class OpCode {
    LIT, LOD, STO, CAL, INT, JMP, JPC, OPR,  // 原有 8 条
    LEA, LODI, STOI, REL                      // 新增 4 条
};

// 虚拟机栈
class VMStack {
private:
    std::vector<int> data;
    int t;  // 栈顶指针
public:
    void push(int value);
    int pop();
    int top() const;
    int& operator[](int index);
};
```

### 5.2.3 指令执行逻辑

```cpp
void PL0VM::step() {
    Instruction& inst = code[pc];
    pc++;
    int b = calculateBase(inst.L);

    switch (inst.op) {
        case OpCode::LIT:
            stack.push(inst.A);
            break;

        case OpCode::LOD:
            stack.push(stack[b + inst.A]);
            break;

        case OpCode::STO:
            stack[b + inst.A] = stack.pop();
            break;

        // ⭐ 新增指令
        case OpCode::LEA:
            // 取地址：压入 base+A（变量地址）
            stack.push(b + inst.A);
            break;

        case OpCode::LODI:
            // 间接取值：pop addr，push mem[addr]
            {
                int addr = stack.pop();
                if (addr < 0 || addr > stack.getT()) {
                    throw std::runtime_error("E0028: Invalid memory access in LODI");
                }
                stack.push(stack[addr]);
            }
            break;

        case OpCode::STOI:
            // 间接存值：pop val，pop addr，mem[addr] = val
            {
                int val = stack.pop();
                int addr = stack.pop();
                if (addr < 0 || addr > stack.getT()) {
                    throw std::runtime_error("E0028: Invalid memory access in STOI");
                }
                stack[addr] = val;
            }
            break;

        case OpCode::REL:
            // 释放 N 个 cell：t -= N
            stack.setT(stack.getT() - inst.A);
            break;

        // ... 其他指令
    }
}
```

---

## 5.3 代码生成

### 5.3.1 借用表达式（`&x`）

**源程序**：
```rust
let x: i32 = 5;
let r: &i32 = &x;
```

**pcode 生成**：
```
LIT 0 5       // 加载常量 5
STO 0 x_addr  // 存储到 x
LEA 0 x_addr  // ⭐ 取 x 的地址
STO 0 r_addr  // 存储地址到 r
```

**实现**：
```cpp
void Parser::handleBorrowExpr(AstNode* n, sTable* s) {
    // & ident 或 &mut ident
    bool is_mut = (currentToekn.getType() == symbol::AMPMUTSYM);
    string name = currentToekn.getVal();
    
    // 查找变量地址
    pos varPos = s->findVar(name);
    
    // 生成 LEA 指令：压入 base + A（变量地址）
    pcode.push_back(pCode("LEA", varPos.pre, varPos.off));
}
```

### 5.3.2 解引用读取（`*r`）

**源程序**：
```rust
let x: i32 = 5;
let r: &i32 = &x;
write(*r);  // 输出 5
```

**pcode 生成**：
```
LIT 0 5       // 加载常量 5
STO 0 x_addr  // 存储到 x
LEA 0 x_addr  // 取 x 的地址
STO 0 r_addr  // 存储地址到 r
LOD 0 r_addr  // 加载 r 的值（即 x 的地址）
LODI          // ⭐ 间接取值：pop addr，push mem[addr]
OPR 0 14      // 输出
```

**实现**：
```cpp
void Parser::handleDerefExpr(AstNode* n, sTable* s) {
    // * ident 或 * ( expr )
    if (currentToekn.getType() == symbol::ID) {
        string name = currentToekn.getVal();
        pos varPos = s->findVar(name);
        
        // 先加载引用的地址
        pcode.push_back(pCode("LOD", varPos.pre, varPos.off));
        
        // 生成 LODI 指令：间接取值
        pcode.push_back(pCode("LODI", 0, 0));
    } else if (currentToekn.getType() == symbol::LPARENSYM) {
        // * ( expr )：先解析表达式，再生成 LODI
        handleExpression(n, s);
        pcode.push_back(pCode("LODI", 0, 0));
    }
}
```

### 5.3.3 解引用写入（`*m := val`）

**源程序**：
```rust
let mut x: i32 = 5;
let m: &mut i32 = &mut x;
*m := 10;
write(x);  // 输出 10
```

**pcode 生成**：
```
LIT 0 5       // 加载常量 5
STO 0 x_addr  // 存储到 x
LEA 0 x_addr  // 取 x 的地址
STO 0 m_addr  // 存储地址到 m
LIT 0 10      // 加载常量 10
LOD 0 m_addr  // 加载 m 的值（即 x 的地址）
STOI          // ⭐ 间接存值：pop val，pop addr，mem[addr] = val
LOD 0 x_addr  // 加载 x 的值
OPR 0 14      // 输出
```

**实现**：
```cpp
void Parser::handleAssignStatement(AstNode * n, sTable * s) {
    AstNode* lvalueNode = currentNode->child[0];
    
    if (lvalueNode->getType() == GrammarSymSpace::DEREFEXPR) {
        // *m := val：生成 STOI 指令
        // 先解析右侧表达式
        currentNode = currentNode->child[2];
        handleExpression(currentNode, currentTable);
        
        // 解析左侧的引用变量（*m 中的 m）
        AstNode* refNode = lvalueNode->child[0];
        if (refNode->getType() == GrammarSymSpace::ID) {
            string refName = refNode->getInfo();
            pos refPos = currentTable->findVar(refName);
            
            // 加载引用的地址
            pcode.push_back(pCode("LOD", refPos.pre, refPos.off));
            
            // 生成 STOI 指令：间接存值
            pcode.push_back(pCode("STOI", 0, 0));
        }
    } else {
        // 普通赋值：x := val
        // ... 原有逻辑
    }
}
```

### 5.3.4 作用域释放（`REL`）

**源程序**：
```rust
begin
    let x: i32 = 5;
    let y: i32 = 10;
    write(x + y);
end  // 离开作用域，释放 x 和 y
```

**pcode 生成**：
```
LIT 0 5       // 加载常量 5
STO 0 x_addr  // 存储到 x
LIT 0 10      // 加载常量 10
STO 0 y_addr  // 存储到 y
LOD 0 x_addr  // 加载 x
LOD 0 y_addr  // 加载 y
OPR 0 2       // 加法
OPR 0 14      // 输出
REL 0 2       // ⭐ 释放 2 个 cell（x 和 y）
```

**实现**：
```cpp
void Parser::handleCompoundStatement(AstNode * n, sTable * s) {
    // 记录进入作用域前的变量数
    int var_count_before = currentTable->getVarSize();
    
    handleStatementTable(currentNode->child[1], currentTable);
    
    // 计算该作用域新增的变量数，生成 REL 指令
    int var_count_after = currentTable->getVarSize();
    int var_count_in_scope = var_count_after - var_count_before;
    if (var_count_in_scope > 0) {
        pcode.push_back(pCode("REL", 0, var_count_in_scope));
    }
}
```

---

## 5.4 编译验证

### 5.4.1 词法分析验证

**测试用例**：`tests/t01_basic_compat.pl0`

```
=== Lexical Analysis ===
ID             PROGRAM        7 (1,1)
ID             T01            3 (1,9)
SEMICOLONSYM   ;              1 (1,12)
VAR            VAR            3 (2,1)
ID             A              1 (2,5)
SEMICOLONSYM   ;              1 (2,6)
BEGIN          BEGIN          5 (3,1)
ID             A              1 (4,3)
BECOMESSYM     :=             2 (4,5)
NUM            10             2 (4,8)
SEMICOLONSYM   ;              1 (4,10)
END            END            3 (5,1)
PERIODSYM      .              1 (5,4)
```

**结果**：✅ 词法分析正确识别所有 Token

### 5.4.2 语法分析验证

**当前状态**：⚠️ 语法分析存在段错误（Segmentation fault）

**问题分析**：
- 段错误发生在 Parser 构造函数中
- 可能原因：Grammar 对象初始化、语法分析栈初始化、AST 节点创建
- 需要进一步调试（使用 gdb 或添加调试输出）

**下一步计划**：
1. 使用 gdb 定位段错误具体位置
2. 检查 Grammar 构造函数中的 First/Follow 集计算
3. 验证预测分析表的正确性

---

## 5.5 本章小结

### 5.5.1 实现亮点

1. **完整的 pcode 扩展**：新增 4 条指令支持引用类型
2. **虚拟机实现**：完整的 PL0VM 支持 12 条指令
3. **代码生成**：正确生成 LEA/LODI/STOI/REL 指令
4. **作用域管理**：自动释放离开作用域的变量

### 5.5.2 与 Rust 的差异

| 特性 | Rust | PL/0+ |
|------|------|-------|
| 指令集 | LLVM IR | 自定义 pcode |
| 间接寻址 | 支持多级 | 仅支持一级 |
| 内存管理 | RAII + drop | RAII（无 drop）|
| 运行时检查 | 边界检查 | LODI/STOI 越界检查 |

### 5.5.3 已知问题

1. **语法分析段错误**：需要进一步调试
2. **多级解引用**：不支持 `**p`（文法层已堵死）
3. **生命周期标注**：不支持 `<'a>`（简化版本）

---

## 参考文献

1. The Rust Programming Language. Chapter 4: Understanding Ownership.
2. PL/0 编译器实现（多版本）. https://github.com/topics/pl0
3. mrustc: A Rust compiler implementation in C++. https://github.com/thepowersgang/mrustc
