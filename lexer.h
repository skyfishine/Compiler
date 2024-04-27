#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include "token.h"
#include "error.h"
using namespace std;

class Lexer
{
private:
    char cha;     // 存放最新读入的字符
    string token; // 存放已读入的字符序列
    ifstream fin;
    ofstream fdyd;
    ofstream fdys;     // 实验要求，再在dys中输出dyd
    int line;          // 存储当前行号
    char retract_char; // 存储回退的字符

    unordered_map<string, int> reserve_map;
    Error *perror;
    void getchar(); // 读入字符，从源程序文件读入字符到cha
    void getnb();   // 删除空白字符。若cha是空白，则一直调用getchar()
    void concat();  // 连接字符串，将cha中的字符连接到token数组末尾
    bool letter();  // 判断字符是否为字母，若cha为字母，返回true
    bool digit();   // 判断字符是否为数字

    // 将刚读入cha中的字符回退到输入字符串中
    // 并将cha的值设置为空白
    void retract();

    // 处理保留字
    // 对存放在token中的字符串查保留字表
    // 查到，返回保留字的类别编码；否则返回0
    // 假设0不是任何单词符号的类别编码
    int reserve();

    // // 对token中的字符串查符号表
    // // 如果查到，返回位置编号；否则将其存放入符号表，并返回其位置编号
    // int buildlist();

    // // 将token中的数字串（字符串）转换为二进制值，查常数表
    // // 已存在：返回编号
    // // 不存在：存放入常数表，返回位置编号
    // int dtb();

    // 处理出现的词法错误
    // 非法字符、不正确常量...
    Token error(int);

    char type(char);

    void createReserveMap();

    Token case_letter();
    Token case_number();
    Token case_lt();
    Token case_gt();
    Token case_assign();
    void dump(ofstream &out, const Token &ws);

public:
    // 分析一个token
    Token analyzeWord();
    // 分析的同时打印token（用于语法分析）
    Token analyzeAndDumpWord();
    // 单独进行完整的词法分析
    void LexicalAnalyze();
    Lexer(const string &in_filepath);
    ~Lexer();
};
