#include <string>
#include <iostream>
#include <fstream>
#include <exception>
#include <map>
#include <sstream>


#include "testParseMap.cpp"

std::string parseGroup(std::istream& file)
{
    expect('[');
    nextBlock(file);
    std::string group = parseWord(file);
    expect(']');

    nextBlock(file);
    return group;
}

std::string parseKey(std::istream& file)
{
    return parseWord(file);
}

std::string parseAssignment(std::istream& file)
{
    expect('=');
    nextBlock(file);

    return "=";
}

std::string parseValue(std::istream& file)
{
    std::string value;
    if (cur() == '{') {
        value = parseMap(file);
    } else if (cur() == '[') {
        value = parseArray(file);
    } else if (cur() == '"') {
        value = parseBasicString(file);
    } else if (cur() == '\'') {
        value = parseLiteralString(file);
    } else {
        value = parseWord(file);
    }

    return value;
}


int main()
{
    std::ifstream file("../example.toml");

    if (file.fail()) {
        std::cout << "open file failed" << std::endl;
        return -1;
    }
    std::stringstream ss;

    nextBlock(file);
    std::string group;
    while (cur() != EOF)
    {
        while (cur() == '[') {
            group = parseGroup(file) + ".";
            // std::cout << "group:" << group << std::endl;
        }
        std::string key = parseKey(file);
        // std::cout << "key:" << key << std::endl;
        ss << group;
        ss << key;
        std::string assign = parseAssignment(file);
        // std::cout << "assign:" << assign << std::endl;
        ss << assign;
        std::string value = parseValue(file);
        // std::cout << "value:" << value << std::endl;
        ss << value;
        ss << "\n";
    }
    std::cout << ss.str();
}
