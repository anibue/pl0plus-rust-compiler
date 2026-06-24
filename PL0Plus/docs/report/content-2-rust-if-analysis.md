# 内容 2：Rust if 条件语句的词法/语法分析

> 关联 [Spec §7 EBNF 文法](../../superpowers/specs/2026-06-17-pl0-rust-compiler-spec.md)

---

## 3.1 词法分析原理

### 3.1.1 词法分析的本质

**词法分析（Lexical Analysis）**是编译的第一阶段，任务是把**字符流**转换为**Token 流**。一个完整的词法分析器需要回答两个核心问题：

| 问题 | 答案 |
|------|------|
| 怎么把字符归并成 Token？ | **有限状态机**（DFA/NFA）|
| 怎么区分相似字符序列？ | **最长匹配**（maximal-munch）+ **关键字表** |

以 `if` 关键字为例，词法分析器要能区分：
- `if`（关键字）
- `if_x`（标识符，词法上独立）
- `i` `f`（两个独立标识符？永远不——会触发 maximal-munch）

### 3.1.2 词法分析的实现方式

主流实现有 3 种：

| 方式 | 特点 | 代表 |
|------|------|------|
| **手写状态机** | 灵活、可针对语言优化 | rustc, mrustc |
| **正则表达式 + 自动生成** | 自动化、但灵活性差 | lex/flex |
| **手写 + 表驱动** | 性能与灵活兼顾 | V8（部分）|

mrustc 采用**手写状态机**，与 rustc 类似。

### 3.1.3 mrustc 词法层分析（参考）

**文件位置**：`mrustc/src/parse/lex.cpp`（1392 行）

**核心数据结构**：

```cpp
// eTokenType.enum.h
enum class TokenType {
    // 关键字
    TOK_RWORD_IF,    // if
    TOK_RWORD_ELSE,  // else
    // ... 其他 200+ token
};

// lex.cpp
// 关键字表
TOKENT("if", TOK_RWORD_IF),
TOKENT("let", TOK_RWORD_LET),
TOKENT("fn",  TOK_RWORD_FN),
```

**关键字识别流程**：

```cpp
Token Lexer::consume_identifier_or_keyword() {
    // 1. 读取连续的 [a-zA-Z0-9_]
    // 2. 查关键字表 table["if"] → TOK_RWORD_IF
    // 3. 查不到则视为标识符 TOK_IDENT
}
```

这是经典的"**查表法**"实现：
- 优点：O(1) 查找，简单清晰
- 缺点：每加一个关键字都要修改表

### 3.1.4 mrustc 的 if 关键字识别（具体代码）

```cpp
// 关键字映射表（节选）
TOKENT("if",      TOK_RWORD_IF),     // 行 172
TOKENT("else",    TOK_RWORD_ELSE),
TOKENT("let",     TOK_RWORD_LET),
TOKENT("for",     TOK_RWORD_FOR),
TOKENT("while",   TOK_RWORD_WHILE),
TOKENT("loop",    TOK_RWORD_LOOP),
TOKENT("match",   TOK_RWORD_MATCH),
```

注意：Rust 的关键字表非常长（200+），因为 Rust 语法丰富（含 `async` `await` `dyn` `where` 等）。

### 3.1.5 PL/0+ 词法层（我们的实现）

按 spec §4，我们保留原 PL/0 的 35 个 Token，新增 8 个 Rust Token（let/mut/i8/i16/i32/&/&mut/:）。

**关键设计**：
- `&mut` 识别为**单 token** `AMPMUTSYM`（maximal-munch）
- 词法层加 `AMP` 状态（前瞻 1 个字符判断是 `&` 还是 `&mut`）

```cpp
// 词法层伪代码
if (current_char == '&') {
    enter_state(AMP);
    next_char();
    if (current_char == 'm' && peek() == 'u' && peek2() == 't') {
        return Token(AMPMUTSYM);
    } else {
        return Token(AMPSYM);
    }
}
```

### 3.1.6 词法层总结

| 维度 | mrustc | PL/0+ |
|------|--------|-------|
| 关键字数 | ~200+ | 43 (35+8) |
| 实现方式 | 手写状态机 | 手写状态机 |
| 关键字表 | 查表法 | 查表法 |
| 特殊 token | `&mut`/`&&` | `&mut` 单 token |
| 状态机 | DFA | DFA + AMP 前瞻 |

---

## 3.2 语法分析原理

### 3.2.1 语法分析的本质

**语法分析（Syntax Analysis）**是编译的第二阶段，任务是把 **Token 流**转换为 **AST（抽象语法树）**。

需要回答的核心问题：
- 当前 Token 序列是否符合语言的**文法**？
- 如何**结构化**表示一个程序的语法？

主流方法：

