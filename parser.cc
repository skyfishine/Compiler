#include "parser.h"
#include <assert.h>
#include "error.h"
using namespace std;

unordered_map<TOKEN_TYPE, string> Token::typesname;

Parser::Parser(string in_filepath)
    : lexer(in_filepath), look($ERROR), var_level(0), var_first_index(-1), var_last_index(-1), proc_level(0), proc_index(0)
{
    string filename = in_filepath.substr(0, in_filepath.find('.'));
    string var_filepath = filename + ".var";
    string pro_filepath = filename + ".pro";
    fvar.open(var_filepath, ios::out);
    fpro.open(pro_filepath, ios::out);
    perror = Error::getInstance();
    advance();
    vartable = VarTable::getInstance();
    proctable = ProcTable::getInstance();
    Token::initTypeNames();
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

    if (sym.type == t)
    {
        advance();
    }
    else
    {
        match_error(sym, Token::typesname[t]);
    }
}

void Parser::match_error(const Token &tk, const string &expect)
{
    string message = "\"" + tk.original_value + "\"符号错误，应该输入\"" + expect + "\"类型的符号";
    perror->printError(tk.line, message);
    assert(0);
}

void Parser::var_isdefined(const Token &tk)
{
    if (!vartable->find(tk.original_value, var_first_index, var_last_index) && tk.original_value != proctable->getProcName(proc_index))
    {
        notdefined_error(tk);
    }
}

void Parser::notdefined_error(const Token &tk)
{
    string message = "符号\"" + tk.original_value + "\"未定义";
    perror->printError(tk.line, message);
    assert(0);
}

void Parser::Procedure()
{
    SubProgram();
    lexer.analyzeAndDumpWord();  // 最后输出eof
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
    proctable->fillVarIndex(proc_index, var_first_index, var_last_index);
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
        // 暂存，防止函数声明在变量声明前面
        // TODO：其实可能没什么意义？因为这样的话中间函数声明的变量会被当成当前函数的
        // 实际上，用proc的数组，然后每个proc中有多个variable，这样的结构更好
        // 但是懒得改代码了！
        int prev_var_first_index = var_first_index;
        int prev_var_last_index = var_last_index;
        var_level++;
        var_first_index = -1;
        var_last_index = -1;
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
        var_first_index = prev_var_first_index;
        var_last_index = prev_var_last_index;
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
        var_last_index = vartable->add(token.original_value, proctable->getProcName(proc_index), kind, INT, var_level);
        // 如果是当前过程的第一个变量
        if (var_first_index == -1)
        {
            var_first_index = var_last_index;
        }

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
        match_error(sym, Token::typesname[$ID]);
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
    proctable->fillVarIndex(proc_index, var_first_index, var_last_index);
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
    Token tk;
    switch (sym.type)
    {
    case $READ:
        advance();
        match($LPAR);
        tk = Var();
        var_isdefined(tk);
        match($RPAR);
        break;
    case $WRITE:
        advance();
        match($LPAR);
        tk = Var();
        var_isdefined(tk);
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
        tk = Var();
        var_isdefined(tk);
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
    // 标识符or函数调用
    if (sym.type == $ID)
    {
        Token tk = sym;
        advance();
        if (sym.type == $LPAR)
        {
            advance();
            ArithExp();
            match($RPAR);
        }
        else
        {
            var_isdefined(tk);
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
        match_error(sym, "关系运算符");
        break;
    }
}
