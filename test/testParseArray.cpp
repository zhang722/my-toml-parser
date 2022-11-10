#include <string>
#include <iostream>
#include <fstream>
#include <exception>
#include <vector>

#include "testParseString.cpp"

std::string parseArray(std::istream& file)
{
    expect('[');
    std::vector<std::string> array;
    std::string result;
    // start
    nextBlock(file);
    while (!notEndAndCurCharIs(']'))
    {
        if (cur() == '[') {
            array.push_back(parseArray(file));
        } else if (cur() == '"') {
            array.push_back(parseBasicString(file));
        } else if (cur() == '\'') {
            array.push_back(parseLiteralString(file));
        } else {
            array.push_back(parseWord(file));
        }
        
        if (cur() == ',') {
            nextBlock(file);
        }
    }

    expect(']');

    result += "[";
    for (size_t i = 0; i < array.size(); ++i) {
        result += array[i];
        if (i + 1 < array.size()) {
            result += ",";
        }
    }
    result += "]";

    return result;
}


int main()
{
    std::ifstream file("../example.toml");

    if (file.fail()) {
        std::cout << "open file failed" << std::endl;
        return -1;
    }

    while (cur() != EOF)
    {
        nextBlock(file);
        if (cur() == '[') {
            std::string a;
            a = parseArray(file);
            std::cout << a;
        }
        parseWord(file);
    }

}
