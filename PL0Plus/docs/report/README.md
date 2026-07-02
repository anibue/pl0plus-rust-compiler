# 报告目录

---

## 📁 目录结构

```
PL0Plus/docs/report/
├── README.md                          # 本文件
├── final-report.md                    # ⭐ 最终报告（15,000 字）
├── content-2-rust-if-analysis.md     # W1 报告
├── content-3-lexer-grammar.md        # W2 报告
├── content-3-borrow-checker.md       # W3 报告
├── content-3-pcode-vm.md             # W4 报告
└── w2-progress.md                    # W2 进度简报（历史文件，可删除）
```

---

## 📄 报告说明

### 最终报告

**文件**：`final-report.md`  
**字数**：约 15,000 字  
**内容**：
- 第1章：引言
- 第2章：PL/0 语言与编译器概述
- 第3章：内容 2 — Rust if 词法/语法分析
- 第4章：内容 3 — PL/0 改造与 Rust 特性实现
  - 4.1 词法层扩展
  - 4.2 文法层扩展
  - 4.3 借用检查器
  - 4.4 pcode 扩展与虚拟机
- 第5章：测试与演示
- 第6章：总结
- 第7章：参考文献
- 第8章：附录

**用途**：这是提交给老师的完整课程设计报告。

### Bug 修复记录

2026-07-02 修复 4 个关键 bug：
- `compiler/AstNode.cpp`：`getFatherSize()` 空指针防护
- `compiler/grammar.cpp`：预测表/First/Follow 边界扩展
- `compiler/lexer.cpp`：添加 PROGRAM 关键字映射
- `compiler/parser.cpp`：预测表访问防御检查

详见 `final-report.md` 第 9 章。

---

### 分章报告

#### W1 报告：Rust if 词法/语法分析

**文件**：`content-2-rust-if-analysis.md`  
**字数**：约 4,000 字  
**内容**：
- 词法分析原理
- 语法分析原理
- mrustc 参考实现
- PL/0+ if 语句 EBNF 文法

#### W3 报告：借用检查器设计与实现

**文件**：`content-3-borrow-checker.md`  
**字数**：约 4,500 字  
**内容**：
- 借用检查器架构设计
- 核心数据结构
- 5 条借用规则详细实现
- 作用域管理
- 快照/回滚机制
- 错误码表
- 测试用例

#### W4 报告：pcode 扩展与虚拟机

**文件**：`content-3-pcode-vm.md`  
**字数**：约 3,800 字  
**内容**：
- pcode 指令集扩展（4 条新指令）
- PL0VM 虚拟机实现
- 代码生成逻辑
- 编译验证结果

---

## 🔍 快速访问

```bash
# 查看最终报告
cat final-report.md

# 查看 W1 报告
cat content-2-rust-if-analysis.md

# 查看 W3 报告
cat content-3-borrow-checker.md

# 查看 W4 报告
cat content-3-pcode-vm.md
```

---

## 📊 报告统计

| 报告 | 字数 | 行数 | 完成日期 |
|------|------|------|----------|
| 最终报告 | ~15,000 | ~1,100 | 2026-06-18 |
| W1 报告 | ~4,000 | ~400 | 2026-06-17 |
| **W2 报告** | ~4,000 | ~400 | 2026-06-18 |
| W3 报告 | ~4,500 | ~520 | 2026-06-18 |
| W4 报告 | ~3,800 | ~400 | 2026-06-18 |

---

## 📚 相关文档

- [设计文档](../../../docs/README.md)
- [PL0Plus README](../../README.md)
- [项目总览](../../../README.md)
