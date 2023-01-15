#include <string>
#include <iostream>
#include <fstream>
#include <exception>
#include <vector>

#include "testParseString.cpp"
#include "testRegex.cpp"


std::shared_ptr<Node>
parseArray(std::istream& file)
{
    expect('[');
    std::string result;
    Array array;
    // start
    nextBlock(file);
    while (!notEndAndCurCharIs(']'))
    {
        std::shared_ptr<Node> value;
        if (cur() == '[') {
            value = parseArray(file); 
            array.push_back(value);
        } else if (cur() == '"') {
            value = parseBasicString(file); 
            array.push_back(value);
        } else if (cur() == '\'') {
            value = parseLiteralString(file); 
            array.push_back(value);
        } else {
            std::string str = parseWord(file);
            value = parseOthers(str); 
            array.push_back(value);
        }
        
        if (cur() == ',') {
            nextBlock(file);
        }
    }

    expect(']');
    nextBlock(file);

    return std::make_shared<Array>(array);
}


// int main()
// {
//     std::ifstream file("../example.toml");

//     if (file.fail()) {
//         std::cout << "open file failed" << std::endl;
//         return -1;
//     }

//     while (cur() != EOF)
//     {
//         nextBlock(file);
//         if (cur() == '[') {
//             std::string a;
//             a = parseArray(file);
//             std::cout << a;
//         }
//         parseWord(file);
//     }

// }
