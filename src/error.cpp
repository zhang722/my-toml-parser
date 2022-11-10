#include "../include/error.h"

std::string Error::toString()
{
    std::string error;
    error += "line: ";
    error += line_;
    error += " : ";
    error += msg_;
    
    return error;
}
