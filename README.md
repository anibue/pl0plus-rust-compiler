# Rust 编译器资料整理 — 课程设计

> **课程：** 人工智能与计科班 Rust 编译器实现
> **整理日期：** 2026-06-17
> **路径：** `E:\rust_com`

---

## 📚 主流开源 Rust 编译器

Rust 编译器生态里有几个重要项目，按学习优先级排序：

### 1. 🌟 rustc（官方编译器）⭐⭐⭐⭐⭐
| 项目 | 信息 |
|------|------|
| **GitHub** | [rust-lang/rust](https://github.com/rust-lang/rust) |
| **语言** | Rust（自身） |
| **规模** | 极大（~30万行核心代码） |
| **架构** | 多 crate 架构，包含编译器、库、标准库 |
| **适合** | 了解工业级编译器实现 |
| **难度** | 极高（不适合直接学习） |

**核心子目录：**
- `compiler/rustc_lexer/` — 词法分析
- `compiler/rustc_parse/` — 语法分析
- `compiler/rustc_ast/` — 抽象语法树
- `compiler/rustc_hir/` — 中间表示
- `compiler/rustc_typeck/` — 类型检查
- `compiler/rustc_codegen_*` — 代码生成

---

### 2. 🌟 mrustc（**课程首选**）⭐⭐⭐⭐⭐
| 项目 | 信息 |
|------|------|
| **GitHub** | [thepowersgang/mrustc](https://github.com/thepowersgang/mrustc) |
| **语言** | **C++**（符合课程要求） |
| **规模** | 较小（~5万行） |
| **作者** | thepowersgang |
| **适合** | **课程设计的最佳参考！** |
| **难度** | 中等 |

**为什么最推荐：**
- ✅ 用 C++ 写成（课程要求 C/C++）
- ✅ 完整的 Rust 子集编译器
- ✅ 代码结构清晰，注释较好
- ✅ 比 rustc 简单几十倍
- ✅ 能编译出真实可运行的 Rust 程序
- ✅ 自带测试用例

**目录结构：**
```
mrustc/
├── src/
│   ├── ast/          # 抽象语法树
│   ├── parse/        # 词法+语法分析
│   ├── hir/          # 高层中间表示
│   ├── hir_typeck/   # 类型检查
│   ├── mir/          # 中层中间表示
│   ├── codegen/      # 代码生成
│   └── main.cpp      # 入口
├── Makefile
└── README.md
```

---

### 3. gccrs（GCC 的 Rust 前端）⭐⭐⭐
| 项目 | 信息 |
|------|------|
| **GitHub** | [Rust-GCC/gccrs](https://github.com/Rust-GCC/gccrs) |
| **语言** | C++（GCC 项目） |
| **规模** | 中等（~10万行） |
| **状态** | 仍在开发中 |
| **适合** | 了解 GCC 生态集成 |

---

### 4. rustc_codegen_gcc（rustc 的 GCC 后端）⭐⭐
| 项目 | 信息 |
|------|------|
| **GitHub** | [rust-lang/rustc_codegen_gcc](https://github.com/rust-lang/rustc_codegen_gcc) |
| **作用** | 让 rustc 用 GCC 作为后端 |
| **适合** | 后端代码生成参考 |

---

## 🎯 课程设计建议

### 推荐参考资料顺序

```
1. mrustc ⭐⭐⭐⭐⭐   ← 主参考（用 C++ 写）
       ↓
2. rustc ⭐⭐⭐⭐     ← 工业级参考（了解前沿实现）
       ↓
3. 编译原理教材      ← 理论支持
       ↓
4. gccrs            ← 可选扩展了解
```

### 必做内容 2 重点

> **以 if 条件语句为例，介绍 Rust 编译器中词法分析和语法分析的实现原理**

**重点看：**
- mrustc 的 `src/parse/` 目录
- rustc 的 `compiler/rustc_lexer/` 和 `compiler/rustc_parse/lexer/`
- EBNF 文法可以从这两个项目的 grammar 描述提取

### 必做内容 3 重点

> **修改 PL/0 编译器支持 Rust 整数类型 / if 语句 / 栈管理**

**建议步骤：**
1. 先读懂原 PL/0 代码
2. 参考 mrustc 的 `src/parse/lex.cpp` 和 `src/parse/parse.cpp`
3. 提取 Rust if 的 EBNF 文法
4. 在 PL/0 词法分析器增加新 token
5. 在 PL/0 语法分析器增加新产生式

---

## 📖 推荐学习资料

### 必读
- 《编译原理》（龙书）- 理论基础
- [Rust Reference](https://doc.rust-lang.org/reference/) - Rust 语言规范
- mrustc README - 项目介绍

### 选读
- 《Engineering a Compiler》- 编译器工程实践
- rustc 官方文档：https://rustc-dev-guide.rust-lang.org/
- 知乎"如何实现一门编程语言"系列

---

## 📁 文件夹组织

```
E:\rust_com\
├── README.md                    # 本文档
├── rustc/                       # 官方编译器源码
├── mrustc/                      # C++ 版 Rust 编译器（**主参考**）
├── gccrs/                       # GCC 的 Rust 前端
└── rustc_codegen_gcc/           # rustc 的 GCC 后端
```

---

## ✅ 关键提示

> 💡 **课程要求原文：**
> - 全部工作用 C/C++ 在原 PL/0 编译器上完成
> - 可以参考主流的 Rust 开源编译器
> - Rust 的 if 语句比较复杂，可自行裁剪

> 🎯 **所以重点研究 mrustc 即可！** 它正好是 C++ 写的，与课程要求完美契合。
