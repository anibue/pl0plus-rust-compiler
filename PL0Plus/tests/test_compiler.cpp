// 编译器核心测试程序
#include "pch.h"
#include "lexer.h"
#include "parser.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <input.pl0>" << endl;
        return 1;
    }

    // 读取输入文件
    ifstream file(argv[1]);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << argv[1] << endl;
        return 1;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string code = buffer.str();
    file.close();

    cout << "=== PL/0+ Compiler ===" << endl;
    cout << "Input: " << argv[1] << endl;
    cout << "Code length: " << code.length() << " bytes" << endl;
    cout << endl;

    // 词法分析
    cout << "=== Lexical Analysis ===" << endl;
    Lexer lexer(code);
    lexer.printTokens();
    cout << endl;

    // 语法分析
    cout << "=== Syntax Analysis ===" << endl;
    Parser parser(code);
    parser.printParsing();
    cout << endl;

    // 错误检查
    if (!parser.errorEmpty()) {
        cout << "=== Errors ===" << endl;
        parser.printErrorList();
        return 1;
    }

    // pcode 生成
    cout << "=== P-Code ===" << endl;
    parser.printPCode();
    cout << endl;

    cout << "=== Compilation Successful ===" << endl;
    return 0;
}
