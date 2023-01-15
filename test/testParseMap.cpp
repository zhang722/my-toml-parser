#include <string>
#include <iostream>
#include <fstream>
#include <exception>
#include <unordered_map>

#include "testParseArray.cpp"

std::shared_ptr<Node>
parseMap(std::istream& file)
{
    expect('{');
    Table t;
    // start
    nextBlock(file);
    while (!notEndAndCurCharIs('}'))
    {
        using std::pair;
        using std::string;
        std::string key = parseWord(file);

        expect('=');
        nextBlock(file);
        std::shared_ptr<Node> value;

        if (cur() == '{') {
            value = parseMap(file);
            t.insert(key, value);
        } else if (cur() == '[') {
            value = parseArray(file);
            t.insert(key, value);
        } else if (cur() == '"') {
            value = parseBasicString(file);
            t.insert(key, value);
        } else if (cur() == '\'') {
            value = parseLiteralString(file);
            t.insert(key, value);
        } else {
            std::string str = parseWord(file);
            value = parseOthers(str);
            t.insert(key, value);
        }

        
        if (cur() == ',') {
            nextBlock(file);
        }
    }

    expect('}');
    nextBlock(file);

    return std::make_shared<Table>(t);
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
//         if (cur() == '{') {
//             std::string a;
//             a = parseMap(file);
//             std::cout << a;
//         }
//         parseWord(file);
//     }

// }
