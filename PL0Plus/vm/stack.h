#pragma execution_character_set("utf-8")
#pragma once
#ifndef STACK_H
#define STACK_H

#include <vector>
#include <stdexcept>

class VMStack {
private:
    std::vector<int> data;
    int t;  // 栈顶指针

public:
    VMStack() : t(-1) {
        data.resize(1000, 0);  // 预分配 1000 个单元
    }

    void push(int value) {
        if (t >= (int)data.size() - 1) {
            throw std::runtime_error("Stack overflow");
        }
        data[++t] = value;
    }

    int pop() {
        if (t < 0) {
            throw std::runtime_error("Stack underflow");
        }
        return data[t--];
    }

    int top() const {
        if (t < 0) {
            throw std::runtime_error("Stack is empty");
        }
        return data[t];
    }

    int& operator[](int index) {
        if (index < 0 || index >= (int)data.size()) {
            throw std::runtime_error("Stack index out of bounds");
        }
        return data[index];
    }

    int getT() const { return t; }
    void setT(int newT) { t = newT; }

    void clear() { t = -1; }
};

#endif // !STACK_H
