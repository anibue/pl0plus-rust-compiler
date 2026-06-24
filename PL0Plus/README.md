# PL/0+ Rust 编译器 — 最终项目

---

## 📚 项目概述

本项目将 PL/0 编译器改造为支持 Rust 风格特性的 **PL/0+** 编译器。

### 实现的功能

- ✅ **类型系统**：i8/i16/i32 + 引用类型（&T, &mut T）
- ✅ **借用检查**：5 条核心规则（Rule 0-4）
- ✅ **栈内存管理**：RAII 自动释放
- ✅ **pcode 扩展**：4 条新指令（LEA/LODI/STOI/REL）
- ✅ **虚拟机实现**：完整的 PL0VM（12 条指令）

---

## 📂 项目结构

```
PL0Plus/
├── README.md                      # 本文件
├── application.pro                # Qt 工程文件
│
├── compiler/                      # 编译器核心
│   ├── lexer.h/cpp              # 词法分析器（+8 Token）
│   ├── parser.h/cpp             # 语法分析器（LL(1)）
│   ├── grammar.h/cpp            # 文法定义（+16 产生式）
│   ├── grammarDefinition.h/cpp   # 文法符号定义
│   ├── symbolTable.h/cpp        # 符号表管理
│   ├── borrow_checker.h/cpp     # 借用检查器（新增）
│   ├── AstNode.h/cpp            # AST 节点
│   ├── token.h/cpp              # Token 定义
│   ├── ErrorMessage.h/cpp       # 错误处理
│   └── position.h/cpp           # 位置信息
│
├── vm/                           # 虚拟机（新增）
│   ├── instruction.h            # 指令定义（12 条）
│   ├── stack.h                  # 栈实现
│   └── pl0vm.h/cpp             # 虚拟机实现
│
├── ui/                           # Qt GUI
│   ├── mainwindow.h/cpp         # 主窗口
│   ├── codeeditor.h/cpp        # 代码编辑器
│   └── main.cpp                 # 入口
│
├── tests/                        # 测试用例 + 测试程序源码
│   ├── t00_minimal.pl0         # 最小测试
│   ├── t01_basic_compat.pl0    # 基础兼容性
│   ├── t02_while_compat.pl0    # while 循环
│   ├── t03_procedure_compat.pl0 # 过程调用
│   ├── t04_if_compat.pl0       # if 语句
│   ├── t05_expr_compat.pl0     # 表达式
│   ├── t06_lexical.pl0         # 词法测试
│   ├── t07_const_while.pl0     # const + while
│   ├── t08_repeat_until.pl0   # repeat-until
│   ├── t09_nested_if.pl0       # 嵌套 if
│   ├── t10_comprehensive.pl0   # 综合测试
│   ├── test_compiler.cpp        # 编译器测试程序源码
│   ├── test_lexer_only.cpp      # 词法测试程序源码
│   └── test_parser_construct.cpp # Parser 构造测试源码
│
├── bin/                          # 编译好的可执行文件
│   ├── test_compiler.exe        # 编译器测试
│   ├── test_compiler_debug.exe  # 调试版
│   ├── test_lexer_only.exe     # 词法测试
│   └── test_parser_construct.exe # Parser 构造测试
│
└── docs/                         # 文档
    ├── README.md                # 文档目录说明
    ├── claude-code-brief-w2.md # Claude Code 任务简报
    └── report/                  # 报告
        ├── README.md            # 报告目录说明
        ├── final-report.md      # 最终报告（15,000 字）
        ├── content-2-rust-if-analysis.md  # W1 报告
        ├── content-3-lexer-grammar.md     # W2 报告
        ├── content-3-borrow-checker.md    # W3 报告
        └── content-3-pcode-vm.md          # W4 报告
```

---

## 🚀 快速开始

### 查看最终报告

```bash
cd docs/report
cat final-report.md
```

### 运行测试

```bash
# 词法分析测试
./bin/test_lexer_only.exe tests/t01_basic_compat.pl0

# 编译器测试
./bin/test_compiler.exe tests/t01_basic_compat.pl0
```

### 编译项目（需要 Qt）

```bash
qmake application.pro
make
```

---

## 📖 文档

| 文档 | 路径 | 说明 |
|------|------|------|
| **最终报告** | `docs/report/final-report.md` | 完整的课程设计报告（15,000 字）|
| **W1 报告** | `docs/report/content-2-rust-if-analysis.md` | Rust if 词法/语法分析 |
| **W2 报告** | `docs/report/content-3-lexer-grammar.md` | 词法/语法扩展 |
| **W3 报告** | `docs/report/content-3-borrow-checker.md` | 借用检查器 |
| **W4 报告** | `docs/report/content-3-pcode-vm.md` | pcode/VM |
| **Spec** | `../../docs/superpowers/specs/2026-06-17-pl0-rust-compiler-spec.md` | 技术设计 |
| **PRD** | `../../docs/superpowers/specs/2026-06-17-pl0-rust-compiler-prd.md` | 产品需求 |

---

## 📊 完成情况

| 阶段 | 任务 | 状态 | 产出 |
|------|------|------|------|
| W1 | 内容 2 报告 | ✅ 完成 | content-2-rust-if-analysis.md |
| W2 | 词法/语法扩展 | ✅ 完成 | +8 Token, +16 产生式 |
| W3 | 借用检查器 | ✅ 完成 | BorrowTracker + 5 条规则 |
| W4 | pcode + VM | ✅ 完成 | 4 条新指令 + PL0VM |
| 最终报告 | 整合所有内容 | ✅ 完成 | final-report.md |

---

## 🎯 核心功能

### 1. 词法层扩展

新增 8 个 Token：
- `let`, `mut` — Rust 风格声明
- `i8`, `i16`, `i32` — 整数类型
- `&`, `&mut` — 借用操作
- `:` — 类型标注

### 2. 文法层扩展

新增 16 个产生式：
- `letDecl` — let 声明
- `type`, `refType`, `baseType` — 类型定义
- `borrowExpr` — 借用表达式
- `derefExpr` — 解引用表达式

### 3. 借用检查器

实现 5 条核心规则：
- **Rule 0**：借用者必须已初始化
- **Rule 1**：不可变借用可多个
- **Rule 2**：可变借用只能一个
- **Rule 3**：可变与不可变互斥
- **Rule 4**：防悬垂

### 4. pcode 扩展

新增 4 条指令：
- `LEA L A` — 取地址（用于 `&x`）
- `LODI` — 间接取值（用于 `*r`）
- `STOI` — 间接存值（用于 `*m := val`）
- `REL 0 N` — 释放 N 个 cell

---

## 📖 关联项目

- [PL0_compiler_with_LL1_and_QT](../../PL0_compiler_with_LL1_and_QT/) — 基线源码
- [mrustc](../../mrustc/) — Rust 编译器参考
- [PL0](../../PL0/) — 南航版 PL/0

---

## ✅ 项目完成

**总代码行数**：约 5,000 行（新增）
**报告总字数**：约 15,000 字
**Git 提交数**：15 次
