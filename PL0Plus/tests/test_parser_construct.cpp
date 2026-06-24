// 测试 Parser 构造
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

    ifstream file(argv[1]);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << argv[1] << endl;
        return 1;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string code = buffer.str();
    file.close();

    cout << "=== Parser Construction Test ===" << endl;
    cout << "Input: " << argv[1] << endl;
    cout << "Code length: " << code.length() << " bytes" << endl;
    cout << endl;

    cout << "Creating Parser..." << endl;
    Parser* parser = new Parser(code);
    cout << "Parser created successfully!" << endl;

    delete parser;
    cout << "Parser deleted successfully!" << endl;

    return 0;
}
