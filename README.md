# Rust 编译器课程设计 — PL/0+ 项目

> **课程：** 编译原理课程设计  
> **学生：** xwmsu  
> **完成日期：** 2026-06-18  
> **项目状态：** ✅ 已完成

---

## 📚 项目概述

本项目将 PL/0 编译器改造为支持 Rust 风格特性的 **PL/0+** 编译器，实现了：

- ✅ **类型系统**：i8/i16/i32 + 引用类型（&T, &mut T）
- ✅ **借用检查**：5 条核心规则（Rule 0-4）
- ✅ **栈内存管理**：RAII 自动释放
- ✅ **pcode 扩展**：4 条新指令（LEA/LODI/STOI/REL）
- ✅ **虚拟机实现**：完整的 PL0VM（12 条指令）

---

## 📁 目录结构

```
E:\rust_com\
├── README.md                          # 本文件（项目总览）
├── INDEX.md                           # 资料下载索引
├── .gitignore                         # Git 忽略规则
│
├── PL0Plus/                           # ⭐ 最终项目（PL/0+ 编译器）
│   ├── README.md                      # PL0Plus 项目说明
│   ├── application.pro                # Qt 工程文件
│   ├── compiler/                      # 编译器核心
│   ├── vm/                            # 虚拟机
│   ├── ui/                            # Qt GUI
│   ├── tests/                         # 测试用例
│   └── docs/                          # 文档与报告
│
├── docs/                              # 设计文档
│   └── superpowers/specs/             # Spec 与 PRD
│
├── mrustc/                            # 🌟 C++ 版 Rust 编译器（主参考）
├── rust/                              # 🦀 rustc 官方编译器源码
├── gccrs/                             # 🔧 GCC 的 Rust 前端
├── rustc_codegen_gcc/                 # 🔧 rustc 的 GCC 后端
│
├── PL0_compiler_with_LL1_and_QT/      # 📋 PL/0 基线源码
├── PL0/                               # 📋 南航版 PL/0
├── PL-0-Compiler/                     # 📋 LLVM 版 PL/0
└── pl0-QT版可运行程序/                # 📋 PL/0 可执行程序
```

---

## 🎯 课程设计内容

### 内容 2：Rust if 条件语句的词法/语法分析

**报告位置**：`PL0Plus/docs/report/content-2-rust-if-analysis.md`

**主要内容**：
- 词法分析原理（有限状态机、maximal-munch）
- 语法分析原理（LL(1) 预测分析）
- mrustc 词法/语法层参考实现
- PL/0+ if 语句 EBNF 文法

### 内容 3：PL/0 改造与 Rust 特性实现

**报告位置**：`PL0Plus/docs/report/final-report.md`

**主要内容**：
- 词法层扩展（+8 Token）
- 文法层扩展（+16 产生式）
- 借用检查器（5 条规则 + 作用域管理）
- pcode 扩展与虚拟机（4 条新指令）

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

---

## 🔍 关键文件路径

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

### 参考编译器

```
E:\rust_com\mrustc\src\parse\
├── lex.cpp                  # ⭐ 词法分析器（C++）
├── token.cpp                # Token 定义
├── root.cpp                 # ⭐ 语法分析器
└── expr.cpp                 # 表达式解析
```

---

## 📚 推荐学习资料

### 必读

1. **最终报告**：`PL0Plus/docs/report/final-report.md`
2. **Spec 文档**：`docs/superpowers/specs/2026-06-17-pl0-rust-compiler-spec.md`
3. **mrustc 源码**：`mrustc/src/parse/`（C++ 实现参考）

### 选读

- 《编译原理》（龙书）- 理论基础
- [Rust Reference](https://doc.rust-lang.org/reference/) - Rust 语言规范
- rustc 官方文档：https://rustc-dev-guide.rust-lang.org/

---

## ✅ 项目亮点

1. **完整的借用检查**：实现了 Rust 的 5 条核心借用规则
2. **作用域管理**：支持块级、过程级、分支级作用域
3. **RAII 自动释放**：借用离开作用域自动释放
4. **快照/回滚**：语句级别错误恢复
5. **精确错误报告**：14 个错误码，精确定位问题
6. **pcode 扩展**：新增 4 条指令支持引用类型
7. **虚拟机实现**：完整的 PL0VM 支持 12 条指令

---

## 📅 项目时间线

- **2026-06-17**：项目启动，下载参考资料，完成 W1 报告
- **2026-06-18 上午**：完成 W2 词法/语法扩展
- **2026-06-18 下午**：完成 W3 借用检查器 + W4 pcode/VM
- **2026-06-18 晚上**：完成最终报告，项目收尾

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

---

## 📞 联系方式

如有问题，请查看：
1. 最终报告：`PL0Plus/docs/report/final-report.md`
2. Spec 文档：`docs/superpowers/specs/2026-06-17-pl0-rust-compiler-spec.md`
3. Git 历史：`git log --oneline`

---

**项目完成日期**：2026-06-18  
**总代码行数**：约 5,000 行（新增）  
**报告总字数**：约 15,000 字
