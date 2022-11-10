#include <string>
#include <fstream>
#include <iostream>

#include "test.h"


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
