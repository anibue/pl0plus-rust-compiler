#pragma execution_character_set("utf-8")
#include "pch.h"
#include "pl0vm.h"

int PL0VM::calculateBase(int L) {
    int b = base;
    while (L > 0) {
        b = stack[b];  // 栈帧底部存储的是上一层的 base
        L--;
    }
    return b;
}

void PL0VM::loadCode(const std::vector<Instruction>& instructions) {
    code = instructions;
    pc = 0;
    base = 0;
    stack.clear();
    
    // 初始化栈帧
    stack.push(0);  // 静态链
    stack.push(0);  // 动态链
    stack.push(0);  // 返回地址
}

void PL0VM::execute() {
    while (pc < (int)code.size()) {
        step();
    }
}

void PL0VM::step() {
    if (pc >= (int)code.size()) {
        return;
    }

    Instruction& inst = code[pc];
    pc++;

    int b = calculateBase(inst.L);

    switch (inst.op) {
        case OpCode::LIT:
            // LIT 0 A: 加载常量 A 到栈顶
            stack.push(inst.A);
            break;

        case OpCode::LOD:
            // LOD L A: 加载变量 mem[base+A] 到栈顶
            stack.push(stack[b + inst.A]);
            break;

        case OpCode::STO:
            // STO L A: 存储栈顶到 mem[base+A]
            stack[b + inst.A] = stack.pop();
            break;

        case OpCode::CAL:
            // CAL L A: 调用过程 A
            stack[stack.getT() + 1] = calculateBase(inst.L);  // 静态链
            stack[stack.getT() + 2] = base;  // 动态链
            stack[stack.getT() + 3] = pc;    // 返回地址
            base = stack.getT() + 1;
            pc = inst.A;
            break;

        case OpCode::INT:
            // INT 0 A: 分配 A 个存储单元（t += A）
            stack.setT(stack.getT() + inst.A);
            break;

        case OpCode::JMP:
            // JMP 0 A: 无条件跳转到 A
            pc = inst.A;
            break;

        case OpCode::JPC:
            // JPC 0 A: 条件跳转到 A（栈顶为 0 时跳转）
            if (stack.pop() == 0) {
                pc = inst.A;
            }
            break;

        case OpCode::OPR:
            // OPR 0 A: 运算操作
            switch (inst.A) {
                case 0:  // 返回
                    pc = stack[base + 2];
                    stack.setT(base - 1);
                    base = stack[base + 1];
                    break;
                case 1:  // 取负
                    stack[stack.getT()] = -stack[stack.getT()];
                    break;
                case 2:  // 加法
                    stack[stack.getT() - 1] = stack[stack.getT() - 1] + stack.pop();
                    break;
                case 3:  // 减法
                    stack[stack.getT() - 1] = stack[stack.getT() - 1] - stack.pop();
                    break;
                case 4:  // 乘法
                    stack[stack.getT() - 1] = stack[stack.getT() - 1] * stack.pop();
                    break;
                case 5:  // 除法
                    stack[stack.getT() - 1] = stack[stack.getT() - 1] / stack.pop();
                    break;
                case 6:  // 奇数判断
                    stack[stack.getT()] = stack[stack.getT()] % 2;
                    break;
                case 8:  // 等于
                    stack[stack.getT() - 1] = (stack[stack.getT() - 1] == stack.pop()) ? 1 : 0;
                    break;
                case 9:  // 不等于
                    stack[stack.getT() - 1] = (stack[stack.getT() - 1] != stack.pop()) ? 1 : 0;
                    break;
                case 10: // 小于
                    stack[stack.getT() - 1] = (stack[stack.getT() - 1] < stack.pop()) ? 1 : 0;
                    break;
                case 11: // 大于等于
                    stack[stack.getT() - 1] = (stack[stack.getT() - 1] >= stack.pop()) ? 1 : 0;
                    break;
                case 12: // 大于
                    stack[stack.getT() - 1] = (stack[stack.getT() - 1] > stack.pop()) ? 1 : 0;
                    break;
                case 13: // 小于等于
                    stack[stack.getT() - 1] = (stack[stack.getT() - 1] <= stack.pop()) ? 1 : 0;
                    break;
                case 14: // 输出
                    std::cout << stack.pop() << " ";
                    break;
                case 15: // 换行
                    std::cout << std::endl;
                    break;
                case 16: // 输入
                    int val;
                    std::cin >> val;
                    stack.push(val);
                    break;
            }
            break;

        // ⭐ PL/0+ 新增指令
        case OpCode::LEA:
            // LEA L A: 取地址，压入 base+A（用于 &x）
            stack.push(b + inst.A);
            break;

        case OpCode::LODI:
            // LODI: 间接取值，pop addr，push mem[addr]（用于 *r）
            {
                int addr = stack.pop();
                if (addr < 0 || addr > stack.getT()) {
                    throw std::runtime_error("E0028: Invalid memory access in LODI");
                }
                stack.push(stack[addr]);
            }
            break;

        case OpCode::STOI:
            // STOI: 间接存值，pop val，pop addr，mem[addr] = val（用于 *m := val）
            {
                int val = stack.pop();
                int addr = stack.pop();
                if (addr < 0 || addr > stack.getT()) {
                    throw std::runtime_error("E0028: Invalid memory access in STOI");
                }
                stack[addr] = val;
            }
            break;

        case OpCode::REL:
            // REL 0 N: 释放 N 个 cell，t -= N（用于离开作用域）
            stack.setT(stack.getT() - inst.A);
            break;
    }
}

void PL0VM::printStack() {
    std::cout << "Stack (t=" << stack.getT() << "): ";
    for (int i = 0; i <= stack.getT(); i++) {
        std::cout << stack[i] << " ";
    }
    std::cout << std::endl;
}
