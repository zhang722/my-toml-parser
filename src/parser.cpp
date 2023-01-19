#include <regex>

#include "parser.h"

namespace toml {

Parser::Parser(const std::string& file) 
{
    file_ = std::ifstream(file);
}

// Doc Parser::parse()
// {}
Doc Parser::parse()
{
    Doc doc;
    if (file_.fail()) {
        std::cout << "open file failed" << std::endl;
        throw Error("[error: open file failed]");
    }
    std::stringstream ss;

    nextBlock();
    std::string group;
    while (cur() != EOF)
    {
        if (cur() == '[') {
            group = parseGroup() + ".";
        }
        
        std::string key = parseKey();
        std::string assign = parseAssignment();
        std::shared_ptr<Node> value = parseValue();

        doc.insert({group + key, value});
    }

    return doc;
}

char Parser::cur()
{
    return cur_ch_;
}

void Parser::basicNextChar()
{
    cur_ch_ = file_.get();
}

void Parser::nextChar()
{
    if (cur() == '\n') {
        ++cur_line_;
    }
    char ch = file_.get();
    if (ch == '#') {
        ch = file_.get();
        while (ch != '\n' && ch != EOF)
        {
            ch = file_.get();
        }
    }
    cur_ch_ = ch;
}

void Parser::nextBlock()
{
    nextChar();
    while ((cur() == ' ' || cur() == '\n' || cur() == '\t') && cur() != EOF)
    {
        nextChar();
    }
}

bool Parser::isWordChar(char ch)
{
    if (ch <= '9' && ch >= '0' ||
        ch <= 'z' && ch >= 'a' ||
        ch <= 'Z' && ch >= 'A' ||
        ch == '_' ||
        ch == '-' ||
        ch == '.' ||
        ch == ':') {
        return true;
    }

    return false;
}

std::string Parser::parseWord()
{
    std::string word;
    while (cur() != EOF && isWordChar(cur())) {
        word += cur();
        nextChar();
    }
    if (cur() == ' ' || cur() == '\n') {
        nextBlock();
    }

    return word;
}

void Parser::expect(char ch)
{
    if (cur() != ch) {
        std::string msg = "[Line " + 
                        std::to_string(cur_line_) + 
                        "][Error: Missing " + 
                        std::string{ch} + "]";
        throw Error{msg};
    }
}

bool Parser::notEndAndCurCharIs(char ch)
{
    return (cur() == EOF || cur() == ch);
}

std::string Parser::parseGroup()
{
    expect('[');
    nextBlock();
    std::string group = parseWord();
    expect(']');

    nextBlock();
    return group;
}

std::string Parser::parseKey()
{
    return parseWord();
}

std::string Parser::parseAssignment()
{
    expect('=');
    nextBlock();

    return "=";
}

Node::Ptr Parser::parseValue()
{
    std::shared_ptr<Node> value;
    if (cur() == '{') {
        value = parseMap();
    } else if (cur() == '[') {
        value = parseArray();
    } else if (cur() == '"') {
        value = parseBasicString();
    } else if (cur() == '\'') {
        value = parseLiteralString();
    } else {
        std::string str = parseWord();
        value = parseOthers(str);
    }

    return value;
}

Node::Ptr Parser::parseBasicString()
{
    expect('"');
   
    std::string str;
    
    // start
    basicNextChar();

    while (!notEndAndCurCharIs('"') && cur() != '\n')
    {
        char ch = cur();
        if (ch == '\\') {
            basicNextChar();
            char ch2 = cur();
            if (ch2 == 't') ch = '\t';
            else if (ch2 == 'n') ch = '\n';
            else if (ch2 == 'r') ch = '\r';
            else if (ch2 == '"') ch = '"';
            else if (ch2 == '\\') ch = '\\';
        }
        str += ch;
        basicNextChar();
    }

    expect('"');
    nextBlock();

    return std::make_shared<Scalar<std::string>>(str);
}

Node::Ptr Parser::parseLiteralString()
{
    expect('\'');
    
    std::string str;

    // start
    basicNextChar();
    while (!notEndAndCurCharIs('\'') && cur() != '\n')
    {
        str += cur();
        basicNextChar();
    }

    expect('\'');
    nextBlock();

    return std::make_shared<Scalar<std::string>>(str);
}

Node::Ptr Parser::parseMap()
{
    expect('{');
    Table t;
    // start
    nextBlock();
    while (!notEndAndCurCharIs('}'))
    {
        using std::pair;
        using std::string;
        std::string key = parseWord();

        expect('=');
        nextBlock();
        std::shared_ptr<Node> value;

        if (cur() == '{') {
            value = parseMap();
            t.insert(key, value);
        } else if (cur() == '[') {
            value = parseArray();
            t.insert(key, value);
        } else if (cur() == '"') {
            value = parseBasicString();
            t.insert(key, value);
        } else if (cur() == '\'') {
            value = parseLiteralString();
            t.insert(key, value);
        } else {
            std::string str = parseWord();
            value = parseOthers(str);
            t.insert(key, value);
        }

        
        if (cur() == ',') {
            nextBlock();
        }
    }

    expect('}');
    nextBlock();

    return std::make_shared<Table>(t);
}

Node::Ptr Parser::parseArray()
{
    expect('[');
    std::string result;
    Array array;
    // start
    nextBlock();
    while (!notEndAndCurCharIs(']'))
    {
        std::shared_ptr<Node> value;
        if (cur() == '[') {
            value = parseArray(); 
            array.push_back(value);
        } else if (cur() == '"') {
            value = parseBasicString(); 
            array.push_back(value);
        } else if (cur() == '\'') {
            value = parseLiteralString(); 
            array.push_back(value);
        } else {
            std::string str = parseWord();
            value = parseOthers(str); 
            array.push_back(value);
        }
        
        if (cur() == ',') {
            nextBlock();
        }
    }

    expect(']');
    nextBlock();

    return std::make_shared<Array>(array);
}

Node::Ptr Parser::parseOthers(const std::string& str)
{
    std::shared_ptr<Node> value;
    if (isInt(str) != IntType::NotInt) {
        value = parseInt(str);
    } else if (isFloat(str)) {
        value = parseFloat(str);
    } else if (isDateTime(str) != DatetimeType::NotDatetime) {
        value = parseDatetime(str);
    } else if (isBoolean(str)) {
        value = parseBoolean(str);
    } else { // error type
        value = std::make_shared<Scalar<std::string>>("[error type]");
    }

    return value;
}

Parser::IntType Parser::isInt(const std::string& str)
{
    std::regex re_int("[+-]?\\d+");
    std::regex re_int_0x("0x[0-9A-Fa-f]?[_]?[0-9A-Fa-f]?");
    std::regex re_int_0o("0o[0-7]{1,8}");
    std::regex re_int_0b("0b[0-1]{1,32}");
    if (std::regex_match(str, re_int)) return IntType::Int;
    else if (std::regex_match(str, re_int_0x)) return IntType::IntHex;
    else if (std::regex_match(str, re_int_0o)) return IntType::IntOct;
    else if (std::regex_match(str, re_int_0b)) return IntType::IntBin;
    else return IntType::NotInt;
}

Node::Ptr Parser::parseInt(const std::string& str)
{
    IntType type = isInt(str);
    std::string convert_str = str;
    if (type == IntType::IntOct || type == IntType::IntBin)
        convert_str = str.substr(2, str.length() - 2);

    int value = std::stoi(convert_str, nullptr, static_cast<int>(type));
    return std::make_shared<Scalar<int>>(value);
}

bool Parser::isFloat(const std::string& str)
{
    std::stringstream ss(str);
    std::string remove_dilimiter_str;
    char a = ss.get();
    while (a != EOF) {
        if (a != '_') {
            remove_dilimiter_str += a;
        }
        a = ss.get();
    }
    std::istringstream iss(remove_dilimiter_str);
    double f;
    iss >> f;
    return iss && iss.eof();
}

Node::Ptr Parser::parseFloat(const std::string& str)
{
    std::stringstream ss(str);
    std::string remove_dilimiter_str;
    char a = ss.get();
    while (a != EOF) {
        if (a != '_') {
            remove_dilimiter_str += a;
        }
        a = ss.get();
    }

    double value = std::stod(remove_dilimiter_str);
    return std::make_shared<Scalar<double>>(value);
}

bool Parser::isBoolean(const std::string& str)
{
    if (str == "true" || str == "TRUE" ||
        str == "false" || str == "FALSE")
        return true;
    return false;
}

Node::Ptr Parser::parseBoolean(const std::string& str)
{
    bool value;

    if (str == "true" || str == "TRUE") {value = true;}
    else {value = false;}

    return std::make_shared<Scalar<bool>>(value);
}

Parser::DatetimeType Parser::isDateTime(const std::string& str)
{
    std::regex re_date("\\d{4}-\\d{2}-\\d{2}");
    std::regex re_time("\\d{2}:\\d{2}:\\d{2}\\.?\\d*");
    std::regex re_datetime("\\d{4}-\\d{2}-\\d{2}T\\d{2}:\\d{2}:\\d{2}\\.?\\d*Z?");
    if (std::regex_match(str, re_datetime))
        return DatetimeType::DateTime;
    else if (std::regex_match(str, re_date))
        return DatetimeType::Date;
    else if (std::regex_match(str, re_time))
        return DatetimeType::Time;
    else 
        return DatetimeType::NotDatetime;
}

Node::Ptr Parser::parseDatetime(const std::string& str)
{
    DatetimeType type = isDateTime(str);

    if (type == DatetimeType::Date) {
        Date::Year year = std::stoi(str.substr(0, 4));
        Date::Month month = std::stoi(str.substr(5, 2));
        Date::Day day = std::stoi(str.substr(8, 2));
        Datetime date(year, month, day);
        std::shared_ptr<Node> node = std::make_shared<Scalar<Datetime>>(date);
        return node;
    } else if (type == DatetimeType::Time) {
        Time::Hour hour = std::stoi(str.substr(0, 2));
        Time::Minute minute = std::stoi(str.substr(3, 2));
        Time::Second second = std::stod(str.substr(6));
        Datetime time(hour, minute, second);
        std::shared_ptr<Node> node = std::make_shared<Scalar<Datetime>>(time);
        return node;
    } else {
        Date::Year year = std::stoi(str.substr(0, 4));
        Date::Month month = std::stoi(str.substr(5, 2));
        Date::Day day = std::stoi(str.substr(8, 2));
        Time::Hour hour = std::stoi(str.substr(11, 2));
        Time::Minute minute = std::stoi(str.substr(14, 2));
        Time::Second second;
        if (str.back() == 'Z') {
            second = std::stod(str.substr(17, str.length() - 17 - 1));
        } else {
            second = std::stod(str.substr(17));
        }
        Datetime datetime(year, month, day, hour, minute, second);
        std::shared_ptr<Node> node = std::make_shared<Scalar<Datetime>>(datetime);
        return node;
    }
}

} // namespace toml