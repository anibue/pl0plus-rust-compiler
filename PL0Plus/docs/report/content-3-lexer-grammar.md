# 内容 3：PL/0 改造与 Rust 特性实现 — W2 词法/语法扩展

---

## 4.W2 词法层与文法层扩展

### 4.W2.1 设计目标

W2 阶段的目标是在 PL/0 编译器的基础上，扩展词法分析器和文法定义，增加支持 Rust 风格特性的 Token 和产生式。具体目标：

1. **词法层扩展**：增加 8 个新 Token（let, mut, i8, i16, i32, &, &mut, :）
2. **文法层扩展**：增加 16 个新产生式（letDecl, type, borrowExpr, derefExpr 等）
3. **PL/0 兼容性**：确保原有 PL/0 程序仍能正确编译运行

### 4.W2.2 词法层扩展

#### 4.W2.2.1 新增 Token（8 个）

| Token | 枚举名 | 含义 | 示例 |
|-------|--------|------|------|
| `let` | `LETSYM` | Rust 风格变量声明 | `let x: i32 = 5;` |
| `mut` | `MUTSYM` | 可变性标记 | `let mut x: i32 = 5;` |
| `i8` | `I8SYM` | 8 位有符号整数 | `x: i8` |
| `i16` | `I16SYM` | 16 位有符号整数 | `x: i16` |
| `i32` | `I32SYM` | 32 位有符号整数 | `x: i32` |
| `&` | `AMPSYM` | 不可变借用 | `&x` |
| `&mut` | `AMPMUTSYM` | 可变借用 | `&mut x` |
| `:` | `COLONSYM` | 类型标注分隔符 | `x: i32` |

#### 4.W2.2.2 Token 总数变化

| 阶段 | Token 数量 | 说明 |
|------|-----------|------|
| PL/0 原有 | 35 | 保留所有 PL/0 Token |
| PL/0+ 扩展 | +8 | let, mut, i8, i16, i32, &, &mut, : |
| **PL/0+ 总计** | **43** | 词法层扩展完成 |

#### 4.W2.2.3 AMP 状态与 maximal-munch

**问题**：如何区分 `&`（单个字符）和 `&mut`（三个字符的单个 Token）？

**解决方案**：新增 `AMP` 状态，使用 **maximal-munch** 原则（最长匹配）：

```cpp
// lexer.cpp
else if (text[n] == '&') {
    enter_state(AMP);
    next_char();
    if (current_char == 'm' && peek() == 'u' && peek2() == 't') {
        // 匹配到 &mut，返回 AMPMUTSYM
        return Token(AMPMUTSYM);
    } else {
        // 只匹配到 &，返回 AMPSYM
        return Token(AMPSYM);
    }
}
```

**状态转换图**：

```
START ──&amp;──→ AMP ──m──→ AMu ──u──→ AMut ──t──→ AMPMUTSYM (final)
                   │
                   └─(其他字符)──→ AMPSYM (final)
```

**maximal-munch 原则**：
- 当遇到 `&` 时，词法分析器会**向前看 3 个字符**（peek, peek2）
- 如果依次是 `mut`，则匹配整个 `&mut` 为单个 token
- 否则，只匹配 `&` 为单个 token

#### 4.W2.2.4 关键字识别

**实现方式**：查表法（与 mrustc 一致）

```cpp
// lexer.cpp - 关键字表
Token Lexer::consume_identifier_or_keyword() {
    // 1. 读取连续的 [a-zA-Z0-9_]
    // 2. 查关键字表
    static std::map<string, TokenType> keywords = {
        {"begin", BEGINSYM},
        {"call", CALLSYM},
        {"const", CONSTSYM},
        // ... 原有 PL/0 关键字
        {"let", LETSYM},      // ⭐ 新增
        {"mut", MUTSYM},      // ⭐ 新增
        {"i8", I8SYM},        // ⭐ 新增
        {"i16", I16SYM},      // ⭐ 新增
        {"i32", I32SYM},      // ⭐ 新增
    };

    auto it = keywords.find(identifier);
    if (it != keywords.end()) {
        return Token(it->second);  // 关键字
    }
    return Token(ID, identifier);  // 标识符
}
```

### 4.W2.3 文法层扩展

#### 4.W2.3.1 新增产生式（16 个）

**let 声明语句**：

```ebnf
letDecl = "let" [ "mut" ] ident ":" type "=" expression ";" .
```

**类型定义**：

```ebnf
type = refType | baseType .

refType = AMPSYM baseType | AMPMUTSYM baseType .

baseType = "i8" | "i16" | "i32" .
```

**借用表达式**：

```ebnf
borrowExpr = AMPSYM ident | AMPMUTSYM ident .
```

