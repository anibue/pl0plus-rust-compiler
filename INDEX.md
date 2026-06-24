# 项目文件索引 — `E:\rust_com\`

---

## 📊 项目文件总览

| 类别 | 路径 | 说明 | 状态 |
|------|------|------|------|
| ⭐ **最终项目** | `PL0Plus/` | PL/0+ 编译器（已完成）| ✅ |
| 📄 **设计文档** | `docs/superpowers/specs/` | Spec + PRD | ✅ |
| 📝 **报告文档** | `PL0Plus/docs/report/` | 最终报告 + 分章报告 | ✅ |
| 📚 **PL/0 实现** | `reference/pl0-impls/` | 4 个 PL/0 版本 | ⭐ 只读 |
| 📚 **Rust 编译器** | `reference/rust-compilers/` | 4 个 Rust 编译器 | ⭐ 只读 |

---

## 📂 完整目录树

```
E:\rust_com\
├── README.md                          # 项目总览
├── INDEX.md                           # 本文件（索引）
├── .gitignore                         # Git 忽略规则
├── pl0_qt.rar                         # 旧下载文件（建议删除）
│
├── PL0Plus/                           # ⭐ 最终项目
│   ├── compiler/                     # 编译器核心
│   ├── vm/                           # 虚拟机
│   ├── ui/                           # Qt GUI
│   ├── tests/                         # 测试用例
│   ├── bin/                          # 编译好的 exe
│   ├── docs/                         # 文档
│   ├── application.pro
│   └── README.md
│
├── docs/                              # 设计文档
│   ├── README.md
│   └── superpowers/specs/
│       ├── 2026-06-17-pl0-rust-compiler-spec.md
│       └── 2026-06-17-pl0-rust-compiler-prd.md
│
└── reference/                         # 📚 参考资料（只读）
    ├── README.md                      # 参考资料说明
    ├── pl0-impls/                    # PL/0 编译器实现
    │   ├── PL0_compiler_with_LL1_and_QT/  # ⭐ 基线
    │   ├── PL0/                      # 南航版
    │   ├── PL-0-Compiler/             # LLVM 版
    │   └── pl0-QT版可运行程序/        # QT 可执行版
    └── rust-compilers/               # Rust 编译器实现
        ├── mrustc/                   # 🌟 C++ 版（主参考）
        ├── rust/                      # 🦀 官方 rustc
        ├── gccrs/                    # GCC 的 Rust 前端
        └── rustc_codegen_gcc/        # rustc 的 GCC 后端
```

---

## 🎯 核心文件路径

### 最终报告

```
E:\rust_com\PL0Plus\docs\report\
├── README.md
├── final-report.md                    # ⭐ 最终报告（15,000 字）
├── content-2-rust-if-analysis.md     # W1 报告
├── content-3-lexer-grammar.md        # W2 报告
├── content-3-borrow-checker.md       # W3 报告
└── content-3-pcode-vm.md            # W4 报告
```

### 设计文档

```
E:\rust_com\docs\superpowers\specs\
├── 2026-06-17-pl0-rust-compiler-spec.md   # Spec（650 行）
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
├── borrow_checker.h/cpp     # ⭐ 借用检查器
├── AstNode.h/cpp           # AST 节点
├── token.h/cpp             # Token 定义
└── ErrorMessage.h/cpp       # 错误处理
```

### PL0Plus 虚拟机

```
E:\rust_com\PL0Plus\vm\
├── instruction.h            # 指令定义（12 条）
├── stack.h                 # 栈实现
└── pl0vm.h/cpp             # ⭐ 虚拟机实现
```

### 测试用例

```
E:\rust_com\PL0Plus\tests\
├── t00_minimal.pl0 ~ t10_comprehensive.pl0  # 11 个测试
├── test_compiler.cpp        # 编译器测试源码
├── test_lexer_only.cpp      # 词法测试源码
└── test_parser_construct.cpp # Parser 测试源码
```

---

## 🔍 参考资料关键文件

### mrustc（最推荐 ⭐）

```
E:\rust_com\reference\rust-compilers\mrustc\src\parse\
├── lex.cpp / lex.hpp              # ⭐ 词法分析器（C++）
├── token.cpp / token.hpp          # Token 定义
├── root.cpp                       # ⭐ 语法分析器（顶层）
├── expr.cpp                       # 表达式解析
└── eTokenType.enum.h              # Token 类型枚举
```

### rustc（工业级参考）

```
E:\rust_com\reference\rust-compilers\rust\compiler\
├── rustc_lexer\                   # ⭐ 词法分析器
├── rustc_parse\                   # ⭐ 语法分析器
├── rustc_ast\                     # AST 定义
├── rustc_hir\                     # HIR
├── rustc_typeck\                  # 类型检查
└── rustc_codegen_*\               # 代码生成
```

### PL/0 基线

```
E:\rust_com\reference\pl0-impls\PL0_compiler_with_LL1_and_QT\
├── compiler/                     # PL/0 编译器源码
│   ├── lexer.h/cpp
│   ├── parser.h/cpp
│   ├── grammar.cpp
│   └── symbolTable.cpp
├── ui/                           # Qt GUI
├── application.pro
└── PL0.exe                       # 可执行文件
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
# Spec
cat E:\rust_com\docs\superpowers\specs\2026-06-17-pl0-rust-compiler-spec.md

# PRD
cat E:\rust_com\docs\superpowers\specs\2026-06-17-pl0-rust-compiler-prd.md
```

### 运行测试

```bash
cd E:\rust_com\PL0Plus
./bin/test_compiler.exe tests/t01_basic_compat.pl0
```

---

## 📅 项目时间线

| 日期 | 事件 |
|------|------|
| 2026-06-17 | 项目启动，下载参考资料 |
| 2026-06-17 | 完成 W1 报告 |
| 2026-06-18 上午 | 完成 W2 词法/语法扩展 |
| 2026-06-18 下午 | 完成 W3 借用检查器 + W4 pcode/VM |
| 2026-06-18 晚上 | 完成最终报告 |
| 2026-06-24 | 整理 PL0Plus 内部结构 |
| 2026-06-25 | 归类参考资料到 reference/ |

---

## ⚠️ 注意事项

1. **PL0Plus 是最终项目** — 所有工作都在此目录
2. **reference/ 仅供学习** — 不要尝试编译这些项目
3. **mrustc 优先** — 课程要求 C/C++，这个最合适
4. **pl0_qt.rar** — 建议删除（已在 .gitignore 中）

---

## 📞 相关文档

- [README.md](README.md) — 项目总览
- [PL0Plus/README.md](PL0Plus/README.md) — PL0Plus 项目说明
- [reference/README.md](reference/README.md) — 参考资料说明
- [PL0Plus/docs/report/final-report.md](PL0Plus/docs/report/final-report.md) — 最终报告
