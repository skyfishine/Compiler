#include "parser.h"
#include <assert.h>
using namespace std;

// Parser::Parser() : proc_level(0), var_level(0), proc_index(0), var_index(0)
// {
//     sym = lexer.analyzeWord();
//     vartable = VarTable::getInstance();
//     proctable = ProcTable::getInstance();
// }

Parser::Parser(string in_filepath) : lexer(in_filepath), proc_level(0), var_level(0), proc_index(0), var_index(0), look($ERROR)
{
    string filename = in_filepath.substr(0, in_filepath.find('.'));
    string var_filepath = filename + ".var";
    string pro_filepath = filename + ".pro";
    fvar.open(var_filepath);
    fpro.open(pro_filepath);
    advance();
    vartable = VarTable::getInstance();
    proctable = ProcTable::getInstance();
}

void Parser::analyze()
{
    Procedure();
}

Parser::~Parser()
{
    fvar.close();
    fpro.close();
}

void Parser::advance()
{
    if (look.type != $ERROR)
    {
        sym = look;
        look.type = $ERROR;
    }
    else
    {
        sym = lexer.analyzeAndDumpWord();
    }
}

void Parser::lookahead()
{
    look = lexer.analyzeAndDumpWord();
}

void Parser::match(TOKEN_TYPE t)
{

    assert(sym.type == t);
    if (sym.type == t)
    {
        advance();
    }
    else
    {
        // error
    }
}

void Parser::Procedure()
{
    SubProgram();
    vartable->dump(fvar);
    proctable->dump(fpro);
}

void Parser::SubProgram()
{
    // 假设最外层存在过程main
    proc_index = proctable->add("main", VOID, proc_level);
    match($BEGIN);
    ExplanStmtTlb();
    // 说明语句结束，回填
    proctable->fillLastVar(proc_index, var_table_index);
    match($SEM);
    ExecStmtTlb();
}

void Parser::ExplanStmtTlb()
{
    ExplanStmt();
    ExplanStmtTlb_();
}

void Parser::ExplanStmtTlb_()
{
    if (sym.type == $SEM)
    {
        // 超前查看，解决冲突
        lookahead();
        if (look.type == $INTEGER)
        {
            advance();
            ExplanStmt();
            ExplanStmtTlb_();
        }
    }
}

bool Parser::isInFormalList(const string &var)
{
    auto it = formal_vars.begin();
    for (; it != formal_vars.end(); it++)
    {
        if (*it == var)
        {
            formal_vars.erase(it);
            return true;
        }
    }
    return false;
}

void Parser::ExplanStmt()
{
    match($INTEGER);
    // 函数说明语句
    if (sym.type == $FUNCTION)
    {
        advance();
        Token id = Ident();
        int prev_proc = proc_index;
        int prev_var_table_index = var_table_index;
        int prev_var_index = var_index;  // 防止函数声明在变量声明前面
        var_level++;
        var_index = 0;
        var_table_index = -1;
        proc_level++;
        proc_index = proctable->add(id.original_value, INT, proc_level);
        match($LPAR);
        Token param = Param();
        formal_vars.push_back(param.original_value);
        match($RPAR);
        match($SEM);
        FuncBody();
        var_level--;
        proc_level--;
        proc_index = prev_proc;
        var_table_index = prev_var_table_index;
        var_index = prev_var_index;
    }
    // 变量说明语句
    else
    {
        Token token = Var();
        // 查形参列表，看是否为形参
        VKIND kind = VARIABLE;
        if (isInFormalList(token.original_value))
        {
            kind = FORMAL_PARAM;
        }
        // 加入到变量表
        var_table_index = vartable->add(token.original_value, proctable->getProcName(proc_index), kind, INT, var_level);
        // 如果是当前过程的第一个变量，那么更新过程名表
        if (var_index == 0)
        {
            proctable->fillFirstVar(proc_index, var_table_index);
        }
        var_index++;

        // error 可能要错误处理
    }
}

Token Parser::Var()
{
    return Ident();
}

Token Parser::Ident()
{
    Token token = sym;
    if (sym.type == $ID)
    {
        advance();
        return token;
    }
    else
    {
        // error
    }
    return sym;
}

Token Parser::Param()
{
    return Var();
}

void Parser::FuncBody()
{
    match($BEGIN);
    ExplanStmtTlb();
    // 声明语句完毕，所有变量都声明完了，所以可以更新过程名表
    proctable->fillLastVar(proc_index, var_table_index);
    match($SEM);
    ExecStmtTlb();
    match($END);
}

void Parser::ExecStmtTlb()
{
    ExecStmt();
    ExecStmtTlb_();
}

void Parser::ExecStmtTlb_()
{
    if (sym.type == $SEM)
    {
        advance();
        ExecStmt();
        ExecStmtTlb_();
    }
}

void Parser::ExecStmt()
{
    switch (sym.type)
    {
    case $READ:
        advance();
        match($LPAR);
        Var();
        match($RPAR);
        break;
    case $WRITE:
        advance();
        match($LPAR);
        Var();
        match($RPAR);
        break;
    case $IF:
        advance();
        CondExp();
        match($THEN);
        ExecStmt();
        match($ELSE);
        ExecStmt();
        break;
    default:
        Var();
        match($ASSIGN);
        ArithExp();
        break;
    }
}

void Parser::ArithExp()
{
    Item();
    ArithExp_();
}

void Parser::ArithExp_()
{
    if (sym.type == $SUB)
    {
        advance();
        Item();
        ArithExp_();
    }
}

void Parser::Item()
{
    Factor();
    Item_();
}

void Parser::Item_()
{
    if (sym.type == $MUL)
    {
        advance();
        Factor();
        Item_();
    }
}

void Parser::Factor()
{
    if (sym.type == $ID)
    {
        advance();
        if (sym.type == $LPAR)
        {
            advance();
            ArithExp();
            match($RPAR);
        }
    }
    else
    {
        match($INT);
    }
}

void Parser::FuncCall()
{
    Ident();
    match($LPAR);
    ArithExp();
    match($RPAR);
}

void Parser::CondExp()
{
    ArithExp();
    RelOp();
    ArithExp();
}

void Parser::RelOp()
{
    switch (sym.type)
    {
    case $LT:
    case $LE:
    case $GT:
    case $GE:
    case $EQ:
    case $NE:
        advance();
        break;
    default:
        // error();
        break;
    }
}
