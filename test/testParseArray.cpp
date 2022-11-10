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
        while (ch != '\n' && !file.eof())
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

bool wordEnd()
{
    char ch = cur();
    if (!(ch < '9' && ch > '0' ||
          ch < 'z' && ch > 'a' ||
          ch < 'Z' && ch > 'A' ||
          ch == '_')) {
        return true;
    }

    return false;
}

std::string parseWord(std::istream& file)
{
    std::string word;
    while (!wordEnd() && file.eof()) {
        word += cur();
        nextChar(file);
    }
}

int main()
{
    std::stringstream file;
    file << "123  456\n ";
    // nextChar(file);
    // std::cout << file.get() << std::endl;
    nextBlock(file);
    std::string word = parseWord(file);
    std::cout << word << std::endl;
    std::cout << cur_ch_ << std::endl;
}
