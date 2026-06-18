#pragma execution_character_set("utf-8")
#pragma once
#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>

// pcode 指令操作码
enum class OpCode {
    // 原有 PL/0 指令
    LIT,    // LIT 0 A: 加载常量 A 到栈顶
    LOD,    // LOD L A: 加载变量 mem[base+A] 到栈顶
    STO,    // STO L A: 存储栈顶到 mem[base+A]
    CAL,    // CAL L A: 调用过程 A
    INT,    // INT 0 A: 分配 A 个存储单元（t += A）
    JMP,    // JMP 0 A: 无条件跳转到 A
    JPC,    // JPC 0 A: 条件跳转到 A（栈顶为 0 时跳转）
    OPR,    // OPR 0 A: 运算操作（A=0 返回，A=2 加，A=3 减，A=4 乘，A=5 除，A=6 奇数判断，A=8 等，A=9 不等，A=10 <，A=11 >=，A=12 >，A=13 <=，A=14 输出，A=15 换行，A=16 输入）
    
    // ⭐ PL/0+ 新增指令
    LEA,    // LEA L A: 取地址，压入 base+A（用于 &x）
    LODI,   // LODI: 间接取值，pop addr，push mem[addr]（用于 *r）
    STOI,   // STOI: 间接存值，pop val，pop addr，mem[addr] = val（用于 *m := val）
    REL     // REL 0 N: 释放 N 个 cell，t -= N（用于离开作用域）
};

struct Instruction {
    OpCode op;
    int L;  // 层级差（display 索引）
    int A;  // 地址或操作数

    Instruction(OpCode o, int l, int a) : op(o), L(l), A(a) {}
};

#endif // !INSTRUCTION_H
