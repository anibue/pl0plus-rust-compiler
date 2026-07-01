"""生成实验报告所需的 10 张图。

数据来源：实际运行 11 个测试用例采集的编译耗时。
"""
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.patches import FancyArrowPatch, FancyBboxPatch, Rectangle
import numpy as np

plt.rcParams['font.sans-serif'] = ['SimHei', 'Microsoft YaHei', 'DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False
plt.rcParams['font.size'] = 10

# ===== 真实数据 =====
test_files = ['t00', 't01', 't02', 't03', 't04', 't05', 't06', 't07', 't08', 't09', 't10']
test_names = ['t00_minimal', 't01_basic', 't02_while', 't03_proc', 't04_if',
              't05_expr', 't06_lexical', 't07_const', 't08_repeat', 't09_nested', 't10_all']
compile_time_ms = [44.67, 33.56, 41.57, 31.13, 37.30, 41.06, 32.15, 37.07, 40.47, 37.96, 39.37]
source_size_B = [27, 47, 152, 175, 102, 113, 120, 160, 103, 170, 361]

OUT = 'e:/rust_com/PL0Plus/docs/report/figures/'

# ========================= 图 2.1：PL/0 编译器总体架构图 =========================
fig, ax = plt.subplots(figsize=(8.5, 3.2))
ax.set_xlim(0, 100)
ax.set_ylim(0, 30)
ax.axis('off')

stages = [
    (5, 15, 18, 8, '源代码\n.pl0', '#FFE4B5'),
    (28, 15, 18, 8, '词法分析\nLexer', '#B0E0E6'),
    (51, 15, 18, 8, '语法分析\nParser\n(LL(1))', '#98FB98'),
    (74, 15, 18, 8, '代码生成\npcode', '#FFB6C1'),
]
for x, y, w, h, txt, color in stages:
    box = FancyBboxPatch((x, y), w, h, boxstyle='round,pad=0.2',
                         edgecolor='black', facecolor=color, linewidth=1.2)
    ax.add_patch(box)
    ax.text(x + w/2, y + h/2, txt, ha='center', va='center', fontsize=10, weight='bold')

for x in [23, 46, 69]:
    arrow = FancyArrowPatch((x, 19), (x + 5, 19), arrowstyle='->',
                            mutation_scale=20, color='black', linewidth=1.5)
    ax.add_patch(arrow)

ax.text(14, 8, 'Token 流', ha='center', fontsize=9, style='italic')
ax.text(37, 8, 'AST + 符号表', ha='center', fontsize=9, style='italic')
ax.text(60, 8, 'pcode 指令序列', ha='center', fontsize=9, style='italic')

# VM
vm_box = FancyBboxPatch((74, 4), 18, 6, boxstyle='round,pad=0.2',
                        edgecolor='red', facecolor='#FFE4E1', linewidth=1.5, linestyle='--')
ax.add_patch(vm_box)
ax.text(83, 7, 'PL0VM\n虚拟机执行', ha='center', va='center', fontsize=9, weight='bold', color='red')
arrow2 = FancyArrowPatch((83, 15), (83, 10), arrowstyle='->',
                         mutation_scale=20, color='red', linewidth=1.5)
ax.add_patch(arrow2)

ax.set_title('图 2.1  PL/0 编译器总体架构与处理流程', fontsize=12, weight='bold', pad=10)
plt.tight_layout()
plt.savefig(OUT + 'fig2_1_architecture.png', dpi=150, bbox_inches='tight')
plt.close()

# ========================= 图 3.1：AMP 状态转换图 =========================
fig, ax = plt.subplots(figsize=(7.5, 3.5))
ax.set_xlim(0, 100)
ax.set_ylim(0, 50)
ax.axis('off')

# 节点
states = [
    (10, 30, 'START', '#90EE90'),
    (35, 30, 'AMP', '#FFE4B5'),
    (65, 30, 'AMu', '#FFE4B5'),
    (85, 30, 'AMut', '#FFE4B5'),
]
for x, y, txt, color in states:
    box = FancyBboxPatch((x-7, y-3), 14, 6, boxstyle='round,pad=0.2',
                         edgecolor='black', facecolor=color, linewidth=1.2)
    ax.add_patch(box)
    ax.text(x, y, txt, ha='center', va='center', fontsize=10, weight='bold')

# 终态
final1 = FancyBboxPatch((28, 5), 14, 6, boxstyle='round,pad=0.2',
                       edgecolor='black', facecolor='#FFB6C1', linewidth=1.2)
ax.add_patch(final1)
ax.text(35, 8, 'AMPSYM\n(`&`)', ha='center', va='center', fontsize=9, weight='bold')

final2 = FancyBboxPatch((78, 5), 14, 6, boxstyle='round,pad=0.2',
                       edgecolor='black', facecolor='#FFB6C1', linewidth=1.5)
ax.add_patch(final2)
ax.text(85, 8, 'AMPMUTSYM\n(`&mut`)', ha='center', va='center', fontsize=9, weight='bold')

# 转移
ax.annotate('', xy=(28, 30), xytext=(17, 30),
            arrowprops=dict(arrowstyle='->', lw=1.5))
ax.text(22.5, 32, '&', fontsize=11, weight='bold', ha='center')

ax.annotate('', xy=(58, 30), xytext=(42, 30),
            arrowprops=dict(arrowstyle='->', lw=1.5))
ax.text(50, 32, 'm', fontsize=11, weight='bold', ha='center')

ax.annotate('', xy=(78, 30), xytext=(72, 30),
            arrowprops=dict(arrowstyle='->', lw=1.5))
ax.text(75, 32, 'u', fontsize=11, weight='bold', ha='center')

# AMP -> AMPSYM (无 m 字符)
ax.annotate('', xy=(35, 11), xytext=(35, 27),
            arrowprops=dict(arrowstyle='->', lw=1.2, linestyle='--', color='gray'))
ax.text(38, 19, '非 m', fontsize=9, color='gray', style='italic')

# AMut -> AMPMUTSYM
ax.annotate('', xy=(85, 11), xytext=(85, 27),
            arrowprops=dict(arrowstyle='->', lw=1.2, color='black'))
ax.text(80, 19, 't', fontsize=11, weight='bold', ha='center')

ax.set_title('图 3.1  AMP 状态转换图（识别 `&` 与 `&mut`）', fontsize=12, weight='bold', pad=10)
plt.tight_layout()
plt.savefig(OUT + 'fig3_1_amp_state.png', dpi=150, bbox_inches='tight')
plt.close()

# ========================= 图 3.2：词法分析 token 流示例 =========================
fig, ax = plt.subplots(figsize=(9, 4))
ax.set_xlim(0, 100)
ax.set_ylim(0, 100)
ax.axis('off')

# 源代码
src_box = FancyBboxPatch((5, 75), 90, 18, boxstyle='round,pad=0.2',
                        edgecolor='black', facecolor='#FFF8DC', linewidth=1)
ax.add_patch(src_box)
ax.text(50, 84, '源代码：let mut x: i32 = &mut y;', ha='center', va='center',
        fontsize=11, family='monospace', weight='bold')

# Token 流
tokens = [
    ('LETSYM', 5, '#90EE90'),
    ('MUTSYM', 18, '#90EE90'),
    ('ID(x)', 31, '#FFE4B5'),
    ('COLONSYM', 44, '#E6E6FA'),
    ('I32SYM', 57, '#90EE90'),
    ('BECOMESSYM', 70, '#E6E6FA'),
    ('AMPMUTSYM', 83, '#FFB6C1'),
    ('ID(y)', 92, '#FFE4B5'),
]
ax.text(2, 60, 'Token 流:', fontsize=10, weight='bold')
for tok, x, color in tokens:
    box = FancyBboxPatch((x-4, 50), 10, 8, boxstyle='round,pad=0.1',
                        edgecolor='black', facecolor=color, linewidth=0.8)
    ax.add_patch(box)
    ax.text(x+1, 54, tok, ha='center', va='center', fontsize=7.5, weight='bold')

# 关键 token 高亮说明
ax.annotate('AMPMUTSYM 是单 token\n（不是 `&` + `mut`）',
            xy=(83, 54), xytext=(60, 30),
            arrowprops=dict(arrowstyle='->', lw=1.2, color='red'),
            fontsize=9, color='red', ha='center',
            bbox=dict(boxstyle='round', facecolor='#FFE4E1', edgecolor='red'))

ax.text(50, 18, '图 3.2  词法分析：源代码到 Token 流的转换示例',
        ha='center', fontsize=11, weight='bold')
plt.tight_layout()
plt.savefig(OUT + 'fig3_2_tokens.png', dpi=150, bbox_inches='tight')
plt.close()

# ========================= 图 4.1：LL(1) 预测分析栈操作 =========================
fig, ax = plt.subplots(figsize=(8.5, 4.5))
ax.set_xlim(0, 100)
ax.set_ylim(0, 100)
ax.axis('off')

# 分析栈
stack_box = FancyBboxPatch((5, 60), 25, 30, boxstyle='round,pad=0.2',
                          edgecolor='black', facecolor='#F0F8FF', linewidth=1.2)
ax.add_patch(stack_box)
ax.text(17.5, 86, '分析栈 (Stack)', ha='center', fontsize=10, weight='bold')
stack_content = ['$', 'E', "E'", 'T', "T'", 'F']
for i, s in enumerate(stack_content):
    y = 78 - i*3
    ax.text(17.5, y, f'| {s} |', ha='center', va='center', fontsize=9, family='monospace')

# 输入
input_box = FancyBboxPatch((40, 60), 50, 30, boxstyle='round,pad=0.2',
                          edgecolor='black', facecolor='#FFFAF0', linewidth=1.2)
ax.add_patch(input_box)
ax.text(65, 86, '剩余输入', ha='center', fontsize=10, weight='bold')
input_content = ['id', '+', 'id', '*', 'id', '$']
for i, s in enumerate(input_content):
    x = 45 + i*7
    ax.text(x, 70, s, ha='center', va='center', fontsize=9, family='monospace',
           bbox=dict(boxstyle='round', facecolor='#FFE4E1' if i==0 else 'white', edgecolor='black'))

# 预测分析表
ax.text(50, 50, '预测分析表 M[A, a]', ha='center', fontsize=11, weight='bold')
table_data = [
    ['', 'id', '+', '*', '(', ')', '$'],
    ['E', 'E→TE\'', '', '', 'E→TE\'', '', ''],
    ["E'", '', "E'→+TE'", '', '', "E'→ε", "E'→ε"],
    ['T', 'T→FT\'', '', '', 'T→FT\'', '', ''],
    ["T'", '', "T'→ε", "T'→*FT'", '', "T'→ε", "T'→ε"],
    ['F', 'F→id', '', '', 'F→(E)', '', ''],
    ['id', 'id', '', '', '(', ')', ''],
]
table = ax.table(cellText=table_data, loc='center', cellLoc='center',
                colWidths=[0.1]*7, bbox=[0.1, 0.05, 0.8, 0.35])
table.auto_set_font_size(False)
table.set_fontsize(8)
table.scale(1, 1.4)
for i in range(7):
    if (0, i) in table._cells:
        table[0, i].set_facecolor('#FFE4B5')
for i in range(1, 7):
    if (i, 0) in table._cells:
        table[i, 0].set_facecolor('#FFE4B5')
        table[i, 0].set_text_props(weight='bold')

ax.set_title('图 4.1  LL(1) 预测分析：栈、输入缓冲与预测分析表',
            fontsize=12, weight='bold', pad=10)
plt.tight_layout()
plt.savefig(OUT + 'fig4_1_ll1.png', dpi=150, bbox_inches='tight')
plt.close()

# ========================= 图 5.1：借用检查器 Rule 4 状态机 =========================
fig, ax = plt.subplots(figsize=(8, 4.5))
ax.set_xlim(0, 100)
ax.set_ylim(0, 100)
ax.axis('off')

# 节点
nodes = [
    (15, 70, '创建借用\nborrow_mut(r, x)', '#B0E0E6'),
    (50, 70, '检查双方\n作用域层级', '#FFE4B5'),
    (85, 70, 'borrower_scope\n≥ owner_scope ?', '#FFE4B5'),
    (35, 30, '通过\n记录 BorrowRecord', '#90EE90'),
    (75, 30, '拒绝\nE0027 悬垂借用', '#FFB6C1'),
]
for x, y, txt, color in nodes:
    box = FancyBboxPatch((x-12, y-5), 24, 10, boxstyle='round,pad=0.2',
                        edgecolor='black', facecolor=color, linewidth=1.2)
    ax.add_patch(box)
    ax.text(x, y, txt, ha='center', va='center', fontsize=9, weight='bold')

# 连线
ax.annotate('', xy=(38, 70), xytext=(27, 70), arrowprops=dict(arrowstyle='->', lw=1.5))
ax.annotate('', xy=(73, 70), xytext=(62, 70), arrowprops=dict(arrowstyle='->', lw=1.5))

# 决策分叉
ax.annotate('', xy=(35, 35), xytext=(80, 65),
            arrowprops=dict(arrowstyle='->', lw=1.2, color='green'))
ax.text(50, 52, '是', fontsize=10, color='green', weight='bold')

ax.annotate('', xy=(75, 35), xytext=(85, 65),
            arrowprops=dict(arrowstyle='->', lw=1.2, color='red'))
ax.text(88, 50, '否', fontsize=10, color='red', weight='bold')

ax.set_title('图 5.1  BorrowTracker.Rule 4 防悬垂检查流程',
            fontsize=12, weight='bold', pad=10)
plt.tight_layout()
plt.savefig(OUT + 'fig5_1_rule4.png', dpi=150, bbox_inches='tight')
plt.close()

# ========================= 图 5.2：pcode 数据流（&x → *m := v）=========================
fig, ax = plt.subplots(figsize=(9, 4))
ax.set_xlim(0, 100)
ax.set_ylim(0, 100)
ax.axis('off')

# 三个阶段
phases = [
    (8, 50, 22, 30, '借用阶段\n&x', '#FFE4B5', 'LEA L A\n压入 base+A'),
    (40, 50, 22, 30, '解引用读取\n*r', '#B0E0E6', 'LOD 加载 r\nLODI 间接取值'),
    (72, 50, 22, 30, '解引用写入\n*m := v', '#FFB6C1', 'LOD m\nLIT v\nSTOI 间接存值'),
]
for x, y, w, h, title, color, instr in phases:
    box = FancyBboxPatch((x, y), w, h, boxstyle='round,pad=0.2',
                        edgecolor='black', facecolor=color, linewidth=1.2)
    ax.add_patch(box)
    ax.text(x + w/2, y + h - 4, title, ha='center', fontsize=10, weight='bold')
    ax.text(x + w/2, y + h/2, instr, ha='center', va='center', fontsize=9, family='monospace')

# 栈状态图示
stack_data = [
    ('TOS', 0),
    ('10', 0),
    ('addr(y)', 0),  # 这个在 LEA 后
    ('x', 0),
]
ax.text(50, 35, 'VM 栈状态变化（自底向上）', ha='center', fontsize=10, weight='bold')

# 阶段 1 后的栈
y_top = 28
ax.text(19, y_top, '| addr(x) |', ha='center', fontsize=8, family='monospace',
       bbox=dict(boxstyle='round', facecolor='white', edgecolor='black'))
ax.text(19, y_top-3, '| x (5) |', ha='center', fontsize=8, family='monospace',
       bbox=dict(boxstyle='round', facecolor='white', edgecolor='black'))
ax.text(19, y_top-6, '| $ |', ha='center', fontsize=8, family='monospace',
       bbox=dict(boxstyle='round', facecolor='white', edgecolor='black'))

# 阶段 2 后的栈
ax.text(51, y_top, '| 5 |', ha='center', fontsize=8, family='monospace',
       bbox=dict(boxstyle='round', facecolor='white', edgecolor='black'))
ax.text(51, y_top-3, '| x (5) |', ha='center', fontsize=8, family='monospace',
       bbox=dict(boxstyle='round', facecolor='white', edgecolor='black'))
ax.text(51, y_top-6, '| $ |', ha='center', fontsize=8, family='monospace',
       bbox=dict(boxstyle='round', facecolor='white', edgecolor='black'))

# 阶段 3 后的栈
ax.text(83, y_top, '| x (10) |', ha='center', fontsize=8, family='monospace',
       bbox=dict(boxstyle='round', facecolor='white', edgecolor='black'))
ax.text(83, y_top-3, '| x (5→10) |', ha='center', fontsize=8, family='monospace',
       bbox=dict(boxstyle='round', facecolor='#FFE4E1', edgecolor='red'))
ax.text(83, y_top-6, '| $ |', ha='center', fontsize=8, family='monospace',
       bbox=dict(boxstyle='round', facecolor='white', edgecolor='black'))

# 箭头
ax.annotate('', xy=(40, 65), xytext=(30, 65), arrowprops=dict(arrowstyle='->', lw=1.5))
ax.annotate('', xy=(72, 65), xytext=(62, 65), arrowprops=dict(arrowstyle='->', lw=1.5))

ax.set_title('图 5.2  pcode 数据流：从借用 `&x` 到解引用写入 `*m := v`',
            fontsize=12, weight='bold', pad=10)
plt.tight_layout()
plt.savefig(OUT + 'fig5_2_pcode_flow.png', dpi=150, bbox_inches='tight')
plt.close()

# ========================= 图 5.3：VM 栈帧内存布局 =========================
fig, ax = plt.subplots(figsize=(7, 4))
ax.set_xlim(0, 100)
ax.set_ylim(0, 100)
ax.axis('off')

# 主栈
stack_cells = [
    ('TOS (栈顶)', '#FFB6C1'),
    ('局部变量 m', '#B0E0E6'),
    ('局部变量 r', '#B0E0E6'),
    ('局部变量 x', '#B0E0E6'),
    ('返回地址 RA', '#FFE4B5'),
    ('动态链 DL', '#FFE4B5'),
    ('静态链 SL', '#FFE4B5'),
    ('base ptr →', '#90EE90'),
    ('调用者帧...', '#E0E0E0'),
]
y_start = 88
for i, (label, color) in enumerate(stack_cells):
    y = y_start - i*8
    box = Rectangle((20, y-3.5), 50, 7, edgecolor='black', facecolor=color, linewidth=1)
    ax.add_patch(box)
    ax.text(45, y, label, ha='center', va='center', fontsize=9, family='monospace')

# 标注
ax.annotate('REL 0 3\n释放 3 个 cell', xy=(70, 70), xytext=(78, 70),
            arrowprops=dict(arrowstyle='->', lw=1.2, color='red'),
            fontsize=9, color='red', ha='left', weight='bold')
ax.text(45, 8, '作用域结束 → 释放所有 cell', ha='center', fontsize=9,
        style='italic', color='red')

ax.set_title('图 5.3  PL0VM 栈帧内存布局与 REL 指令作用域释放',
            fontsize=12, weight='bold', pad=10)
plt.tight_layout()
plt.savefig(OUT + 'fig5_3_stack.png', dpi=150, bbox_inches='tight')
plt.close()

# ========================= 图 5.4：借用关系 DAG =========================
fig, ax = plt.subplots(figsize=(7, 3.5))
ax.set_xlim(0, 100)
ax.set_ylim(0, 100)
ax.axis('off')

# 节点（变量）
nodes = [
    (15, 70, 'x', '#90EE90', 'owner'),
    (40, 70, 'r', '#B0E0E6', 'borrower\n&x'),
    (65, 70, 'm', '#FFB6C1', 'borrower\n&mut x'),
    (40, 30, 'y', '#FFE4B5', 'owner'),
    (75, 30, 's', '#E0E0E6', 'borrower\n&y'),
]
for x, y, name, color, role in nodes:
    circle = mpatches.Circle((x, y), 6, edgecolor='black', facecolor=color, linewidth=1.2)
    ax.add_patch(circle)
    ax.text(x, y, name, ha='center', va='center', fontsize=12, weight='bold')
    ax.text(x, y-10, role, ha='center', fontsize=8, style='italic')

# 边
edges = [
    (15, 70, 40, 70, '&', 'green'),
    (15, 70, 65, 70, '&mut', 'red'),
    (40, 30, 75, 30, '&', 'green'),
]
for x1, y1, x2, y2, label, color in edges:
    arrow = FancyArrowPatch((x1+6, y1), (x2-6, y2), arrowstyle='->',
                            mutation_scale=15, color=color, linewidth=1.5)
    ax.add_patch(arrow)
    mid_x, mid_y = (x1+x2)/2, (y1+y2)/2 + 4
    ax.text(mid_x, mid_y, label, ha='center', fontsize=9, weight='bold', color=color,
           bbox=dict(boxstyle='round,pad=0.2', facecolor='white', edgecolor=color))

# 标注
ax.text(50, 95, '借用关系图：x 被 r（不可变）和 m（可变）同时借用', ha='center',
        fontsize=10, weight='bold')
ax.text(50, 15, '注：Rust 规则禁止同时存在 `&` 和 `&mut` 借用同一变量', ha='center',
        fontsize=8, color='red', style='italic')

plt.tight_layout()
plt.savefig(OUT + 'fig5_4_borrow_dag.png', dpi=150, bbox_inches='tight')
plt.close()

# ========================= 图 6.1：测试用例编译耗时柱状图 =========================
fig, ax = plt.subplots(figsize=(9, 4.5))
colors = ['#B0E0E6' if i > 0 else '#FFB6C1' for i in range(len(test_names))]
bars = ax.bar(test_names, compile_time_ms, color=colors, edgecolor='black', linewidth=0.8)

for bar, t in zip(bars, compile_time_ms):
    ax.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.3,
            f'{t:.1f}', ha='center', va='bottom', fontsize=8)

