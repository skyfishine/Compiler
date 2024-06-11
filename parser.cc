#include "parser.h"
#include <assert.h>
#include "error.h"
using namespace std;

unordered_map<TOKEN_TYPE, string> Token::typesname;

Parser::Parser(const string &in_filepath)
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
    // string message = "\"" + tk.original_value + "\"符号错误，应该输入\"" + expect + "\"类型的符号";
    string message = "\"" + tk.original_value + "\"前缺少\"" + expect + "\"";
    perror->printError(tk.line, message);
    throw ParserException(tk.line, message);
}

bool Parser::double_defined_error(const Token &tk)
{
    if (var_first_index != -1 &&
        vartable->find(tk.original_value, var_first_index, var_last_index))
    {
        string message = "符号\"" + tk.original_value + "\"重复定义";
        perror->printError(tk.line, message);
        return true;
    }
    return false;
}

void Parser::var_notdefined_error(const Token &tk)
{
    if (var_first_index == -1 ||
        (!vartable->find(tk.original_value, var_first_index, var_last_index) &&
         tk.original_value != proctable->getProcName(proc_index)))
    {
        string message = "标识符\"" + tk.original_value + "\"未定义";
        perror->printError(tk.line, message);
    }
}

void Parser::proc_notdefined_error(const Token &tk)
{
    assert(tk.type == $ID);
    if (!proctable->find(tk.original_value))
    {
        string message = "过程\"" + tk.original_value + "\"未定义";
        perror->printError(tk.line, message);
    }
}

void Parser::recover(TOKEN_TYPE t)
{
    do
    {
        advance();
    } while (sym.type != t && sym.type != $EOF && sym.type != $ERROR);
    assert(sym.type == t);
}

void Parser::recover(unordered_set<TOKEN_TYPE> predict)
{
    do
    {
        advance();
    } while (predict.count(sym.type) == 0 && sym.type != $EOF && sym.type != $ERROR);
    // assert(predict.count(sym.type) != 0);
}

void Parser::Procedure()
{
    SubProgram();
    vartable->dump(fvar);
    proctable->dump(fpro);
    if (!perror->getIsError())
        cout << "Parsing succeeded" << endl;
}

void Parser::SubProgram()
{
    // 假设最外层存在过程main
    proc_index = proctable->add("main", VOID, proc_level);
    try
    {
        match($BEGIN);
    }
    catch (ParserException e)
    {
        recover($INTEGER);
    }
    ExplanStmtTlb();
    // 说明语句结束，回填
    proctable->fillVarIndex(proc_index, var_first_index, var_last_index);

    try
    {
        match($SEM);
    }
    catch (ParserException e)
    {
        do
        {
            recover($SEM);
            advance();
        } while (sym.type == $BEGIN);
    }

    ExecStmtTlb();
    try
    {
        match($END); // 在匹配$END之后，自然会再匹配下一个token此时就能够把最后的eof读出来了
    }
    catch (ParserException e)
    {
    }
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
        if (look.type == $INTEGER || look.type == $SEM)
        {
            advance();
            ExplanStmt();
            ExplanStmtTlb_();
        }
    }
}

void Parser::ExplanStmt()
{
    if (sym.type == $SEM)
    {
        string message = "说明语句不能为空";
        perror->printError(sym.line, message);
        return;
    }
    try
    {
        match($INTEGER);
        // 函数说明语句
        if (sym.type == $FUNCTION)
        {
            advance();
            Token id = Ident();
            int prev_proc = proc_index;
            int prev_var_first_index = var_first_index;
            int prev_var_last_index = var_last_index;
            var_level++;
            var_first_index = -1;
            var_last_index = -1;
            proc_level++;
            proc_index = proctable->add(id.original_value, INT, proc_level);
            try
            {
                match($LPAR);
                Token param = Param();
                var_last_index = vartable->add(param.original_value, id.original_value, FORMAL_PARAM, INT, var_level);
                if (var_first_index == -1)
                {
                    var_first_index = var_last_index;
                }
                match($RPAR);
                match($SEM);
                FuncBody();
            }
            catch (ParserException e)
            {
                recover($SEM);
            }

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
            VKIND kind = VARIABLE;
            if (double_defined_error(token))
            {
                return;
            }
            // 加入到变量表
            var_last_index = vartable->add(token.original_value, proctable->getProcName(proc_index), kind, INT, var_level);
            // 如果是当前过程的第一个变量
            if (var_first_index == -1)
            {
                var_first_index = var_last_index;
            }
        }
    }
    catch (ParserException e)
    {
        // 可能抛出异常的情况：match函数，Var()
        recover($SEM); // 恢复程序，直到出现分号
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
    try
    {
        match($INTEGER);
    }
    catch(ParserException e) {
    }
    return Var();
}

void Parser::FuncBody()
{
    match($BEGIN);
    if (sym.type == $INTEGER)
    {
        ExplanStmtTlb();

        match($SEM);
    }
    // 声明语句完毕，所有变量都声明完了，所以可以更新过程名表
    // TODO: 如果过程没有定义局部变量，也没有传参，如何处理？
    proctable->fillVarIndex(proc_index, var_first_index, var_last_index);
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
    string message;
    if (sym.type == $SEM || sym.type == $END)
    {
        message = "执行语句不能为空";
        perror->printError(sym.line, message);
        return;
    }

    try
    {
        switch (sym.type)
        {
        case $READ:
            advance();
            if (sym.type == $LPAR)
            {
                advance();
            }
            else
            {
                string message = "缺少匹配的左括号";
                perror->printError(sym.line, message);
            }
            tk = Var();
            var_notdefined_error(tk);
            if (sym.type == $RPAR)
            {
                advance();
            }
            else
            {
                string message = "缺少匹配的右括号";
                perror->printError(sym.line, message);
            }
            break;
        case $WRITE:
            advance();
            match($LPAR);
            tk = Var();
            var_notdefined_error(tk);
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
        case $INTEGER:
            message = "不能在执行语句表中编写说明语句";
            perror->printError(sym.line, message);
            // 可以调用声明语句的过程，解析声明语句
            ExplanStmt();
            break;
        default:
            tk = Var();
            var_notdefined_error(tk);
            if (sym.type == $RPAR)
            {
                message = "缺少匹配的左括号";
                perror->printError(tk.line, message);
                recover({$SEM, $END});
            }
            else
            {
                if (sym.type == $ASSIGN)
                {
                    advance();
                    ArithExp();
                }
                else
                {
                    message = "赋值语句缺少\":=\"";
                    perror->printError(sym.line, message);
                    recover({$SEM, $END});
                }
            }
            break;
        }
    }
    catch (ParserException)
    {
        recover({$SEM, $END});
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
    // 变量or过程调用
    if (sym.type == $ID)
    {
        Token tk = sym;
        advance();
        if (sym.type == $LPAR)
        {
            // 过程调用
            proc_notdefined_error(tk);
            advance();
            ArithExp();
            match($RPAR);
        }
        else
        {
            // 变量引用
            var_notdefined_error(tk);
        }
    }
    else
    {
        match($INT);
    }
}

void Parser::FuncCall()
{
    Token tk = Ident();
    proc_notdefined_error(tk);
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
