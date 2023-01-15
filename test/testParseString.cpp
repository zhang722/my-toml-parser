#include <string>
#include <iostream>
#include <fstream>
#include <exception>

#include "test.h"

class Error : public std::exception
{
private:
    std::string msg_;
public:
    Error(std::string& str) : msg_(str) {}
    Error(std::string&& str) : msg_(std::move(str)) {}
    ~Error() = default;

    virtual const char* what() const throw() {return msg_.c_str();}
};


void expect(char ch)
{
    if (cur() != ch) {
        std::string msg = "[Line " + 
                        std::to_string(cur_line_) + 
                        "][Error: Missing " + 
                        std::string{ch} + "]";
        throw Error{msg};
    }
}

bool notEndAndCurCharIs(char ch)
{
    return (cur() == EOF || cur() == ch);
}

std::shared_ptr<Node>
parseBasicString(std::istream& file)
{
    expect('"');
   
    std::string str;
    
    // start
    basicNextChar(file);

    while (!notEndAndCurCharIs('"') && cur() != '\n')
    {
        char ch = cur();
        if (ch == '\\') {
            basicNextChar(file);
            char ch2 = cur();
            if (ch2 == 't') ch = '\t';
            else if (ch2 == 'n') ch = '\n';
            else if (ch2 == 'r') ch = '\r';
            else if (ch2 == '"') ch = '"';
            else if (ch2 == '\\') ch = '\\';
        }
        str += ch;
        basicNextChar(file);
    }

    expect('"');
    nextBlock(file);

    return std::make_shared<Scalar<std::string>>(str);
}

std::shared_ptr<Node>
parseLiteralString(std::istream& file)
{
    expect('\'');
    
    std::string str;

    // start
    basicNextChar(file);
    while (!notEndAndCurCharIs('\'') && cur() != '\n')
    {
        str += cur();
        basicNextChar(file);
    }

    expect('\'');
    nextBlock(file);

    return std::make_shared<Scalar<std::string>>(str);
}

// int main()
// {
//     std::ifstream file("../example.toml");

//     if (file.fail()) {
//         std::cout << "open file failed" << std::endl;
//         return -1;
//     }

//     // now we are at ", start of a the string "TOML Example"
//     while (!(notEndAndCurCharIs('"') || notEndAndCurCharIs('\'')))
//     {
//         nextBlock(file);
//         try
//         {
//             if (cur() == '"') {
//                 std::string str = parseBasicString(file);
//                 nextBlock(file);
//                 std::cout << str << std::endl;
//             }
//             else if (cur() == '\'') {
//                 std::string str = parseLiteralString(file);
//                 nextBlock(file);
//                 std::cout << str << std::endl;
//             }           
//         }
//         catch(const Error& e)
//         {
//             std::cout << e.what() << '\n';
//         }
        

//     }

// }