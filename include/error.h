#pragma once
#include <fstream>
#include <string>
using namespace std;

class ParserException
{
public:
    int line;
    string message;
    ParserException(int line, const string message) : line(line), message(message) {}
    string what()
    {
        return "***LINE: " + to_string(line) + +" " + message;
    }
};

class Error
{
private:
    static Error *error;
    ofstream ferror;
    bool isError; // 是否发生了错误
    Error() {}

public:
    static Error *getInstance();
    void initError(const string &ferror_path);
    void printError(int line, string message);
    bool getIsError() { return isError; }

    Error(Error const &) = delete;
    Error &operator=(Error const &) = delete;
    ~Error();
};
