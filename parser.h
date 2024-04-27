#pragma once
#include <fstream>
#include <list>
#include "token.h"
#include "lexer.h"
#include "symbol.h"

using namespace std;

class Parser
{
private:
    Lexer lexer;
    Token sym;                // 即将匹配的符号
    Token look;               // 用于超前查看，若type为error，则表示没有超前查看
    ofstream fvar, fpro;      // 变量名表，过程名表
    int var_level;            // 当前变量层次
    int var_index;            // 当前变量是当前过程中的第几个变量
    int var_table_index;      // 当前变量在符号表中的位置
    int proc_level;           // 当前过程层次
    int proc_index;           // 当前过程在过程表的下标
    list<string> formal_vars; // 形参列表
    VarTable *vartable;       // 变量名表
    ProcTable *proctable;     // 过程名表

    // 匹配单词符号，读入下一符号
    void advance();

    // 超前查看一个符号
    void lookahead();

    // 匹配单词符号,并读入下一符号
    void match(TOKEN_TYPE t);

    // 判断当前变量是否为形参
    bool isInFormalList(const string &var);

    // <程序> -> <分程序>
    // <Program> -> <SubProgram>
    void Procedure();

    // <子程序> -> begin <说明语句表>;<执行语句表> end
    // <SubProgram>->begin <ExplanStmtTlb>;<ExecStmtTlb> end
    void SubProgram();

    // <说明语句表> -> <说明语句><说明语句表_>
    // <ExplanStmtTlb> -> <ExplanStmt><ExplanStmtTlb_>
    void ExplanStmtTlb();

    // <说明语句表_> -> ;<说明语句><说明语句表_>|ε
    // <ExplanStmtTlb_> -> ;<ExplanStmt><ExplanStmtTlb_>|ε
    void ExplanStmtTlb_();

    // <说明语句> -> integer <变量>
    //  | integer function <标识符> ( <参数> ) ; <函数体>
    // <ExplanStmt> -> integer <Var>
    //  | integer function <Ident>(<Param>);<FuncBody>
    // 备注：此处在消除了两个都为终结符开头的文法
    void ExplanStmt();

    // <变量> -> <标识符>
    // <Var> -> <Ident>
    Token Var();

    // <标识符>
    // <Ident>
    // 已经在词法分析的时候识别出了
    Token Ident();

    // <参数> -> <变量>
    // <Param> -> <Var>
    Token Param();

    // <函数体>->begin <说明语句表>;<执行语句表> end
    // <FuncBody>->begin <ExplanStmtTlb>;<ExecStmtTlb> end
    void FuncBody();

    // <执行语句表> -> <执行语句><执行语句表_>
    // <ExecStmtTlb> -> <ExecStmt><ExecStmtTlb_>
    void ExecStmtTlb();

    // <执行语句表_> -> ;<执行语句><执行语句表_>|ε
    // <ExecStmtTlb_> -> ;<ExecStmt><ExecStmtTlb_>|ε
    void ExecStmtTlb_();

    // <执行语句> -> <读语句> | <写语句> | <赋值语句> | <条件语句>
    // <ExecStmt> -> <ReadStmt> | <WriteStmt> | <AssignStmt> | <CondStmt>
    // 改写：
    // <ExecStmt> -> read(<Var>)
    //  | write(<Var>)
    //  | <Var>:=<ArithExp>
    //  | if <CondExp> then <ExecStmt> else <ExecStmt>
    void ExecStmt();

    // <算术表达式> -> <项><算术表达式_>
    // <ArithExp> -> <Item><ArithExp_>
    void ArithExp();

    // <算术表达式_>-> -<项><算术表达式_> | ε
    // <ArithExp_> -> -<Item><ArithExp_> | ε
    void ArithExp_();

    // <项> -> <因子><项_>
    // <Item> ->  <Factor><Item_>
    void Item();

    // <项_> -> *<因子><项_> | ε
    // <Item_> -> *<Factor><Item_>|ε
    void Item_();

    // <因子> -> <变量> | <常数> | <函数调用>
    // <Factor> -> <Var> | <Const> | <FuncCall>
    // 改写：
    // <Factor> -> <Ident>
    //  | <Number>
    //  | <Ident>(<ArithExp>)
    void Factor();

    // <函数调用> -> <标识符>(<算术表达式>)
    // <FuncCall> -> <Ident>(<ArithExp>)
    void FuncCall();

    // <条件表达式> -> <算术表达式><关系运算符><算术表达式>
    // <CondExp> -> <ArithExp><RelOp><ArithExp>
    void CondExp();

    // <关系运算符>  -> < | <= | > | >= | = | <>
    // <RelOp>  -> < | <= | > | >= | = | <>
    void RelOp();

public:
    Parser(string in_filepath);
    void analyze();
    ~Parser();
};