| 方法 | 文法类型 | 实现难度 | 用途 |
|------|---------|---------|------|
| 递归下降 | LL(k) | 简单 | 教学、小型编译器 |
| LL(1) 预测分析 | LL(1) | 中 | PL/0、PL0+ |
| LR(1) | LR(1) | 复杂 | yacc/bison、工业编译器 |
| LALR(1) | LALR(1) | 复杂 | rustc（部分）|

mrustc 用**手写递归下降**，rustc 用**LALR(1) + 手写递归下降**混合，PL/0+ 用**LL(1) 预测分析**。

### 3.2.2 LL(1) 文法

**LL(1)** = 从左到右扫描、Leftmost 推导、1 个 lookahead token。

**判定条件**：
- 对每个非终结符 A 的每个产生式 A → α | β
- 满足 `FIRST(α) ∩ FIRST(β) = ∅`
- 或能通过 **FOLLOW 集**区分

PL/0 文法天然是 LL(1) 的（设计上保证了这一点），所以 PL/0+ 继承这个特性。

### 3.2.3 预测分析表

LL(1) 文法 → 构造 **预测分析表 M[A, t]**：
- 行：非终结符 A
- 列：终结符 t
- 值：A 在看到 t 时选哪个产生式

**构造算法**（教科书内容）：
```
对每个产生式 A → α：
  对每个 t ∈ FIRST(α)：
    M[A, t] = A → α
  如果 ε ∈ FIRST(α)：
    对每个 t ∈ FOLLOW(A)：
      M[A, t] = A → α
```

PL/0+ 继承 PL/0 的预测分析表（已有 ~80 个条目）。

### 3.2.4 mrustc 语法层分析（参考）

**文件位置**：`mrustc/src/parse/expr.cpp`（1647 行）

mrustc 的 if 语句解析（**关键代码**）：

```cpp
/// Parse an 'if' statement
// Note: TOK_RWORD_IF has already been eaten
ExprNodeP Parse_IfStmt(TokenStream& lex)
{
    TRACE_FUNCTION;

    Token   tok;
    std::vector<AST::ExprNode_If::Arm>  arms;
    ExprNodeP else_block;
    do {
        std::vector<AST::IfLet_Condition>   conditions;

        {
            SET_PARSE_FLAG(lex, disallow_struct_literal);
            conditions = Parse_IfLetChain(lex);
        }

        // Contents
        ExprNodeP code = Parse_ExprBlockNode(lex);

        arms.push_back(AST::ExprNode_If::Arm {
            std::move(conditions),
            std::move(code)
        });

        // Handle else:
        if( !lex.getTokenIf(TOK_RWORD_ELSE) ) {
            // No `else`, leave `else_block` as `nullptr`
            break;
        }
        // Recurse for 'else if'
        if( !lex.getTokenIf(TOK_RWORD_IF) ) {
            else_block= Parse_ExprBlockNode(lex);
            break;
        }
        // Keep looping
    } while(true);

    return NEWNODE( AST::ExprNode_If, ::std::move(arms), ::std::move(else_block) );
}
```

### 3.2.5 mrustc if 语句的设计亮点

1. **AST 结构 `ExprNode_If`**：
   ```cpp
   struct ExprNode_If {
       struct Arm {
           vector<IfLet_Condition> conditions;  // 条件
           ExprNodeP code;                         // 分支体
       };
       vector<Arm> arms;       // 多个 if / else if 分支
       ExprNodeP else_block;   // 终态 else
   };
   ```
   - 用 `vector<Arm>` 而非链表，支持任意个 `else if`
   - `else_block` 是可空的（if 可以没 else）

2. **`if let` 链式支持**：
   - 解析 `if let pat = val { ... } else if let ... { ... }`
   - 用 `Parse_IfLetChain` 解析条件链

3. **else if 链式递归**：
   - `else if` 走 `do-while` 循环
   - `else` 走 break 分支
   - 这种结构避免了深度递归的栈溢出

4. **注释 "TOK_RWORD_IF has already been eaten"**：
   - 表明 `if` 已经在更外层被消费过
   - 这是常见的递归下降模式（外层 dispatcher 已经分流）

### 3.2.6 PL/0+ 语法层（我们的实现）

按 spec §7，PL/0+ 继承 PL/0 的 LL(1) 框架。

**if 语句的 EBNF**：

```ebnf
ifSt = "if" condition "then" statement [ "else" statement ] .
```

**与 mrustc 对比**：

| 维度 | mrustc | PL/0+ |
|------|--------|-------|
| 方法 | 递归下降 | LL(1) 预测分析 |
| if let | ✅ 支持 | ❌ 不支持（简化）|
| else if 链 | ✅ 任意个 | ✅（递归 else 部分）|
| AST | `ExprNode_If::Arm[]` | `AstNodeIf` 单节点 |
| 条件 | 任意表达式 | 必须是 `condition` |

### 3.2.7 语法层总结

