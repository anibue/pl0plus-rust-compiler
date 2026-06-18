#pragma execution_character_set("utf-8")
#pragma once
#ifndef _SYMBOLTABLE_H_
#define _SYMBOLTABLE_H_
#include<set>
#include<string>
#include<iostream>
#include<map>
#include<vector>

#define inf 0x3f3f3f;

using std::set;
using std::string;
using std::cout;
using std::ostream;
using std::map;
using std::vector;
using std::pair;

struct cNode
{
	string name;
	int val;
	cNode(string s)
	{
		name = s;
		val = inf;
	}
	cNode(string s, int v)
	{
		name = s;
		val = v;
	}
	bool operator < (const cNode & a) const
	{
		return name < a.name;
	}
};

struct pos
{
	int pre, off;
	pos(int p, int o)
	{
		pre = p;
		off = o;
	}
};

class symbolTable
{
private:
	symbolTable* pre;
	set<string>v;
	set<string>c;
	set<string>p;
	set<cNode>cc;
public:
	symbolTable();
	symbolTable(symbolTable * pres);
	symbolTable* getPre();
	bool findVar(string s);
	bool findConst(string s);
	bool findProcedure(string s);
	bool findWithNoRecursive(string s);
	void addConst(string s);
	void addVar(string s);
	void addProcedure(string s);
	void addCConst(string s);
	void addCConst(string s, int v);
	void printTable();
};

class sTable
{
private:
	sTable* pre;
	vector<string>v;
	map<string, int>c;
	map<string, int>p;
public:
	sTable();
	sTable(sTable* pres);
	sTable* getPre();
	pos findVar(string s);
	int findConst(string s);
	pos findProcedure(string s);
	bool judgeVarOrConst(string s);
	void addConst(string s, int v);
	void addVar(string s);
	void addProcedure(string s);
	void setProcedure(string s, int v);
	int getVarSize();
};

enum class DeclKind {
	CONST,    // const x = 5;       - 不可变，立即初始化，不可借
	VAR,      // var x;             - 可变，不要求初始化，不可借
	LET,      // let x: i32 = 5;    - 不可变，立即初始化，可借 &
	LET_MUT   // let mut x: i32 = 5; - 可变，立即初始化，可借 &mut
};

struct Symbol {
	string name;
	string type;                  // "i8" | "i16" | "i32" | "&i32" | "&mut i32" | "integer"
	int    addr;                  // 栈地址
	int    scope_level;           // 作用域层级
	int    decl_line;             // 声明行
	int    first_assign_line;     // 第一次赋值行（-1 = 未赋值）
	bool   is_initialized;
	DeclKind decl_kind;
	int    borrow_count_imm;      // 不可变借用计数
	int    borrow_count_mut;      // 可变借用计数

	Symbol() : addr(0), scope_level(0), decl_line(0),
		first_assign_line(-1), is_initialized(false),
		decl_kind(DeclKind::VAR), borrow_count_imm(0), borrow_count_mut(0) {}

	Symbol(string n, string t, int a, int lvl, int line, DeclKind k)
		: name(n), type(t), addr(a), scope_level(lvl), decl_line(line),
		first_assign_line(-1), is_initialized(false), decl_kind(k),
		borrow_count_imm(0), borrow_count_mut(0) {}

	// 派生属性，不存状态
	bool is_mutable() const {
		return decl_kind == DeclKind::VAR || decl_kind == DeclKind::LET_MUT;
	}
	bool can_be_borrowed() const {
		return decl_kind == DeclKind::LET || decl_kind == DeclKind::LET_MUT;
	}
};

class RustSymbolTable {
private:
	vector<Symbol> symbols;
	int current_scope_level;  // ⭐ 新增：当前作用域层级
	stack<SymbolSnapshot> snapshot_stack;  // ⭐ 新增：快照栈

	struct SymbolSnapshot {
		vector<Symbol> symbols_copy;
		int scope_level_copy;
	};

public:
	RustSymbolTable() : current_scope_level(0) {}
	void declare(string name, string type, DeclKind kind, int line);
	Symbol* lookup(string name);
	bool exists(string name) const;
	void assign(string name, int line);
	void enter_scope();
	void exit_scope(int scope_level);
	void borrow_imm(string name);
	void borrow_mut(string name);
	void release_borrow(string owner, bool is_mut);  // ⭐ 精确释放单个借用
	void release_borrows(string name);
	int get_addr(string name);
	int get_scope_level() const { return current_scope_level; }  // ⭐ 新增
	int get_var_count() const { return symbols.size(); }  // ⭐ 新增：获取当前变量总数
	void printTable();

	// ⭐ 快照/回滚支持
	void take_snapshot();
	void rollback_snapshot();
	void commit_snapshot();
};

using RustSymbol = Symbol;

// 错误码常量
namespace ErrCode {
	const char* const E0001 = "E0001";  // 未声明标识符
	const char* const E0002 = "E0002";  // 重复声明
	const char* const E0010 = "E0010";  // 类型不匹配
	const char* const E0011 = "E0011";  // 缺少类型标注
	const char* const E0020 = "E0020";  // 借用规则违反
	const char* const E0021 = "E0021";  // 不可变借用冲突
	const char* const E0022 = "E0022";  // 可变借用冲突
	const char* const E0023 = "E0023";  // 跨过程借用
	const char* const E0024 = "E0024";  // 不可变变量被可变借用
	const char* const E0025 = "E0025";  // 解引用非引用类型
	const char* const E0026 = "E0026";  // 通过不可变借用赋值
	const char* const E0027 = "E0027";  // 悬垂借用
	const char* const E0040 = "E0040";  // const 被借用
	const char* const E0041 = "E0041";  // var 被借用
	const char* const E0099 = "E0099";  // 词法错误
}

#endif // !_SYMBOLTABLE_H
