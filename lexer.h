#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
using namespace std;

struct WordStruct
{
    int type;
    string original_value;
};

class Lexer
{
private:
    char cha;     // 存放最新读入的字符
    string token; // 存放已读入的字符序列
    ifstream fin;
    ofstream fout, ferror;
    int line;          // 存储当前行号
    char retract_char; // 存储回退的字符

    unordered_map<string, int> reserve_map;
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

    // 处理出现的词法错误
    // 非法字符、不正确常量...
    WordStruct error(int);

    char type(char);

    void createReserveMap();

    WordStruct case_letter();
    WordStruct case_number();
    WordStruct case_lt();
    WordStruct case_gt();
    WordStruct case_assign();

    WordStruct analyzeWord();

    void dump(WordStruct ws);

public:
    Lexer(string in_filepath, string out_filepath, string error_path);
    Lexer(string in_filapath);
    ~Lexer();
    void LexicalAnalyze();
};
