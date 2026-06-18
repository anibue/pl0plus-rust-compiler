# 资料下载索引 — `E:\rust_com\`

> **生成时间：** 2026-06-17
> **课程：** Rust 编译器实现

---

## 📊 下载结果总览

| 项目 | 路径 | 大小 | 状态 |
|------|------|------|------|
| 📘 **mrustc** ⭐ | `mrustc/` | ~11 MB | ✅ 完整下载 |
| 🦀 **rustc 官方** | `rust/` | 数百 MB | ✅ 完整下载 |
| 🏗️ **rustc_codegen_gcc** | `rustc_codegen_gcc/` | ~2.5 MB | ✅ 完整下载 |
| 🔧 **gccrs** | `gccrs/` | 较大 | ✅ 完整下载 |
| 📋 资料说明 | `README.md` | 4.6 KB | ✅ 已生成 |

---

## 🎯 课程必做内容对应表

### 必做内容 2 — if 条件语句的词法/语法分析

| 参考项目 | 关键目录 | 用途 |
|----------|----------|------|
| `mrustc/src/parse/` | `lex.cpp` `root.cpp` `token.cpp` | 词法分析 + 语法分析（C++） |
| `rust/compiler/rustc_lexer/` | 词法分析器 | 工业级实现参考 |
| `rust/compiler/rustc_parse/` | 语法分析器 | 工业级实现参考 |

**EBNF 文法**可以从以下地方提取：
- `mrustc/src/parse/root.cpp` 中找 if 相关产生式
- `rust/compiler/rustc_parse/src/grammar/` 中有完整 grammar

### 必做内容 3 — PL/0 改造

| 任务 | 推荐参考 |
|------|----------|
| 整数类型 i8/i16/i32 | `mrustc/src/ast/types.cpp` |
| Rust if 语句 | `mrustc/src/parse/root.cpp` |
| 栈内存管理 | `mrustc/src/trans/` 目录 |

---

## 🔍 关键文件路径速查

### mrustc（最推荐 ⭐）
```
E:\rust_com\mrustc\
├── README.md                              # 项目说明
├── Makefile                               # 构建脚本
└── src\
    ├── main.cpp                           # 编译器入口
    ├── parse\
    │   ├── lex.cpp / lex.hpp              # ⭐ 词法分析器
    │   ├── token.cpp / token.hpp          # Token 定义
    │   ├── root.cpp                       # ⭐ 语法分析器（顶层）
    │   ├── expr.cpp                       # 表达式解析
    │   ├── pattern.cpp                    # 模式解析
    │   └── eTokenType.enum.h              # Token 类型枚举
    ├── ast\                               # 抽象语法树
    ├── hir\                               # 高层中间表示
    ├── hir_typeck\                        # 类型检查
    ├── mir\                               # 中层中间表示
    └── trans\                             # 代码生成
```

### rustc（工业级参考）
```
E:\rust_com\rust\compiler\
├── rustc_lexer\                           # ⭐ 词法分析器
├── rustc_parse\                           # ⭐ 语法分析器
│   ├── src\
│   │   ├── lexer\                         # 词法分析实现
│   │   └── grammar\                       # 语法定义
│   └── grammar\                           # 完整 EBNF 文法
├── rustc_ast\                             # AST 定义
├── rustc_hir\                             # HIR
├── rustc_typeck\                          # 类型检查
└── rustc_codegen_*\                       # 代码生成
```

### rustc_codegen_gcc
```
E:\rust_com\rustc_codegen_gcc\
├── src\                                   # GCC 后端实现
└── README.md                              # 介绍
```

### gccrs
```
E:\rust_com\gccrs\
├── gcc\                                   # GCC 源代码集成
│   └── rust\                              # Rust 前端实现
├── configure
└── Makefile.in
```

---

## 💡 接下来怎么用这些资料

### 步骤 1：先看 mrustc README
```bash
cd e:/rust_com/mrustc
cat README.md
```

### 步骤 2：浏览 mrustc 词法分析器
```bash
# 推荐先看 token 定义
cat e:/rust_com/mrustc/src/parse/eTokenType.enum.h

# 再看词法分析
less e:/rust_com/mrustc/src/parse/lex.cpp
```

### 步骤 3：找 if 语句的 EBNF
```bash
# 在 mrustc 中找 if 相关的代码
grep -rn "if_expr" e:/rust_com/mrustc/src/parse/root.cpp
```

### 步骤 4：对比 rustc 的实现
```bash
# 看 rustc 的 if 表达式定义
ls e:/rust_com/rust/compiler/rustc_ast/src/ast.rs | xargs grep "If"
```

---

## ⚠️ 注意事项

1. **不要尝试编译这些项目** — 只是参考源码
2. **mrustc 优先** — 课程要求 C/C++，这个最合适
3. **rustc 看不动就别看** — 太工业级，初学没必要
4. **EBNF 文法** — 重点从 mrustc 的 `root.cpp` 提取

---

## 📅 下载时间线

- 11:00 - mrustc 完成 (~3 分钟)
- 10:53 - rustc_codegen_gcc 完成 (~7 秒)
- 11:00 - gccrs 完成 (~6 分钟)
- 11:02 - rust 完成 (~7 分钟)