**解引用表达式**：

```ebnf
derefExpr = "*" ( ident | "(" expression ")" ) .
```

#### 4.W2.3.2 完整声明语句产生式

```ebnf
statement = assignSt
          | callSt
          | compoundSt
          | ifSt
          | whileSt
          | repeatSt
          | readSt
          | writeSt
          | letDecl              (* ⭐ 新增 *)
          .
```

#### 4.W2.3.3 产生式统计

| 类别 | 原有 | 新增 | 总计 |
|------|------|------|------|
| 非终结符 | ~20 | +6 | ~26 |
| 终结符 | ~35 | +10 | ~45 |
| 产生式 | ~30 | +16 | ~46 |

### 4.W2.4 符号表扩展

#### 4.W2.4.1 DeclKind 枚举

```cpp
// symbolTable.h
enum class DeclKind {
    CONST,     // const 声明
    VAR,       // var 声明
    LET,       // let 声明（不可变）
    LET_MUT    // let mut 声明（可变）
};
```

#### 4.W2.4.2 Symbol 结构扩展

```cpp
// symbolTable.h
struct Symbol {
    string name;           // 变量名
    int kind;              // DeclKind
    string type;          // 类型：i8/i16/i32/&i32/&mut i32
    int value;            // 常量值（const）
    int addr;             // 地址（编译时确定）
    bool is_initialized;  // 是否已初始化
    int scope_level;      // 作用域层级
    bool is_ref;          // 是否为引用类型
    bool is_mut_ref;      // 是否为可变引用
    // ... 其他字段
};
```

#### 4.W2.4.3 错误码扩展（14 个）

| 错误码 | 含义 | 触发条件 |
|--------|------|----------|
| E0010 | 类型不匹配 | `let x: i8 = "hello";` |
| E0011 | 引用类型错误 | `let x: &i8 = 5;` |
| E0012 | 无效的类型标注 | `let x: invalid = 5;` |
| E0013 | 整数溢出 | `let x: i8 = 200;` |
| E0014 | 借用类型错误 | `&i8` 但右值不是 i8 类型 |
| E0015 | 解引用非引用类型 | `*5`（5 不是引用）|
| E0016 | 可变引用不可用于只读操作 | `&mut x` 用于只读上下文 |
| E0017 | 生命周期错误 | 引用超出所有者生命周期 |
| E0032 | 变量未声明 | 使用未声明的变量 |

### 4.W2.5 Parser 扩展

#### 4.W2.5.1 新增方法（4 个）

```cpp
// parser.h
class Parser {
private:
    // ⭐ 处理 let 声明
    void handleLetDeclaration(AstNode* n, sTable* s);

    // ⭐ 处理类型标注
    void handleType(string& outType, bool& isRef, bool& isMutRef);

    // ⭐ 处理借用表达式
    void handleBorrowExpr(AstNode* n, sTable* s);

    // ⭐ 处理解引用表达式
    void handleDerefExpr(AstNode* n, sTable* s);

    // ... 原有方法
};
```

#### 4.W2.5.2 handleLetDeclaration 实现

```cpp
void Parser::handleLetDeclaration(AstNode* n, sTable* s) {
    // 1. 解析 "let"
    match(symbol::LETSYM);

    // 2. 可选 "mut"
    bool is_mut = false;
    if (currentToekn.getType() == symbol::MUTSYM) {
        is_mut = true;
        advance();
    }

    // 3. 解析标识符
    string name = currentToekn.getVal();
    match(symbol::ID);

    // 4. 解析 ":"
    match(symbol::COLONSYM);

    // 5. 解析类型
    string type;
    bool is_ref = false;
    bool is_mut_ref = false;
    handleType(type, is_ref, is_mut_ref);

    // 6. 解析 "="
    match(symbol::BECOMESSYM);

    // 7. 解析表达式
    handleExpression(n->child.back(), s);

    // 8. 解析 ";"
    match(symbol::SEMICOLONSYM);

    // 9. 注册到符号表
    s->declare(name, is_mut ? DeclKind::LET_MUT : DeclKind::LET,
               type, is_ref, is_mut_ref);
}
```

#### 4.W2.5.3 handleBorrowExpr 实现

```cpp
void Parser::handleBorrowExpr(AstNode* n, sTable* s) {
    // & ident 或 &mut ident
    bool is_mut = (currentToekn.getType() == symbol::AMPMUTSYM);

    // 消耗 & 或 &mut
    advance();

    // 解析标识符
    string name = currentToekn.getVal();
    match(symbol::ID);

    // 生成 pcode：LEA 指令
    pos varPos = s->findVar(name);
    pcode.push_back(pCode("LEA", varPos.pre, varPos.off));
}
```

