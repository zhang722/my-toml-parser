#include <string>
#include <iostream>
#include <fstream>
#include <exception>
#include <map>

#include "testParseArray.cpp"

std::string parseMap(std::istream& file)
{
    expect('{');
    std::map<std::string, std::string> m;
    std::string result;
    // start
    nextBlock(file);
    while (!notEndAndCurCharIs('}'))
    {
        using std::pair;
        using std::string;
        std::string key = parseWord(file);

        expect('=');
        nextBlock(file);
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

        m.insert(pair<string, string>(key, value));
        
        if (cur() == ',') {
            nextBlock(file);
        }
    }

    expect('}');
    nextBlock(file);

    result += "{";
    for (auto& e : m) {
        result += e.first;
        result += "=";
        result += e.second;
        result += ",";
    }
    // TODO: use string_view to acclerate it.
    result = result.substr(0, result.length() - 1);
    result += "}";

    return result;
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
