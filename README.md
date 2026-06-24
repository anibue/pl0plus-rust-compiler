# Rust 编译器课程设计 — 项目总览

---

## 📚 项目概述

本项目将 PL/0 编译器改造为支持 Rust 风格特性的 **PL/0+** 编译器。

### 课程设计必做内容

| 内容 | 说明 | 状态 |
|------|------|------|
| **内容 2** | Rust if 条件语句的词法/语法分析 | ✅ 完成 |
| **内容 3** | PL/0 改造与 Rust 特性实现 | ✅ 完成 |

### 实现的 Rust 特性

- ✅ **类型系统**：i8/i16/i32 + 引用类型（&T, &mut T）
- ✅ **借用检查**：5 条核心规则（Rule 0-4）
- ✅ **栈内存管理**：RAII 自动释放
- ✅ **pcode 扩展**：4 条新指令（LEA/LODI/STOI/REL）
- ✅ **虚拟机实现**：完整的 PL0VM（12 条指令）

---

## 📂 目录结构

```
E:\rust_com\
├── README.md                          # 本文件（项目总览）
├── INDEX.md                           # 文件索引
├── .gitignore                         # Git 忽略规则
│
├── PL0Plus/                           # ⭐ 最终项目（PL/0+ 编译器）
│   ├── compiler/                     # 编译器核心
│   ├── vm/                           # 虚拟机
│   ├── ui/                           # Qt GUI
│   ├── tests/                         # 测试用例 + 测试程序源码
│   ├── bin/                          # 编译好的可执行文件
│   └── docs/                         # 文档与报告
│
├── docs/                              # 设计文档
│   ├── README.md
│   └── superpowers/specs/             # Spec 与 PRD
│
├── reference/                         # 📚 参考资料（只读）
│   ├── README.md                      # 参考资料目录说明
│   ├── pl0-impls/                    # PL/0 编译器实现（4 个版本）
│   │   ├── PL0_compiler_with_LL1_and_QT/  # ⭐ 基线源码
│   │   ├── PL0/                      # 南航版
│   │   ├── PL-0-Compiler/             # LLVM 版
│   │   └── pl0-QT版可运行程序/        # QT 可执行版
│   └── rust-compilers/               # Rust 编译器实现（4 个）
│       ├── mrustc/                   # 🌟 C++ 版（主参考）
│       ├── rust/                      # 🦀 官方 rustc
│       ├── gccrs/                    # 🔧 GCC 的 Rust 前端
│       └── rustc_codegen_gcc/        # 🔧 rustc 的 GCC 后端
│
└── pl0_qt.rar                         # ⚠️ 旧下载文件（建议删除）
```

---

## 🎯 课程设计内容

### 内容 2：Rust if 条件语句的词法/语法分析

**报告**：`PL0Plus/docs/report/content-2-rust-if-analysis.md`

### 内容 3：PL/0 改造与 Rust 特性实现

**分章报告**：
- **W2 报告**：`PL0Plus/docs/report/content-3-lexer-grammar.md` — 词法/语法扩展
- **W3 报告**：`PL0Plus/docs/report/content-3-borrow-checker.md` — 借用检查器
- **W4 报告**：`PL0Plus/docs/report/content-3-pcode-vm.md` — pcode/VM
- **最终报告**：`PL0Plus/docs/report/final-report.md` — 整合所有内容

---

## 📊 项目完成情况

| 阶段 | 任务 | 状态 | 产出 |
|------|------|------|------|
| W1 | 内容 2 报告 | ✅ 完成 | content-2-rust-if-analysis.md |
| W2 | 词法/语法扩展 | ✅ 完成 | +8 Token, +16 产生式 |
| W3 | 借用检查器 | ✅ 完成 | BorrowTracker + 5 条规则 |
| W4 | pcode + VM | ✅ 完成 | 4 条新指令 + PL0VM |
| 最终报告 | 整合所有内容 | ✅ 完成 | final-report.md |

---

## 📖 快速开始

### 查看最终报告

```bash
cd PL0Plus/docs/report
cat final-report.md
```

### 运行测试

```bash
cd PL0Plus
./bin/test_compiler.exe tests/t01_basic_compat.pl0
```

### 编译项目（需要 Qt）

```bash
cd PL0Plus
qmake application.pro
make
```

---

## 🔍 参考资料说明

> ⚠️ `reference/` 下所有目录是课程参考资料，**只读不要修改**。

### PL/0 实现

| 目录 | 说明 | 用途 |
|------|------|------|
| `reference/pl0-impls/PL0_compiler_with_LL1_and_QT/` | PL/0 + LL(1) + Qt | ⭐ 基线源码，本项目在此基础上改造 |
| `reference/pl0-impls/PL0/` | 南航版 PL/0 | C++ 实现参考 |
| `reference/pl0-impls/PL-0-Compiler/` | LLVM 版 PL/0 | LLVM IR 代码生成参考 |
| `reference/pl0-impls/pl0-QT版可运行程序/` | QT 可执行版 | PL/0 可执行程序 |

### Rust 编译器

| 目录 | 说明 | 用途 |
|------|------|------|
| `reference/rust-compilers/mrustc/` | 🌟 C++ 版 Rust 编译器 | **主参考**，用 C++ 写成 |
| `reference/rust-compilers/rust/` | 🦀 rustc 官方编译器 | 工业级实现参考 |
| `reference/rust-compilers/gccrs/` | 🔧 GCC 的 Rust 前端 | GCC 生态集成参考 |
| `reference/rust-compilers/rustc_codegen_gcc/` | 🔧 rustc 的 GCC 后端 | 后端代码生成参考 |

---

## 📚 推荐学习资料

### 必读

1. **最终报告**：`PL0Plus/docs/report/final-report.md`
2. **Spec 文档**：`docs/superpowers/specs/2026-06-17-pl0-rust-compiler-spec.md`
3. **mrustc 源码**：`reference/rust-compilers/mrustc/src/parse/`

### 选读

- 《编译原理》（龙书）- 理论基础
- [Rust Reference](https://doc.rust-lang.org/reference/)
- rustc 官方文档：https://rustc-dev-guide.rust-lang.org/

---

## ✅ 项目亮点

1. **完整的借用检查**：实现了 Rust 的 5 条核心借用规则
2. **作用域管理**：支持块级、过程级、分支级作用域
3. **RAII 自动释放**：借用离开作用域自动释放
4. **快照/回滚**：语句级别错误恢复
5. **精确错误报告**：14 个错误码
6. **pcode 扩展**：新增 4 条指令支持引用类型
7. **虚拟机实现**：完整的 PL0VM 支持 12 条指令

---

## 💡 关键提示

> 🎯 **课程要求原文：**
> - 全部工作用 C/C++ 在原 PL/0 编译器上完成
> - 可以参考主流的 Rust 开源编译器
> - Rust 的 if 语句比较复杂，可自行裁剪

> ✅ **本项目完美满足要求：**
> - 使用 C++ 实现
> - 参考 mrustc（C++ 版 Rust 编译器）
> - 实现了 Rust 风格的所有系统
