#pragma once
#include <string>
using namespace std;
#define $BEGIN 1
#define $END 2
#define $INTEGER 3
#define $IF 4
#define $THEN 5
#define $ELSE 6
#define $FUNCTION 7
#define $READ 8
#define $WRITE 9
#define $ID 10
#define $INT 11
#define $EQ 12
#define $NE 13
#define $LE 14
#define $LT 15
#define $GE 16
#define $GT 17
#define $SUB 18
#define $MUL 19
#define $ASSIGN 20
#define $LPAR 21
#define $RPAR 22
#define $SEM 23
#define $EOLN 24
#define $EOF 25
#define $ERROR 26

#define TOKEN_TYPE int

struct Token
{
    TOKEN_TYPE type;        // token类型
    string original_value;  // 原始内容，例如数字or标识符名
    int line;               // token所在行号
    Token():type($ERROR), line(-1){}
    Token(TOKEN_TYPE type) : type(type), line(-1) {}
    Token(TOKEN_TYPE type, const string& original_value, int line)
        : type(type), original_value(original_value), line(line){}
    Token& operator=(const Token& token) {
        type = token.type;
        original_value = token.original_value;
        return *this;
    }
};