ax.set_xlabel('测试用例', fontsize=11)
ax.set_ylabel('编译耗时 (ms)', fontsize=11)
ax.set_title('图 6.1  11 个 PL/0 兼容性测试用例编译耗时实测结果', fontsize=12, weight='bold')
ax.set_ylim(0, max(compile_time_ms) * 1.15)
ax.grid(axis='y', alpha=0.3, linestyle='--')
ax.tick_params(axis='x', rotation=15)

# 均值线
mean_t = np.mean(compile_time_ms[1:])  # 排除 t00 (空程序)
ax.axhline(y=mean_t, color='red', linestyle=':', alpha=0.7, label=f'平均: {mean_t:.1f}ms')
ax.legend()

plt.tight_layout()
plt.savefig(OUT + 'fig6_1_perf.png', dpi=150, bbox_inches='tight')
plt.close()

# ========================= 图 6.2：源码大小 vs 编译耗时散点图 =========================
fig, ax = plt.subplots(figsize=(8, 4.5))
ax.scatter(source_size_B, compile_time_ms, s=80, c='#4682B4',
          edgecolor='black', alpha=0.7, zorder=3)

# 标注
for i, name in enumerate(test_names):
    ax.annotate(name, (source_size_B[i], compile_time_ms[i]),
                xytext=(5, 5), textcoords='offset points', fontsize=8, alpha=0.8)

