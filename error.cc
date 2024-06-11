#include "error.h"
#include <iostream>

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
    if(!isError) isError = true;
    ferror << "***LINE:" << line << "\t";
    ferror << message << endl;
    std::cerr << "***LINE:" << line << "\t";
    std::cerr << message << std::endl;
}

Error::~Error()
{
    ferror.close();
}
