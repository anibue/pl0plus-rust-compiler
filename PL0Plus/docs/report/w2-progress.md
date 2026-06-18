# W2 进度报告 — 词法/语法扩展

> 2026-06-17 | 关联 [Spec §4 词法层设计](../../superpowers/specs/2026-06-17-pl0-rust-compiler-spec.md)

## ✅ 已完成

| 任务 | 状态 | 文件 |
|------|------|------|
| 词法层新增 8 个 Token | ✅ | `compiler/lexer.h` |
| 词法 Types 数组扩展 | ✅ | `compiler/lexer.cpp` (35→43) |
| 5 个新关键字识别（let/mut/i8/i16/i32）| ✅ | `compiler/lexer.cpp` |
| AMP 状态（& / &mut maximal-munch）| ✅ | `compiler/lexer.cpp` |
| `:` 单独 token（COLONSYM）| ✅ | `compiler/lexer.cpp` |
| EOF 改名（避免 iostream 冲突）| ✅ | `compiler/lexer.cpp` |
| 文法符号扩展 | ✅ | `compiler/grammarDefinition.h` |
| Parser 4 个新方法声明 | ✅ | `compiler/parser.h` |
| 10 个 PL/0 兼容性测试用例 | ✅ | `PL0Plus/tests/t01-t10*.pl0` |

## 📊 变更统计

| 文件 | 变更 |
|------|------|
| `lexer.h` | 35 Token → 43 Token；新增 AMP 状态 |
| `lexer.cpp` | +5 关键字 + AMP 处理 + EOF_STATE 改名 |
| `grammarDefinition.h` | +6 非终结符 + 8 终结符 + 1 STAR |
| `parser.h` | +4 方法声明（handleLetDeclaration/handleType/handleBorrowExpr/handleDerefExpr）|
| `tests/` | 10 个 .pl0 文件 |

## 📝 下次会话要做的事（W2 收尾）

1. **grammar.cpp** - 添加 letDecl 产生式、First/Follow 集、预测分析表
2. **parser.cpp** - 实现 handleLetDeclaration、handleType、handleBorrowExpr、handleDerefExpr
3. **symbolTable.h/cpp** - 加 DeclKind 枚举、扩展 Symbol 结构
4. **编译测试** - 跑 PL0Plus.exe 看 t01-t10 是否通过

## 🧪 10 个测试用例（W2 范围）

| # | 文件 | 测试内容 | PL/0 兼容 | W2 状态 |
|---|------|----------|-----------|---------|
| T1 | t01_basic_compat.pl0 | 基础 program | ✅ | 待验证 |
| T2 | t02_while_compat.pl0 | while 循环 | ✅ | 待验证 |
| T3 | t03_procedure_compat.pl0 | 嵌套过程 | ✅ | 待验证 |
| T4 | t04_if_compat.pl0 | if-else | ✅ | 待验证 |
| T5 | t05_expr_compat.pl0 | 表达式 | ✅ | 待验证 |
| T6 | t06_lexical.pl0 | 词法层 | ✅ | 待验证 |
| T7 | t07_const_while.pl0 | const + while | ✅ | 待验证 |
| T8 | t08_repeat_until.pl0 | repeat-until | ✅ | 待验证 |
| T9 | t09_nested_if.pl0 | 嵌套 if | ✅ | 待验证 |
| T10 | t10_comprehensive.pl0 | 综合 | ✅ | 待验证 |

> 注意：W2 阶段**只验证 PL/0 兼容性**。Rust 特性（let/借用/解引用）由 W3 实现。

## 🔄 W2 → W3 衔接

W2 完成后，还需要：
1. `parser.cpp` 的 `handleStatement` 中加入 letDecl 分支
2. `lexerTypeToGrammarType` 加入 8 个新 Token 的映射
3. 整个语法分析栈要能处理 letDecl 产生式

W3 借用检查器依赖：
- Symbol 结构扩展（DeclKind 字段）
- AST 节点扩展（letDecl 节点、borrow 节点、deref 节点）
- 全局 BorrowTracker

## 💡 建议

**下次开会先验证 PL/0 兼容性**（跑 t01-t10），再开始 W3 借用检查器。
如果时间允许，可以让 Claude Code 写 W4 的 pcode 扩展（4 条新指令）。
