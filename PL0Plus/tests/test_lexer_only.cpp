// 简单的词法分析测试
#include "pch.h"
#include "lexer.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <input.pl0>" << endl;
        return 1;
    }

    ifstream file(argv[1]);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << argv[1] << endl;
        return 1;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string code = buffer.str();
    file.close();

    cout << "=== Lexical Analysis Only ===" << endl;
    cout << "Input: " << argv[1] << endl;
    cout << "Code length: " << code.length() << " bytes" << endl;
    cout << endl;

    Lexer lexer(code);
    lexer.printTokens();

    cout << endl << "=== Lexical Analysis Complete ===" << endl;
    return 0;
}
