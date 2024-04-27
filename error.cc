#include "error.h"

// 全局的指针
Error *error = Error::getInstance();

Error *Error::getInstance()
{
    if (error == nullptr)
    {
        error = new Error();
    }
    else
    {
        return error;
    }
}
void Error::initError(const string &ferror_path)
{
    ferror.open(ferror_path);
}

void Error::printError(int line, string message)
{
    ferror << "第" << line << "行";
    ferror << message << endl;
}