| 维度 | mrustc | PL/0+ |
|------|--------|-------|
| 分析方法 | 递归下降 | LL(1) 预测分析 |
| if 产生式 | 复杂（支持 if let）| 简单（PL/0 风格）|
| 实现 | do-while 循环 | 预测分析表 + 状态机 |
| 错误恢复 | panic recover | panic 模式（spec §8.3）|
| 行号追踪 | 手动 SET_PARSE_FLAG | Position 类 |

---

## 3.3 if 语句的 EBNF 文法（PL/0+）

### 3.3.1 完整 if 产生式

```ebnf
(* 在 statement 中引入 ifSt *)
statement = assignSt
          | callSt
          | compoundSt
          | ifSt                      (* ⭐ if 语句 *)
          | whileSt
          | repeatSt
          | readSt
          | writeSt
          | letDecl
          .

(* if 语句本身 *)
ifSt = "if" condition "then" statement [ "else" statement ] .

(* condition 是 PL/0 风格：odd expr | expr relop expr *)
condition = "odd" expression
          | expression relop expression .
relop = "=" | "<>" | "<" | "<=" | ">" | ">=" .
```

### 3.3.2 关键决策

| 决策 | 选择 | 理由 |
|------|------|------|
| if 是语句还是表达式？ | **语句** | 保持 PL/0 风格 |
| condition 是表达式还是布尔？ | **PL/0 风格条件** | 简单，用 relop 比较 |
| else if 链 | **递归** | `else statement` 中的 statement 可以又是 ifSt |
| 嵌套 if | ✅ 支持 | 文法天然允许 |

### 3.3.3 推导示例：`if x > 0 then write(1) else write(-1)`

```
ifSt ⇒ "if" condition "then" statement "else" statement
     ⇒ "if" expression relop expression "then" statement "else" statement
     ⇒ "if" ident ">" number "then" "write" "(" expression ")" "else" ...
     ⇒ "if" "x" ">" "0" "then" "write" "(" number ")" "else" "write" "(" "0" "−" number ")"
```

### 3.3.4 与 Rust 真实 if 对比

| Rust 真实 if | PL/0+ if |
|--------------|----------|
| `if expr { ... } else { ... }` | `if condition then statement else statement` |
| 条件是任意 bool 表达式 | 条件是 `odd expr` 或 `expr relop expr` |
| 表达式可返回值 | 只能是语句 |
| `if let` 模式匹配 | ❌ 不支持 |

PL/0+ 简化了 Rust 的 if：
- 条件必须是 `condition`（PL/0 风格）
- 没有花括号，用 `then` / `end` 关键字
- 没有 if let 模式匹配

这些简化**不影响课程目标**（展示 Rust 编译原理的核心理念），但**让实现可控**。

---

## 3.4 本章小结

### 3.4.1 内容 2 完成度

| 任务 | 状态 |
|------|------|
| 词法分析原理讲解 | ✅ |
| 词法层 mrustc 参考 | ✅ |
| 语法分析原理讲解 | ✅ |
| 语法层 mrustc 参考 | ✅ |
| PL/0+ 词法层设计 | ✅ |
| PL/0+ 语法层设计 | ✅ |
| if 语句 EBNF 文法 | ✅ |

### 3.4.2 关键收获

1. **词法层**是简单的字符 → Token 映射
   - 关键：maximal-munch + 关键字表
   - 我们的创新：`&mut` 识别为单 token（maximal-munch）

2. **语法层**是 Token → AST 映射
   - 关键：选择分析方法（递归下降 vs LL(1) vs LR）
   - PL/0+ 选择 LL(1) 是因为 PL/0 文法天然适配

3. **Rust if 的设计**比 PL/0 复杂
   - 支持 if let 模式
   - 条件可以是任意 bool
   - 表达式返回值
   - 任意个 else if 链
   - PL/0+ 简化了这些，保持教学合理性

4. **从 mrustc 学到的技巧**：
   - 用 `vector<Arm>` 表达 else if 链
   - 用 `do-while` 循环解析链式结构
   - "TOKEN has already been eaten" 模式
   - 注释清晰（TRACE_FUNCTION）

### 3.4.3 后续衔接

本章的词法/语法分析为**内容 3**（PL/0 改造）打下基础：
- 词法层新增的 8 个 Token（let/mut/i8/i16/i32/&/&mut/:）将在 W2 实现
- 语法层新增的 `letDecl` 产生式将在 W2 实现
- 借用检查的拦截点（spec §8.5）依赖本章的 AST 结构

---

## 参考文献

1. Aho, Lam, Sethi, Ullman. 《编译原理（原书第 2 版）》. 机械工业出版社, 2009. (龙书)
2. Niklaus Wirth. 《编译器构造》. 清华大学出版社, 2005.
3. thepowersgang. mrustc. https://github.com/thepowersgang/mrustc
4. Rust Language Reference. https://doc.rust-lang.org/reference/
5. PL/0 编译器实现（多版本）. https://github.com/topics/pl0
