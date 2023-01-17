#ifndef __PARSER_H__
#define __PARSER_H__

#pragma once

#include <string>
#include <fstream>
#include <memory>

#include "toml_types.h"

namespace toml {

class Parser
{
public:
    enum class IntType
    {
        NotInt = 0,
        Int    = 10,         // 43, -17
        IntHex = 16,         // 0xdeadbeef
        IntOct = 8,          // 0o12345607
        IntBin = 2           // 0b01010101
    };
    enum class DatetimeType
    {
        NotDatetime = 0,
        Date,
        Time,
        DateTime
    };
private:
    char cur_ch_ = 'c';
    size_t cur_line_ = 1;
    std::ifstream file_;
public:
    Parser() = default;
    Parser(const std::string& file);
    ~Parser() = default;

    Doc parse();

    char cur();
    // just skip to next char
    void basicNextChar();
    // skip comments
    void nextChar();

    void nextBlock();

    bool isWordChar(char ch);

    std::string parseWord();

    void expect(char ch);

    bool notEndAndCurCharIs(char ch);

    std::string parseGroup();

    std::string parseKey();

    std::string parseAssignment();
 
    Node::Ptr parseValue();

    Node::Ptr parseBasicString();

    Node::Ptr parseLiteralString();

    Node::Ptr parseMap();

    Node::Ptr parseArray();

    Node::Ptr parseOthers(const std::string& str);

    Node::Ptr parseInt(const std::string& str);

    Node::Ptr parseFloat(const std::string& str);

    Node::Ptr parseBoolean(const std::string& str);

    Node::Ptr parseDatetime(const std::string& str);

    IntType isInt(const std::string& str);

    bool isFloat(const std::string& str);

    bool isBoolean(const std::string& str);

    DatetimeType isDateTime(const std::string& str);
};

} // namespace toml

#endif // __PARSER_H__