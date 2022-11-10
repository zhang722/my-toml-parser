#ifndef TEST_H_
#define TEST_H_

#include <string>
#include <fstream>
#include <iostream>

char cur_ch_ = 'c';
size_t cur_line_ = 1;

char cur()
{
    return cur_ch_;
}

// just skip to next char
void basicNextChar(std::istream& file)
{
    cur_ch_ = file.get();
}

// skip comments
void nextChar(std::istream& file)
{
    if (cur() == '\n') {
        ++cur_line_;
    }
    char ch = file.get();
    if (ch == '#') {
        ch = file.get();
        while (ch != '\n' && ch != EOF)
        {
            ch = file.get();
        }
    }
    cur_ch_ = ch;
}



void nextBlock(std::istream& file)
{
    nextChar(file);
    while ((cur() == ' ' || cur() == '\n') && cur() != EOF)
    {
        nextChar(file);
    }
}

bool isWordChar(char ch)
{
    if (ch <= '9' && ch >= '0' ||
        ch <= 'z' && ch >= 'a' ||
        ch <= 'Z' && ch >= 'A' ||
        ch == '_' ||
        ch == '-' ||
        ch == '.' ||
        ch == ':') {
        return true;
    }

    return false;
}

std::string parseWord(std::istream& file)
{
    std::string word;
    while (cur() != EOF && isWordChar(cur())) {
        word += cur();
        nextChar(file);
    }
    if (cur() == ' ' || cur() == '\n') {
        nextBlock(file);
    }

    return word;
}

#endif
