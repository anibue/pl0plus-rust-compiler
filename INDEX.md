# 项目文件索引 — `E:\rust_com\`

> **最后更新：** 2026-06-18  
> **项目状态：** ✅ 已完成

---

## 📊 项目文件总览

| 类别 | 路径 | 说明 | 状态 |
|------|------|------|------|
| 📘 **最终项目** | `PL0Plus/` | PL/0+ 编译器（已完成）| ✅ |
| 📄 **设计文档** | `docs/superpowers/specs/` | Spec + PRD | ✅ |
| 📝 **报告文档** | `PL0Plus/docs/report/` | 最终报告 + 分章报告 | ✅ |
| 🌟 **参考编译器** | `mrustc/` | C++ 版 Rust 编译器 | ✅ |
| 🦀 **参考编译器** | `rust/` | rustc 官方源码 | ✅ |
| 🔧 **参考编译器** | `gccrs/` | GCC 的 Rust 前端 | ✅ |
| 🔧 **参考编译器** | `rustc_codegen_gcc/` | rustc 的 GCC 后端 | ✅ |
| 📋 **基线源码** | `PL0_compiler_with_LL1_and_QT/` | PL/0 基线 | ✅ |
| 📋 **基线源码** | `PL0/` | 南航版 PL/0 | ✅ |
| 📋 **基线源码** | `PL-0-Compiler/` | LLVM 版 PL/0 | ✅ |

---

## 🎯 核心文件路径

### 最终报告

```
E:\rust_com\PL0Plus\docs\report\
├── final-report.md                    # ⭐ 最终报告（15,000 字）
├── content-2-rust-if-analysis.md      # W1 报告：Rust if 词法/语法分析
├── content-3-lexer-grammar.md         # ⭐ W2 报告：词法/语法扩展
├── content-3-borrow-checker.md        # W3 报告：借用检查器
└── content-3-pcode-vm.md             # W4 报告：pcode 扩展与虚拟机
```

### 设计文档

```
E:\rust_com\docs\superpowers\specs\
├── 2026-06-17-pl0-rust-compiler-spec.md   # Spec（650 行，52 条决策）
└── 2026-06-17-pl0-rust-compiler-prd.md    # PRD（360 行）
```

### PL0Plus 编译器核心

```
E:\rust_com\PL0Plus\compiler\
├── lexer.h/cpp              # 词法分析器（+8 Token）
├── parser.h/cpp             # 语法分析器（LL(1)）
├── grammar.h/cpp            # 文法定义（+16 产生式）
├── grammarDefinition.h/cpp  # 文法符号定义
├── symbolTable.h/cpp        # 符号表管理
├── borrow_checker.h/cpp     # ⭐ 借用检查器（新增）
├── AstNode.h/cpp            # AST 节点
├── token.h/cpp              # Token 定义
└── ErrorMessage.h/cpp       # 错误处理
```

### PL0Plus 虚拟机

```
E:\rust_com\PL0Plus\vm\
├── instruction.h            # 指令定义（12 条）
├── stack.h                  # 栈实现
└── pl0vm.h/cpp              # ⭐ 虚拟机实现（新增）
```

### 测试用例

```
E:\rust_com\PL0Plus\tests\
├── t00_minimal.pl0          # 最小测试
├── t01_basic_compat.pl0     # 基础兼容性
├── t02_while_compat.pl0     # while 循环
├── t03_procedure_compat.pl0 # 过程调用
├── t04_if_compat.pl0        # if 语句
├── t05_expr_compat.pl0      # 表达式
├── t06_lexical.pl0          # 词法测试
├── t07_const_while.pl0      # const + while
├── t08_repeat_until.pl0     # repeat-until
├── t09_nested_if.pl0        # 嵌套 if
└── t10_comprehensive.pl0    # 综合测试
```

---

## 🔍 参考编译器关键文件

### mrustc（最推荐 ⭐）

```
E:\rust_com\mrustc\src\parse\
├── lex.cpp / lex.hpp              # ⭐ 词法分析器（C++）
├── token.cpp / token.hpp          # Token 定义
├── root.cpp                       # ⭐ 语法分析器（顶层）
├── expr.cpp                       # 表达式解析
├── pattern.cpp                    # 模式解析
└── eTokenType.enum.h              # Token 类型枚举
```

### rustc（工业级参考）

```
E:\rust_com\rust\compiler\
├── rustc_lexer\                   # ⭐ 词法分析器
├── rustc_parse\                   # ⭐ 语法分析器
│   ├── src\
│   │   ├── lexer\                 # 词法分析实现
│   │   └── grammar\               # 语法定义
│   └── grammar\                   # 完整 EBNF 文法
├── rustc_ast\                     # AST 定义
├── rustc_hir\                     # HIR
├── rustc_typeck\                  # 类型检查
└── rustc_codegen_*\               # 代码生成
```

---

## 💡 快速访问指南

### 查看最终报告

```bash
cd E:\rust_com\PL0Plus\docs\report
cat final-report.md
```

### 查看设计文档

```bash
# 查看 Spec（技术设计）
cat E:\rust_com\docs\superpowers\specs\2026-06-17-pl0-rust-compiler-spec.md

# 查看 PRD（产品需求）
cat E:\rust_com\docs\superpowers\specs\2026-06-17-pl0-rust-compiler-prd.md
```

### 运行测试

```bash
cd E:\rust_com\PL0Plus
./test_compiler.exe tests/t01_basic_compat.pl0
```

### 查看 mrustc 词法分析器

```bash
# 查看 Token 定义
cat E:\rust_com\mrustc\src\parse\eTokenType.enum.h

# 查看词法分析实现
less E:\rust_com\mrustc\src\parse\lex.cpp
```

---

## 📅 项目时间线

| 日期 | 事件 |
|------|------|
| 2026-06-17 | 项目启动，下载参考资料 |
| 2026-06-17 | 完成 W1 报告（Rust if 词法/语法分析）|
| 2026-06-18 上午 | 完成 W2 词法/语法扩展 |
| 2026-06-18 下午 | 完成 W3 借用检查器 + W4 pcode/VM |
| 2026-06-18 晚上 | 完成最终报告，项目收尾 |

---

## ⚠️ 注意事项

1. **PL0Plus 是最终项目** — 所有工作都在此目录
2. **参考编译器仅供学习** — 不要尝试编译它们
3. **mrustc 优先** — 课程要求 C/C++，这个最合适
4. **最终报告在 PL0Plus/docs/report/** — 这是提交的内容

---

## 📞 相关文档

- [README.md](README.md) — 项目总览
- [PL0Plus/README.md](PL0Plus/README.md) — PL0Plus 项目说明
- [PL0Plus/docs/report/final-report.md](PL0Plus/docs/report/final-report.md) — 最终报告
