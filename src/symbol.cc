#include <iomanip>
#include <assert.h>
#include <algorithm>
#include "symbol.h"

VarTable *VarTable::vartable = nullptr;
ProcTable *ProcTable::proctable = nullptr;

VarTable *VarTable::getInstance()
{
    if (vartable == nullptr)
    {
        vartable = new VarTable();
    }
    return vartable;
}

ProcTable *ProcTable::getInstance()
{
    if (proctable == nullptr)
    {
        proctable = new ProcTable();
    }
    return proctable;
}

int VarTable::add(const string &vname, const string &vproc, VKIND vkind, VTYPE vtype, int vlevel)
{
    table.emplace_back(vname, vproc, vkind, vtype, vlevel, table.size());
    return table.size() - 1;
}

bool VarTable::find(const string &varname, int start, int end)
{
    assert(start >= 0 && start < (int)table.size());
    assert(end >= 0 && end < (int)table.size());
    for (int i = start; i <= end; i++)
    {
        if (table[i].vname == varname)
        {
            return true;
        }
    }
    return false;
}

void VarTable::dump(ofstream &out)
{
    out << setw(16) << setfill(' ') << "vname";
    out << setw(16) << "vproc";
    out << setw(9) << "vkind";
    out << setw(11) << "vtype";
    out << setw(8) << "vlev";
    out << setw(8) << "vadr";

    for (const Var &v : table)
    {
        out << endl;
        out << setw(16) << v.vname;
        out << setw(16) << v.vproc;
        out << setw(9) << v.vkind;
        out << setw(11) << (v.vtype == INT ? "INTEGER" : "ERROR");
        out << setw(8) << v.vlev;
        out << setw(8) << v.vadr;
    }
}

// 向过程名表填写一条记录
// 返回当前记录的索引
int ProcTable::add(const string &pnmae, VTYPE ptype, int plev)
{
    table.emplace_back(pnmae, ptype, plev, -1, -1);
    return table.size() - 1;
}

string ProcTable::getProcName(int index)
{
    return table[index].pname;
}

void ProcTable::dump(ofstream &out)
{
    out << setw(16) << setfill(' ') << "pname";
    out << setw(11) << "ptype";
    out << setw(8) << "plev";
    out << setw(8) << "fadr";
    out << setw(8) << "ladr";
    for (const Proc &proc : table)
    {
        out << endl;
        string type = "INTEGER";
        if (proc.ptype == VOID)
            type = "VOID";
        out << setw(16) << proc.pname;
        out << setw(11) << type;
        out << setw(8) << proc.plev;
        out << setw(8) << proc.fadr;
        out << setw(8) << proc.ladr;
    }
}

void ProcTable::fillVarIndex(int index, int fadr, int ladr)
{
    table[index].fadr = fadr;
    table[index].ladr = ladr;
}

bool ProcTable::find(const string &procname)
{
    auto pos = find_if(table.begin(), table.end(), [&](const Proc& p){
        return procname == p.pname;
    });
    return pos != table.end();
}
