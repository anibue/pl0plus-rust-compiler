#pragma execution_character_set("utf-8")
#include "pch.h"
#include "symbolTable.h"

symbolTable::symbolTable()
{
	pre = nullptr;
	//c = set<string>{};
	//v = set<string>{};
	//p = set<string>{};
}

symbolTable::symbolTable(symbolTable * pres)
{
	pre = pres;
	//c = set<string>{};
	//v = set<string>{};
	//p = set<string>{};
}

symbolTable * symbolTable::getPre()
{
	return pre;
}

bool symbolTable::findVar(string s)
{
	symbolTable* tmp = this;
	while (tmp != nullptr)
	{
		if (tmp->v.find(s) != tmp->v.end())
			return true;
		tmp = tmp->pre;
	}
	return false;
}

bool symbolTable::findConst(string s)
{
	symbolTable* tmp = this;
	while (tmp != nullptr)
	{
		if (tmp->c.find(s) != tmp->c.end())
			return true;
		tmp = tmp->pre;
	}
	return false;
}

bool symbolTable::findProcedure(string s)
{
	//语法分析中不可跨级调用Procedure，但是生成语义分析和pcode的时候可以（生成pcode时，跨级调用无法回填procedure的地址）
	/*symbolTable* tmp = this;
	while (tmp != nullptr)
	{
		if (tmp->p.find(s) != tmp->p.end())
			return true;
		tmp = tmp->pre;
	}
	return false;*/
	if (p.find(s) != p.end())
		return true;
	return false;
}

bool symbolTable::findWithNoRecursive(string s)
{
	if (v.find(s) == v.end() && c.find(s) == c.end() && p.find(s) == p.end())
		return false;
	return true;
}

void symbolTable::addConst(string s)
{
	c.insert(s);
}

void symbolTable::addVar(string s)
{
	v.insert(s);
}

void symbolTable::addProcedure(string s)
{
	p.insert(s);
}

void symbolTable::addCConst(string s)
{
	cc.insert(cNode(s));
}

void symbolTable::addCConst(string s, int v)
{
	cc.insert(cNode(s, v));
}

void symbolTable::printTable()
{
	int js = 1;
	symbolTable* tmp = this;
	while (tmp->pre != nullptr)
	{
		++js;
		tmp = tmp->pre;
	}
	cout << "Level:" << js;
	cout << "\nconst: ";
	for (set<string>::iterator it = c.begin(); it != c.end(); ++it)
	{
		cout << *it << " ";
	}
	cout << "\nvar: ";
	for (set<string>::iterator it = v.begin(); it != v.end(); ++it)
	{
		cout << *it << " ";
	}
	cout << "\nprocedure: ";
	for (set<string>::iterator it = p.begin(); it != p.end(); ++it)
	{
		cout << *it << " ";
	}
	cout << "\n";
}

sTable::sTable()
{
	pre = nullptr;
}

sTable::sTable(sTable * pres)
{
	pre = pres;
}

sTable * sTable::getPre()
{
	return pre;
}

pos sTable::findVar(string s)
{
	int p = 0, o;
	sTable* tmp = this;
	while (1)
	{
		for (int i = 0; i < tmp->v.size(); ++i)
		{
			if (tmp->v[i] == s)
			{
				o = 3 + i;
				return pos(p, o);
			}
		}
		++p;
		tmp = tmp->getPre();
	}
}

int sTable::findConst(string s)
{
	sTable* tmp = this;
	while (1)
	{
		if (tmp->c.find(s) != tmp->c.end())
		{
			return tmp->c.find(s)->second;
		}
		tmp = tmp->getPre();
	}
}

pos sTable::findProcedure(string s)
{
	int p = 0, o;
	sTable* tmp = this;
	while (1)
	{
		if (tmp->p.find(s) != tmp->p.end())
		{
			return pos(p, tmp->p.find(s)->second);
		}
		++p;
		tmp = tmp->getPre();
	}
}

bool sTable::judgeVarOrConst(string s)
{
	sTable* tmp = this;
	while (1)
	{
		if (tmp->c.find(s) != tmp->c.end())
		{
			return false;
		}
		for (int i = 0; i < tmp->v.size(); ++i)
		{
			if (tmp->v[i] == s)
			{
				return true;
			}
		}
		tmp = tmp->getPre();
	}
}

void sTable::addConst(string s, int v)
{
	c.insert(pair<string, int>{s, v});
}

void sTable::addVar(string s)
{
	v.push_back(s);
}

void sTable::addProcedure(string s)
{
	p.insert(pair<string, int>{s, 0x3f3f3f});
}

void sTable::setProcedure(string s, int v)
{
	p.find(s)->second = v;
}

int sTable::getVarSize()
{
	return v.size();
}

void RustSymbolTable::declare(string name, string type, DeclKind kind, int line) {
	// 检查重复声明
	for (const auto& s : symbols) {
		if (s.name == name) {
			return;  // E0002 重复声明
		}
	}
	int next_addr = symbols.size();
	symbols.push_back(Symbol(name, type, next_addr, 0, line, kind));
}

Symbol* RustSymbolTable::lookup(string name) {
	for (auto& s : symbols) {
		if (s.name == name) {
			return &s;
		}
	}
	return nullptr;
}

bool RustSymbolTable::exists(string name) const {
	for (const auto& s : symbols) {
		if (s.name == name) return true;
	}
	return false;
}

void RustSymbolTable::assign(string name, int line) {
	Symbol* s = lookup(name);
	if (s) {
		if (!s->is_initialized) {
			s->first_assign_line = line;
		}
		s->is_initialized = true;
	}
}

void RustSymbolTable::enter_scope() {
	for (auto& s : symbols) {
		s.scope_level++;
	}
}

void RustSymbolTable::exit_scope(int scope_level) {
	// 移除该层的所有符号，递减借用的 owner 的计数
	vector<Symbol> remaining;
	for (auto& s : symbols) {
		if (s.scope_level == scope_level) {
			continue;  // 离开作用域，丢弃
		}
		remaining.push_back(s);
	}
	symbols = remaining;
}

void RustSymbolTable::borrow_imm(string name) {
	Symbol* s = lookup(name);
	if (s) s->borrow_count_imm++;
}

void RustSymbolTable::borrow_mut(string name) {
	Symbol* s = lookup(name);
	if (s) s->borrow_count_mut++;
}

void RustSymbolTable::release_borrows(string name) {
	Symbol* s = lookup(name);
	if (s) {
		s->borrow_count_imm = 0;
		s->borrow_count_mut = 0;
	}
}

int RustSymbolTable::get_addr(string name) {
	RustSymbol* s = lookup(name);
	return s ? s->addr : -1;
}

void RustSymbolTable::printTable() {
	std::cout << "=== Rust Symbol Table ===\n";
	for (const auto& s : symbols) {
		std::cout << "  " << s.name << " : " << s.type
			<< "  scope=" << s.scope_level
			<< "  line=" << s.decl_line
			<< "  init=" << (s.is_initialized ? "Y" : "N")
			<< "  borrow(imm=" << s.borrow_count_imm
			<< ", mut=" << s.borrow_count_mut << ")\n";
	}
}
