#include "token.h"
// 在parser中对该map进行初始化
// 用于错误处理
void Token::initTypeNames()
{
    if (!typesname.empty())
        return;
    typesname[$BEGIN] = "begin";
    typesname[$END] = "end";
    typesname[$INTEGER] = "integer";
    typesname[$IF] = "if";
    typesname[$THEN] = "then";
    typesname[$ELSE] = "else";
    typesname[$FUNCTION] = "function";
    typesname[$READ] = "read";
    typesname[$WRITE] = "write";
    typesname[$ID] = "标识符";
    typesname[$INT] = "数字";
    typesname[$EQ] = "=";
    typesname[$NE] = "<>";
    typesname[$LE] = "<=";
    typesname[$LT] = "<";
    typesname[$GE] = ">=";
    typesname[$GT] = ">";
    typesname[$SUB] = "-";
    typesname[$MUL] = "*";
    typesname[$ASSIGN] = ":=";
    typesname[$LPAR] = "(";
    typesname[$RPAR] = ")";
    typesname[$SEM] = ";";
    typesname[$EOLN] = "EOLN";
    typesname[$EOF] = "EOF";
    typesname[$ERROR] = "ERROR";
}