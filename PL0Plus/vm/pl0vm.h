#pragma execution_character_set("utf-8")
#pragma once
#ifndef PL0VM_H
#define PL0VM_H

#include "instruction.h"
#include "stack.h"
#include <vector>
#include <iostream>

class PL0VM {
private:
    std::vector<Instruction> code;
    VMStack stack;
    int pc;     // 程序计数器
    int base;   // 当前栈帧基址

    // 计算基址（display 机制）
    int calculateBase(int L);

public:
    PL0VM() : pc(0), base(0) {}

    // 加载指令
    void loadCode(const std::vector<Instruction>& instructions);

    // 执行
    void execute();

    // 单步执行（调试用）
    void step();

    // 打印栈状态（调试用）
    void printStack();
};

#endif // !PL0VM_H
