#include "symbol.h"
#include <iomanip>

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

int VarTable::add(string vname, string vproc, VKIND vkind, VTYPE vtype, int vlevel)
{
    table.emplace_back(vname, vproc, vkind, vtype, vlevel, table.size());
    return table.size() - 1;
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
int ProcTable::add(string pnmae, VTYPE ptype, int plev)
{
    table.emplace_back(pnmae, ptype, plev, -1, -1);
    return table.size() - 1;
}

void ProcTable::fillFirstVar(int index, int fadr)
{
    table[index].fadr = fadr;
}
void ProcTable::fillLastVar(int index, int ladr)
{
    table[index].ladr = ladr;
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
    for(const Proc& proc : table) {
        out << endl;
        string type = "INTEGER";
        if(proc.ptype == VOID) type = "VOID";
        out << setw(16) << proc.pname;
        out << setw(11) << type;
        out << setw(8) << proc.plev;
        out << setw(8) << proc.fadr;
        out << setw(8) << proc.ladr;
    }
}
