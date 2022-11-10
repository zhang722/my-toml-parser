#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <regex>

#include "../include/error.h"

class Parser
{
public:
    char cur_ch_;
    std::ifstream file_;
    std::vector<Error> errors_;
public:
    Parser(std::string& filename);
    ~Parser();
    void nextChar();
    char currentChar();
    void skipCommentsAndSpaces();
    bool expectChar(char ch);
    std::string parseGroup();
    std::string parseKey();
    std::string parseValue();
    std::string parseScalar();
    std::string parseArray();
    std::string parseMap();
    std::string parseWord();
    std::string parseString();
    std::string parseBoolean();
    std::string parseNumber();
    bool parseAssignment();
    void parse();
};

Parser::Parser(std::string& filename)
{
    cur_ch_ = 'c';
    file_.open(filename);
}

Parser::~Parser()
{
    file_.close();
}

void Parser::nextChar()
{
    cur_ch_ = file_.get();
}

char Parser::currentChar()
{
    return cur_ch_;
}

void Parser::skipCommentsAndSpaces() 
{
    nextChar();
    while (currentChar() == ' ' || currentChar() == '#') {
        if (currentChar() == '#') {
            file_.ignore(1024, '\n');
        }
        nextChar();
    }
}

bool Parser::expectChar(char ch) 
{
    skipCommentsAndSpaces();
    return currentChar() == ch;
}

std::string Parser::parseGroup()
{
    // std::cout << "parse group{" << std::endl;
    std::string group;
    nextChar();
    while (currentChar() != ']') {
        group += currentChar();
        nextChar();
    }
    // std::cout << "}parse group" << std::endl;

    return group;
}

std::string Parser::parseKey()
{
    // std::cout << "parse key{" << std::endl;
    std::string key;
    while (currentChar() != ' ' && 
           currentChar() != '=' && 
           currentChar() != '\n' &&
           currentChar() != EOF) {
        key += currentChar();
        nextChar();
    }
    // std::cout << "}parse key" << std::endl;
    
    return key;
}

bool Parser::parseAssignment()
{
    if (expectChar('=')) {
        return true;
    }

    return false;
}

std::string Parser::parseValue()
{
    // skip spaces
    // while (currentChar() == ' ')
    // {
    //     nextChar();
    // }
    skipCommentsAndSpaces();

    if (currentChar() == '[') return parseArray();
    else if (currentChar() == '{') return parseMap();
    else if (currentChar() == '"' ||
            currentChar() == '\'' ||
            (currentChar() < '9' && currentChar() > '0') ||
            currentChar() == '+' ||
            currentChar() == '-' ||
            currentChar() == 'n' ||
            currentChar() == 'i' ||
            currentChar() == 't' ||
            currentChar() == 'f') return parseScalar();
    else {
        // Error
        nextChar();
        return {"%Error%"};
    }
}

std::string Parser::parseArray()
{
    // skip spaces
    // while (currentChar() == ' ')
    // {
    //     nextChar();
    // }
    skipCommentsAndSpaces();

    std::vector<std::string> elements;
    while (currentChar() != ']' && currentChar() != EOF) {
        std::string e = parseValue();
        elements.push_back(e);
        if (currentChar() == ',') {
            nextChar();
        }   
    }
    if (currentChar() == ']') {
        nextChar();
    }
    std::stringstream ss;
    ss << '[';
    for (auto& e : elements) {
        ss << e << ',';
    }
    ss << ']';
    
    return ss.str();
}

std::string Parser::parseMap()
{
    return "";
}

std::string Parser::parseString()
{
    std::string value;
    // process "" string
    if (currentChar() == '"') {
        nextChar();
        while (currentChar() != EOF && currentChar() != '"') {
            char ch = currentChar();
            if (ch == '\\') {
                nextChar();
                char ch2 = currentChar();
                if (ch2 == 't') ch = '\t';
                else if (ch2 == 'n') ch = '\n';
                else if (ch2 == 'r') ch = '\r';
                else if (ch2 == '"') ch = '"';
                else if (ch2 == '\\') ch = '\\';
                else if (ch2 == EOF) {
                    value += ch;
                    break;
                }
            }
            value += ch;
            nextChar();
            if (currentChar() == EOF) {
                // Error
                return {"%Error%"};
            }
        }
    } else if (currentChar() == '\'') { // process '' string
        nextChar();
        while (currentChar() != EOF && currentChar() != '\'') {
            value += currentChar();
            nextChar();
            if (currentChar() == EOF) {
                // Error
                return {"%Error%"};
            }
        }
    } else {
        // Error
        // Why are we here? You should check somewhere else.
        return {"%Error%"};
    }
    nextChar();
    return value;
}

std::string Parser::parseWord()
{
    std::string word;
    while (currentChar() != EOF && 
            currentChar() != '\n' && 
            currentChar() != ' ' &&
            currentChar() != ',' &&
            currentChar() != ']' &&
            currentChar() != '}') {
        word += currentChar();
        nextChar();
    }
    return word;
}

std::string Parser::parseBoolean()
{
    std::string word = parseWord();
    if (word == "true" || word == "false") {
        return word;
    } else {
        // Error
        return {"%Error%"};
    }
}

std::string Parser::parseNumber()
{
    // helper
    auto isInt = [this](std::string& str) {
        std::regex re("[+-]?\\d+");
        return std::regex_match(str, re);
    };

    // helper
    auto isFloat = [this, &isInt](std::string& str) {
        return !isInt(str);
    };

    std::string word = parseWord();
    if (isInt(word)) return "int";
    else if (isFloat(word)) return "float";
    else {
        // Error
        return {"%Error: not a number%"};
    }
}

std::string Parser::parseScalar()
{
    char ch = currentChar();
    if (ch == '"' || ch == '\'') return parseString();
    else if (ch == 't' || ch == 'f') return parseBoolean();
    else return parseNumber();
}

void Parser::parse()
{
    std::string group;
    while (currentChar() != EOF) {
        skipCommentsAndSpaces();
        // std::cout << "parse{" << std::endl;

        // start parse
        if (currentChar() == '[') {
            group = parseGroup() + '.';
            // std::cout << group << std::endl;
        } else if (currentChar() == '\n') {
            continue;
        } else {
            std::string key = group + parseKey();
            std::string value;
            if (parseAssignment()) {
                value = parseValue();
                // std::cout << "parse value" << std::endl;
            } else {
                std::cout << "missing =" << std::endl;
                // nextChar();
            }
            
            std::cout << "key:" << key << std::endl;
            std::cout << "value:" << value << std::endl;
        }
        // std::cout << "}parse" << std::endl;

        // end parse
    }
}

int main()
{
    std::string filename = "../example.toml";
    Parser parser(filename);
    parser.parse();
}