#pragma execution_character_set("utf-8")
#pragma once
#ifndef BORROW_CHECKER_H
#define BORROW_CHECKER_H

#include "symbolTable.h"
#include <vector>
#include <stack>
#include <string>

using std::string;
using std::vector;
using std::stack;

struct BorrowRecord {
	string borrower;
	string owner;
	bool   is_mut;
	int    borrow_line;
	int    owner_scope;     // 所有者作用域层级
	int    borrower_scope;  // 借用者作用域层级
	bool   alive;
};

class BorrowTracker {
private:
	RustSymbolTable* sym_table;
	vector<BorrowRecord> records;
	int current_scope_level;

	struct Snapshot {
		vector<BorrowRecord> records_copy;
		int scope_level_copy;
	};

	stack<Snapshot> snapshot_stack;

public:
	BorrowTracker(RustSymbolTable* st) : sym_table(st), current_scope_level(0) {}

	// 语句级别 API（封装快照逻辑）
	void begin_statement();   // 语句开始前调用
	void end_statement();     // 语句成功完成后调用
	void abort_statement();   // 语句失败时调用

	// 作用域管理
	void enter_scope();
	void exit_scope();

	// 借用操作
	bool borrow_imm(string borrower, string owner, int line);
	bool borrow_mut(string borrower, string owner, int line);
	void release_borrow(string borrower);

	// 规则检查
	bool check_rule0(string owner);  // 借用者必须已初始化
	bool check_rule1(string owner);  // 不可变借用可多个
	bool check_rule2(string owner);  // 可变借用只能一个
	bool check_rule3(string owner);  // 可变与不可变互斥
	bool check_rule4(string owner, int borrower_scope);  // 防悬垂

	// 查询
	bool is_borrowed(string owner);
	bool is_mut_borrowed(string owner);
	int get_borrow_count(string owner);
};

#endif // !BORROW_CHECKER_H
