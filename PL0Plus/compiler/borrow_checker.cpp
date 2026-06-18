#pragma execution_character_set("utf-8")
#include "pch.h"
#include "borrow_checker.h"

// 语句级别 API
void BorrowTracker::begin_statement() {
	Snapshot snap;
	snap.records_copy = records;
	snap.scope_level_copy = current_scope_level;
	snapshot_stack.push(snap);
	sym_table->take_snapshot();
}

void BorrowTracker::end_statement() {
	if (!snapshot_stack.empty()) {
		snapshot_stack.pop();
	}
	sym_table->commit_snapshot();
}

void BorrowTracker::abort_statement() {
	if (snapshot_stack.empty()) return;
	Snapshot snap = snapshot_stack.top();
	snapshot_stack.pop();
	records = snap.records_copy;
	current_scope_level = snap.scope_level_copy;
	sym_table->rollback_snapshot();
}

// 作用域管理
void BorrowTracker::enter_scope() {
	current_scope_level++;
	sym_table->enter_scope();
}

void BorrowTracker::exit_scope() {
	// 释放当前作用域的所有借用
	for (auto& r : records) {
		if (r.borrower_scope == current_scope_level && r.alive) {
			r.alive = false;
			sym_table->release_borrow(r.owner, r.is_mut);
		}
	}
	sym_table->exit_scope(current_scope_level);
	current_scope_level--;
}

// 借用操作
bool BorrowTracker::borrow_imm(string borrower, string owner, int line) {
	// Rule 0: 检查 owner 是否已初始化
	if (!check_rule0(owner)) {
		return false;  // E0030 或 E0001
	}

	// DeclKind 检查: const/var 不可借用
	RustSymbol* sym = sym_table->lookup(owner);
	if (sym) {
		if (sym->decl_kind == DeclKind::CONST) return false;  // E0040
		if (sym->decl_kind == DeclKind::VAR) return false;    // E0041
	}

	// Rule 1: 不可变借用可多个（但不能有可变借用）
	if (!check_rule1(owner)) {
		return false;  // E0021
	}

	// Rule 4: 防悬垂
	if (!check_rule4(owner, current_scope_level)) {
		return false;  // E0027
	}

	// 记录借用
	BorrowRecord record;
	record.borrower = borrower;
	record.owner = owner;
	record.is_mut = false;
	record.borrow_line = line;
	record.owner_scope = sym ? sym->scope_level : 0;
	record.borrower_scope = current_scope_level;
	record.alive = true;
	records.push_back(record);

	// 更新符号表
	sym_table->borrow_imm(owner);

	return true;
}

bool BorrowTracker::borrow_mut(string borrower, string owner, int line) {
	// Rule 0: 检查 owner 是否已初始化
	if (!check_rule0(owner)) {
		return false;  // E0030 或 E0001
	}

	RustSymbol* sym = sym_table->lookup(owner);
	if (sym) {
		// DeclKind 检查: const/var 不可借用
		if (sym->decl_kind == DeclKind::CONST) return false;  // E0040
		if (sym->decl_kind == DeclKind::VAR) return false;    // E0041

		// 不可变变量被可变借用
		if (sym->decl_kind == DeclKind::LET) return false;    // E0024
	}

	// Rule 2: 可变借用只能一个
	if (!check_rule2(owner)) {
		return false;  // E0022
	}

	// Rule 3: 可变与不可变互斥
	if (!check_rule3(owner)) {
		return false;  // E0021
	}

	// Rule 4: 防悬垂
	if (!check_rule4(owner, current_scope_level)) {
		return false;  // E0027
	}

	// 记录借用
	BorrowRecord record;
	record.borrower = borrower;
	record.owner = owner;
	record.is_mut = true;
	record.borrow_line = line;
	record.owner_scope = sym ? sym->scope_level : 0;
	record.borrower_scope = current_scope_level;
	record.alive = true;
	records.push_back(record);

	// 更新符号表
	sym_table->borrow_mut(owner);

	return true;
}

void BorrowTracker::release_borrow(string borrower) {
	for (auto& r : records) {
		if (r.borrower == borrower && r.alive) {
			r.alive = false;
			sym_table->release_borrow(r.owner, r.is_mut);
		}
	}
}

// 规则检查
bool BorrowTracker::check_rule0(string owner) {
	RustSymbol* sym = sym_table->lookup(owner);
	if (!sym) return false;  // E0001: 未声明
	if (!sym->is_initialized) return false;  // E0030: 未初始化
	return true;
}

bool BorrowTracker::check_rule1(string owner) {
	// 不可变借用可以多个同时存在，但不能有可变借用
	for (const auto& r : records) {
		if (r.owner == owner && r.is_mut && r.alive) {
			return false;  // 已存在可变借用
		}
	}
	return true;
}

bool BorrowTracker::check_rule2(string owner) {
	// 可变借用只能有一个
	for (const auto& r : records) {
		if (r.owner == owner && r.is_mut && r.alive) {
			return false;  // 已存在可变借用
		}
	}
	return true;
}

bool BorrowTracker::check_rule3(string owner) {
	// 可变与不可变互斥：如果要创建可变借用，不能有任何借用存在
	for (const auto& r : records) {
		if (r.owner == owner && r.alive) {
			return false;  // 已存在任何借用
		}
	}
	return true;
}

bool BorrowTracker::check_rule4(string owner, int borrower_scope) {
	RustSymbol* sym = sym_table->lookup(owner);
	if (!sym) return false;

	int owner_scope = sym->scope_level;

	// 借用者的作用域层级必须 >= 所有者的作用域层级
	// 如果借用者在更外层（数字更小），而所有者在更内层，则内层销毁时会导致悬垂
	if (borrower_scope < owner_scope) {
		return false;  // E0027: 悬垂借用
	}
	return true;
}

// 查询
bool BorrowTracker::is_borrowed(string owner) {
	for (const auto& r : records) {
		if (r.owner == owner && r.alive) return true;
	}
	return false;
}

bool BorrowTracker::is_mut_borrowed(string owner) {
	for (const auto& r : records) {
		if (r.owner == owner && r.is_mut && r.alive) return true;
	}
	return false;
}

int BorrowTracker::get_borrow_count(string owner) {
	int count = 0;
	for (const auto& r : records) {
		if (r.owner == owner && r.alive) count++;
	}
	return count;
}