# 趋势线
z = np.polyfit(source_size_B, compile_time_ms, 1)
p = np.poly1d(z)
x_line = np.linspace(min(source_size_B), max(source_size_B), 100)
ax.plot(x_line, p(x_line), '--', color='red', alpha=0.6,
       label=f'线性拟合: y={z[0]:.4f}x+{z[1]:.1f}')

ax.set_xlabel('源代码大小 (字节)', fontsize=11)
ax.set_ylabel('编译耗时 (ms)', fontsize=11)
ax.set_title('图 6.2  编译耗时与源码规模的相关性分析', fontsize=12, weight='bold')
ax.grid(alpha=0.3, linestyle='--')
ax.legend()

plt.tight_layout()
plt.savefig(OUT + 'fig6_2_scatter.png', dpi=150, bbox_inches='tight')
plt.close()

# ========================= 图 6.3：词法分析 token 数量分布 =========================
fig, ax = plt.subplots(figsize=(8, 4.5))
# 估算每个测试用例的 token 数（基于源码大小估算：每 5-6 字节一个 token）
token_counts = [max(3, s // 5) for s in source_size_B]
ax.bar(test_names, token_counts, color='#98FB98', edgecolor='black', linewidth=0.8)
for i, (name, tc, t) in enumerate(zip(test_names, token_counts, compile_time_ms)):
    ax.text(i, tc + 0.3, f'{tc}', ha='center', fontsize=8)

ax.set_xlabel('测试用例', fontsize=11)
ax.set_ylabel('Token 数量（估算）', fontsize=11)
ax.set_title('图 6.3  各测试用例产生的 Token 数量分布', fontsize=12, weight='bold')
ax.grid(axis='y', alpha=0.3, linestyle='--')
ax.tick_params(axis='x', rotation=15)

plt.tight_layout()
plt.savefig(OUT + 'fig6_3_tokens_dist.png', dpi=150, bbox_inches='tight')
plt.close()

print('10 张图全部生成完成。')
print(f'输出目录: {OUT}')
