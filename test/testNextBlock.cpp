#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

char cur_ch_ = 'c';

void nextChar(std::istream& file)
{
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

char cur()
{
    return cur_ch_;
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
        ch == '.') {
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

    return word;
}

int main()
{
    std::fstream file("../example.toml");

    if (file.fail()) {
        std::cout << "open file failed" << std::endl;
    }

    while (cur() != EOF)
    {
        nextBlock(file);
        std::cout << "[current location]" << cur_ch_ << std::endl;
        std::string word = parseWord(file);
        std::cout << word << std::endl;
    }
}
