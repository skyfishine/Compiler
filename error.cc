#include "error.h"

Error *Error::error = nullptr;

Error *Error::getInstance()
{
    if (error == nullptr)
    {
        error = new Error();
    }
    return error;
}
void Error::initError(const string &ferror_path)
{
    ferror.open(ferror_path);
}

void Error::printError(int line, string message)
{
    ferror << "第" << line << "行\t";
    ferror << message << endl;
}

Error::~Error()
{
    ferror.close();
}
