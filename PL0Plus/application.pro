QT += widgets
requires(qtConfig(filedialog))

# 头文件
HEADERS += compiler/lexer.h \
           compiler/token.h \
           compiler/position.h \
           compiler/grammar.h \
           compiler/grammarDefinition.h \
           compiler/AstNode.h \
           compiler/ErrorMessage.h \
           compiler/symbolTable.h \
           compiler/parser.h \
           compiler/borrow_checker.h \
           compiler/pch.h \
           vm/instruction.h \
           vm/stack.h \
           vm/pl0vm.h \
           ui/mainwindow.h \
           ui/codeeditor.h

# 源文件
SOURCES += compiler/lexer.cpp \
           compiler/token.cpp \
           compiler/position.cpp \
           compiler/grammar.cpp \
           compiler/grammarDefinition.cpp \
           compiler/AstNode.cpp \
           compiler/ErrorMessage.cpp \
           compiler/symbolTable.cpp \
           compiler/parser.cpp \
           compiler/borrow_checker.cpp \
           compiler/pch.cpp \
           vm/pl0vm.cpp \
           ui/main.cpp \
           ui/mainwindow.cpp \
           ui/codeeditor.cpp

# 包含路径
INCLUDEPATH += compiler vm ui

# 目标文件
TARGET = PL0Plus
TEMPLATE = app
