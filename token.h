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
    TOKEN_TYPE type;
    int value;
    string original_value;
    Token():type($ERROR){}
    Token(TOKEN_TYPE type) : type(type) {}
    Token(TOKEN_TYPE type, int value, const string& original_value)
        : type(type), value(value), original_value(original_value){}
    Token& operator=(const Token& token) {
        type = token.type;
        value = token.value;
        original_value = token.original_value;
        return *this;
    }
};