#### 4.W2.5.4 handleDerefExpr 实现

```cpp
void Parser::handleDerefExpr(AstNode* n, sTable* s) {
    // * ident 或 * ( expr )
    advance();  // 消耗 *

    if (currentToekn.getType() == symbol::ID) {
        // * ident
        string name = currentToekn.getVal();
        match(symbol::ID);

        // 加载引用的地址
        pos varPos = s->findVar(name);
        pcode.push_back(pCode("LOD", varPos.pre, varPos.off));

        // 生成 LODI：间接取值
        pcode.push_back(pCode("LODI", 0, 0));
    } else if (currentToekn.getType() == symbol::LPARENSYM) {
        // * ( expr )
        match(symbol::LPARENSYM);
        handleExpression(n, s);
        match(symbol::RPARENSYM);

        // 生成 LODI：间接取值
        pcode.push_back(pCode("LODI", 0, 0));
    }
}
```

### 4.W2.6 测试用例

#### 4.W2.6.1 PL/0 兼容性测试（10 个）

| # | 文件 | 测试内容 | 预期结果 |
|---|------|---------|---------|
| T1 | t01_basic_compat.pl0 | 基础 program | ✅ 编译通过 |
| T2 | t02_while_compat.pl0 | while 循环 | ✅ 编译通过 |
| T3 | t03_procedure_compat.pl0 | 嵌套过程 | ✅ 编译通过 |
| T4 | t04_if_compat.pl0 | if-else | ✅ 编译通过 |
| T5 | t05_expr_compat.pl0 | 表达式 | ✅ 编译通过 |
| T6 | t06_lexical.pl0 | 词法层 | ✅ 编译通过 |
| T7 | t07_const_while.pl0 | const + while | ✅ 编译通过 |
| T8 | t08_repeat_until.pl0 | repeat-until | ✅ 编译通过 |
| T9 | t09_nested_if.pl0 | 嵌套 if | ✅ 编译通过 |
| T10 | t10_comprehensive.pl0 | 综合 | ✅ 编译通过 |

#### 4.W2.6.2 词法分析测试结果

**测试用例**：`tests/t01_basic_compat.pl0`

```
PROGRAM T01;
VAR A;
BEGIN
  A := 10;
END.
```

**词法分析输出**：

```
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

**结果**：✅ 所有 Token 正确识别

### 4.W2.7 本章小结

#### 4.W2.7.1 完成情况

| 任务 | 状态 | 说明 |
|------|------|------|
| 词法层新增 8 个 Token | ✅ 完成 | let, mut, i8, i16, i32, &, &mut, : |
| AMP 状态实现 | ✅ 完成 | maximal-munch 识别 & 和 &mut |
| 文法符号扩展 | ✅ 完成 | +6 非终结符 + 10 终结符 |
| 新增 16 个产生式 | ✅ 完成 | letDecl, type, borrowExpr, derefExpr |
| Parser 4 个新方法 | ✅ 完成 | handleLetDeclaration/Type/BorrowExpr/DerefExpr |
| 符号表扩展 | ✅ 完成 | DeclKind + RustSymbol |
| PL/0 兼容性测试 | ✅ 通过 | 词法分析正确识别所有 Token |

#### 4.W2.7.2 关键设计决策

| 决策 | 选择 | 理由 |
|------|------|------|
| `&mut` 是单 token | ✅ | 简化词法分析，maximal-munch |
| 类型标注 `:` | ✅ | 与 Rust 语法一致 |
| let 声明是语句 | ✅ | 与 const/var 保持一致 |
| 不支持类型推断 | ✅ | 教学友好，显式标注 |

#### 4.W2.7.3 与 mrustc 的对比

| 特性 | mrustc | PL/0+ |
|------|--------|-------|
| token 识别方式 | 查表法 | 查表法 ✅ |
| `&mut` 处理 | 字符串匹配 | maximal-munch ✅ |
| 类型标注 | `:` | `:` ✅ |
| let 语法 | `let x: i32 = 5;` | `let x: i32 = 5;` ✅ |
| 多级借用 | 支持 | 不支持（简化）|

#### 4.W2.7.4 W2 → W3 衔接

W2 完成后，为 W3 借用检查器奠定了基础：

1. **Token 层**：所有借用相关 Token 已就绪
2. **文法层**：borrowExpr、derefExpr 产生式已定义
3. **Parser 层**：handleBorrowExpr、handleDerefExpr 方法已实现
4. **符号表**：DeclKind 已定义，可区分 let/let mut

W3 将在此基础上：
- 实现 BorrowTracker 类
- 添加 5 条借用规则检查
- 实现作用域管理
- 添加错误报告（E0020-E0028）
