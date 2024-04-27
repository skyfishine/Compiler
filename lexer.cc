#include <iostream>
#include <fstream>
#include <iomanip>
#include "lexer.h"
using namespace std;

void Lexer::getchar()
{
    if (retract_char)
    {
        cha = retract_char;
        retract_char = 0;
    }
    else
    {
        if (fin.get(cha).eof())
        {
            cha = -1;
            return;
        }

        if (cha == '\n')
            line++;
    }
}

void Lexer::getnb()
{
    while (cha == ' ')
    {
        getchar();
    }
}

void Lexer::retract()
{
    retract_char = cha;
    cha = ' ';
    if (retract_char == '\n')
    {
        --line;
    }
}

Token Lexer::error(int i)
{
    string err;
    switch (i)
    {
    case 1:
        ferror << "第" << line << "行 ";
        ferror << "标识符长度过长 \"" << token << "\"" << endl;
        break;
    case 2:
        ferror << "第" << line << "行 ";
        ferror << "非法字符 \"" << cha << "\"" << endl;
        break;
    case 3:
        ferror << "第" << line << "行 ";
        ferror << "常数常量太大 \"" << token << "\"" << endl;
    default:
        break;
    }
    return {$ERROR, "ERROR", line};
}

bool Lexer::letter()
{
    return cha >= 'a' && cha <= 'z';
}

bool Lexer::digit()
{
    return cha >= '0' && cha <= '9';
}

void Lexer::concat()
{
    token.push_back(cha);
}

int Lexer::reserve()
{
    if (reserve_map.find(token) != reserve_map.end())
    {
        return reserve_map[token];
    }
    else
    {
        return 0;
    }
}

char Lexer::type(char ch)
{
    if (ch >= '0' && ch <= '9')
        return '0';
    if (ch >= 'a' && ch <= 'z')
        return 'a';
    if (ch == '\r' || ch == '\n')
        return '\n';
    return ch;
}

Token Lexer::case_letter()
{
    int len = 0;
    while (letter() || digit())
    {
        concat();
        getchar();
        ++len;
    }
    retract();
    if (len > 16)
    {
        return error(1);
    }
    int c = reserve();
    if (c == 0)
    {
        return {$ID, token, line};
    }
    else
        return {c, token, line}; // 关键字
}

Token Lexer::case_number()
{
    int len = 0;
    while (digit())
    {
        concat();
        getchar();
        ++len;
    }
    retract();
    if (len > 16)
    {
        return error(3);
    }
    return {$INT, token, line};
}

Token Lexer::case_lt()
{
    concat();
    getchar();
    if (cha == '=')
    {
        concat();
        return {$LE, token, line};
    }
    else if (cha == '>')
    {
        concat();
        return {$NE, token, line};
    }
    retract();
    return {$LT, token, line};
}

Token Lexer::case_gt()
{
    getchar();
    if (cha == '=')
        return {$GE, ">=", line};
    retract();
    return {$GT, ">", line};
}

Token Lexer::case_assign()
{
    concat();
    getchar();
    if (cha == '=')
    {
        concat();
        return {$ASSIGN, token, line};
    }
    else
        return error(2);
}

Token Lexer::analyzeWord()
{
    token.clear();
    getchar();
    getnb();
    char t = type(cha);

    switch (t)
    {
    case 'a':
        return case_letter();
    case '0':
        return case_number();
    case '=':
        return {$EQ, "=", line};
    case '-':
        return {$SUB, "-", line};
    case '*':
        return {$MUL, "*", line};
    case '(':
        return {$LPAR, "(", line};
    case ')':
        return {$RPAR, ")", line};
    case ';':
        return {$SEM, ";", line};
    case '<':
        return case_lt();
    case '>':
        return case_gt();
    case ':':
        return case_assign();
    case '\n':
        return {$EOLN, "EOLN", line};
    case -1:
        return {$EOF, "EOF", line};
    default:
        return error(2);
    }
    // return Token();
}

void Lexer::createReserveMap()
{
    reserve_map["begin"] = $BEGIN;
    reserve_map["end"] = $END;
    reserve_map["integer"] = $INTEGER;
    reserve_map["if"] = $IF;
    reserve_map["then"] = $THEN;
    reserve_map["else"] = $ELSE;
    reserve_map["function"] = $FUNCTION;
    reserve_map["read"] = $READ;
    reserve_map["write"] = $WRITE;
}

Lexer::Lexer(string in_filepath, string out_filepath, string error_filepath) : line(1), retract_char(0)
{
    createReserveMap();
    fin.open(in_filepath, ios::in);
    fout.open(out_filepath, ios::out);
    ferror.open(error_filepath, ios::out);
}

Lexer::Lexer(string in_filepath)
{

    string filename = in_filepath.substr(0, in_filepath.find('.'));
    string out_filepath = filename + ".dyd";
    string error_filepath = filename + ".err";
    // placement new
    new (this) Lexer(in_filepath, out_filepath, error_filepath);
}

Lexer::~Lexer()
{
    fin.close();
    fout.close();
    ferror.close();
}

void Lexer::dump(Token ws)
{
    if (ws.type == $ERROR)
        return;
    fout << setw(16) << setfill(' ') << ws.original_value;
    fout << ' ';
    fout << setw(2) << setfill('0') << ws.type;
    if (ws.type != $EOF)
        fout << endl;
}

Token Lexer::analyzeAndDumpWord()
{
    Token tk;
    if (!fin.eof())
    {
        do
        {
            tk = analyzeWord();
            dump(tk);
        } while (tk.type == $EOLN);
    }
    else
    {
        tk = {$EOF, "EOF", line};
        dump(tk);
    }
    return tk;
}

void Lexer::LexicalAnalyze()
{
    while (!fin.eof())
    {
        dump(analyzeWord());
    }
    dump({$EOF, "EOF", line});
}