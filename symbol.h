// 用于符号表管理
#pragma once
#include <vector>
#include <string>
#include <fstream>
#include "token.h"
using namespace std;

enum VKIND
{
    VARIABLE,    // 变量
    FORMAL_PARAM // 形参
};

enum VTYPE
{
    INT, // int类型
    VOID
};

struct Var
{
    string vname; // 变量名
    string vproc; // 所属过程
    VKIND vkind;  // 变量分类，变量or形参
    VTYPE vtype;  // 变量类型，int...
    int vlev;     // 变量层次
    int vadr;     // 变量在变量表位置
    Var(string vname, string vproc, VKIND vkind, VTYPE vtype, int vlev, int vadr)
        : vname(vname), vproc(vproc), vkind(vkind), vtype(vtype), vlev(vlev), vadr(vadr) {}
};

struct Proc
{
    string pname; // 过程名
    VTYPE ptype;  // 过程类型
    int plev;     // 过程层次
    int fadr;     // 第一个变量在变量表位置
    int ladr;     // 最后一个变量表在变量表位置
    Proc(string pname, VTYPE ptype, int plev, int fadr, int ladr)
        : pname(pname), ptype(ptype), plev(plev), fadr(fadr), ladr(ladr) {}
};

class VarTable
{
private:
    static VarTable *vartable;
    vector<Var> table;
    VarTable() = default;

public:
    static VarTable *getInstance();
    int add(const string &vname, const string &vproc, VKIND vkind, VTYPE vtype, int vlevel);
    bool find(const string &varname, int start, int end);
    void dump(ofstream &out);
    VarTable(VarTable const &) = delete;
    VarTable &operator=(VarTable const &) = delete;
    ~VarTable() = default;
};

class ProcTable
{
private:
    static ProcTable *proctable;
    vector<Proc> table;
    ProcTable() = default;

public:
    static ProcTable *getInstance();
    int add(const string &pnmae, VTYPE ptype, int plev);
    string getProcName(int index);
    void fillVarIndex(int index, int fadr, int ladr);
    void dump(ofstream &out);
    ProcTable(ProcTable const &) = delete;
    ProcTable &operator=(ProcTable const &) = delete;
    ~ProcTable() = default;
};
