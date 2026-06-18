# PL/0+ Rust 编译器课程设计

## 📚 文档

| 文档 | 描述 |
|------|------|
| [Spec](../docs/superpowers/specs/2026-06-17-pl0-rust-compiler-spec.md) | 完整技术设计（650 行）|
| [PRD](../docs/superpowers/specs/2026-06-17-pl0-rust-compiler-prd.md) | 产品需求文档 |

## 🎯 项目目标

将 PL/0 编译器**增量式**改造为支持 Rust 风格特性：
- 类型系统（i8/i16/i32 + 借用）
- 借用检查（5 条规则）
- 栈内存管理（含悬垂借用检测）
- 28 个测试用例

## 📂 项目结构

```
PL0Plus/                    # 本目录（最终项目）
├── compiler/                # 编译器核心（从 PL0_compiler_with_LL1_and_QT 扩展）
│   ├── lexer.h/cpp
│   ├── parser.h/cpp
│   ├── grammar.h/cpp
│   ├── symbolTable.h/cpp
│   ├── borrow_checker.h/cpp # ⭐ 新增
│   ├── ast.h/cpp
│   ├── token.h/cpp
│   └── error.h/cpp
├── vm/                      # 虚拟机（已增强 LEA/LODI/STOI/REL）
│   ├── pl0vm.h/cpp
│   ├── instruction.h
│   └── stack.h/cpp
├── ui/                      # Qt GUI（保留原框架）
│   ├── mainwindow.h/cpp
│   ├── codeeditor.h/cpp
│   └── main.cpp
├── tests/                   # 28 个 .pl0 测试
├── docs/                    # 报告与设计文档
│   └── report/
├── README.md
└── application.pro          # Qt 工程
```

## 🚀 编译

```bash
qmake application.pro
make
```

## 📅 时间表

| 周 | 任务 |
|----|------|
| W1 | 内容 2 报告 + 读 mrustc |
| W2 | 词法/语法扩展 → T1-T10 |
| W3 | 借用检查器 → T11-T22 |
| W4 | pcode + 栈管理 → T23-T28 + 报告 |

## 📖 关联项目

- [PL0_compiler_with_LL1_and_QT](../PL0_compiler_with_LL1_and_QT/) — 基础源码
- [mrustc](../mrustc/) — Rust 编译器参考
- [PL0](../PL0/) — 南航版 PL/0
