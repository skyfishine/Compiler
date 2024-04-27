#pragma once
#include <fstream>
#include <string>
using namespace std;

class Error
{
private:
    static Error* error;
    ofstream ferror;
    Error(){ }

public:
    static Error *getInstance();
    void initError(const string &ferror_path);
    void printError(int line, string message);

    Error(Error const &) = delete;
    Error &operator=(Error const &) = delete;
    ~Error() = default;
};
// 全局的Error指针
extern Error *error;
