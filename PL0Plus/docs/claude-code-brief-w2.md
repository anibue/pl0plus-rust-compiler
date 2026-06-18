# Claude Code 任务：B 阶段 W2 词法/语法实际实现

## 项目背景

这是 **PL/0+ Rust 编译器课程设计**项目。我已经做了：
- ✅ Spec 文档（650 行，52 条设计决策）
- ✅ PRD 文档
- ✅ Git 仓库初始化
- ✅ W1 内容 2 报告
- ✅ W2 部分工作（词法层 8 个 Token + AMP 状态 + 文法符号定义）

现在需要你**完成 W2 剩余的实际实现**。

## 工作目录

```
E:\rust_com\PL0Plus\
├── compiler/        # 你要修改的目录
│   ├── lexer.h/cpp    # ✅ 已扩展（8 个新 Token）
│   ├── parser.h/cpp   # ⚠️ 需要扩展（方法已声明，需要实现）
│   ├── grammar.h/cpp  # ⚠️ 需要扩展（产生式需要加）
│   ├── grammarDefinition.h  # ✅ 已扩展（枚举已加）
│   └── symbolTable.h/cpp  # ⚠️ 需要扩展（DeclKind + Symbol 字段）
├── tests/            # ✅ 已有 10 个 .pl0 测试
└── docs/superpowers/specs/2026-06-17-pl0-rust-compiler-design.md  # 完整 Spec
```

## 你的任务（4 个具体改动）

### 任务 1：symbolTable.h/cpp - 加 DeclKind + 扩展 Symbol

参考 Spec §6，添加：

```cpp
// symbolTable.h
enum class DeclKind {
    CONST,    // const x = 5;
    VAR,      // var x;
    LET,      // let x: i32 = 5;
    LET_MUT,  // let mut x: i32 = 5;
};

struct Symbol {
    string name;
    string type;       // "i8" | "i16" | "i32" | "&i32" | "&mut i32" | "integer"
    int    addr;
    int    scope_level;
    int    decl_line;
    int    first_assign_line;  // -1 = 未赋值
    bool   is_initialized;
    DeclKind decl_kind;
    int    borrow_count_imm;
    int    borrow_count_mut;
    
    // 辅助方法
    bool is_mutable() const;
    bool can_be_borrowed() const;
};
```

实现：
- `is_mutable()` 返回 `decl_kind == VAR || decl_kind == LET_MUT`
- `can_be_borrowed()` 返回 `decl_kind == LET || decl_kind == LET_MUT`

**不要破坏原有 Symbol 类的现有调用点**。

### 任务 2：grammar.cpp - 加 letDecl 产生式

参考 Spec §7.1 EBNF 文法：

```ebnf
letDecl      = "let" [ "mut" ] ident ":" type "=" expression ";" .
type         = refType | baseType .
refType      = AMPSYM baseType | AMPMUTSYM baseType .
baseType     = "i8" | "i16" | "i32" .
borrowExpr   = AMPSYM ident | AMPMUTSYM ident .
derefExpr    = "*" ( ident | "(" expression ")" ) .
```

注意：原 PL/0 已有 `factor` 产生式（处理 ident/number/括号/关系运算符），需要扩展加上 `borrowExpr` 和 `derefExpr`。

修改点：
- STATEMENT 产生式加 `letDecl`
- 产生式列表加 letDecl, type, refType, baseType, borrowExpr, derefExpr
- FACTOR 产生式加 borrowExpr 和 derefExpr
- 重新计算 First/Follow 集（可能需要新增非终结符：LET_DECLARATION）
- 更新预测分析表

### 任务 3：parser.cpp - 实现 handleLetDeclaration

参考 Spec §7 词法+语法衔接。实现方法：

```cpp
void Parser::handleLetDeclaration(AstNode* n, sTable* s) {
    // 当前 token 应该是 LETSYM（已消费）
    // 1. 可选 MUTSYM
    bool is_mut = false;
    if (currentToken().getType() == MUTSYM) {
        is_mut = true;
        advance();
    }
    
    // 2. ident
    string name = currentToken().getVal();
    expect(IDSYM);
    
    // 3. COLONSYM
    expect(COLONSYM);
    
    // 4. type
    string typeName;
    bool isRef = false, isMutRef = false;
    handleType(typeName, isRef, isMutRef);
    
    // 5. BECOMESSYM
    expect(BECOMESSYM);
    
    // 6. expression
    AstNode* expr = expression();
    
    // 7. SEMICOLONSYM
    expect(SEMICOLONSYM);
    
    // 8. 注册到符号表
    DeclKind kind = is_mut ? DeclKind::LET_MUT : DeclKind::LET;
    symTable->declare(name, typeName, kind, currentLine(), expr);
}
```

`handleType` 实现：

```cpp
void Parser::handleType(string& outType, bool& isRef, bool& isMutRef) {
    isRef = false;
    isMutRef = false;
    
    if (currentToken().getType() == AMPSYM || currentToken().getType() == AMPMUTSYM) {
        isRef = true;
        isMutRef = (currentToken().getType() == AMPMUTSYM);
        advance();
    }
    
    if (currentToken().getType() == I8SYM) { outType = "i8"; }
    else if (currentToken().getType() == I16SYM) { outType = "i16"; }
    else if (currentToken().getType() == I32SYM) { outType = "i32"; }
    else { /* error: expected type */ }
    advance();
}
```

也要在 `handleStatement` 中加 `case LETSYM` 调用 `handleLetDeclaration`。

### 任务 4：lexerTypeToGrammarType - 加 8 个 Token 映射

在 `parser.cpp` 的 `lexerTypeToGrammarType` 函数中加：

```cpp
case LETSYM:    return GrammarSymSpace::LET;
case MUTSYM:    return GrammarSymSpace::MUT;
case I8SYM:     return GrammarSymSpace::I8SYM;
case I16SYM:    return GrammarSymSpace::I16SYM;
case I32SYM:    return GrammarSymSpace::I32SYM;
case AMPSYM:    return GrammarSymSpace::AMPSYM;
case AMPMUTSYM: return GrammarSymSpace::AMPMUTSYM;
case COLONSYM:  return GrammarSymSpace::COLONSYM;
```

## 测试验证

完成后尝试编译（如果有 qmake/make）：

```bash
cd "E:\rust_com\PL0Plus"
qmake application.pro 2>&1
make 2>&1
```

如果编译失败，修复错误直到通过。`application.pro` 已经存在于该目录。

## 关键约束

1. **不要破坏现有 PL/0 兼容性** — 35 个原 Token 必须继续工作
2. **不要做 W3 工作** — 借用检查、5 条规则、错误恢复都不做（那是 W3）
3. **不要修改测试用例** — T1-T10 已经是合法的 PL/0
4. **不要删除任何现有功能** — 只增量添加
5. **参考 Spec** — 所有实现细节在 `docs/superpowers/specs/2026-06-17-pl0-rust-compiler-design.md`

## 完成标准

- [ ] `symbolTable.h` 加了 DeclKind 枚举和扩展的 Symbol 结构
- [ ] `grammar.cpp` 加了 letDecl/type/borrowExpr/derefExpr 产生式
- [ ] `parser.cpp` 实现了 handleLetDeclaration、handleType 等 4 个方法
- [ ] `lexerTypeToGrammarType` 加了 8 个 Token 映射
- [ ] 编译无错（如果能编译）

## 完成后请报告

1. 改了哪些文件，每个文件改了几行
2. 是否编译通过
3. 遇到的难点和解决方案
4. W3 需要从哪个接口开始

开始干活吧。
