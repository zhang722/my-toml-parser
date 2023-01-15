#ifndef __STD_REGEX_CPP__
#define __STD_REGEX_CPP__

#include <iostream>
#include <string>
#include <sstream>
#include <regex>
#include <limits>
#include <cmath>
#include <ctime>

#include "testStdAny.cpp"

using std::string;
using std::cout;
using std::endl;
enum class IntType
{
    NotInt = 0,
    Int    = 10,         // 43, -17
    IntHex = 16,         // 0xdeadbeef
    IntOct = 8,          // 0o12345607
    IntBin = 2           // 0b01010101
};
IntType isInt(const string& str)
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

std::shared_ptr<Node>
parseInt(const std::string& str)
{
    IntType type = isInt(str);
    std::string convert_str = str;
    if (type == IntType::IntOct || type == IntType::IntBin)
        convert_str = str.substr(2, str.length() - 2);

    int value = std::stoi(convert_str, nullptr, static_cast<int>(type));
    return std::make_shared<Scalar<int>>(value);
}

bool isFloat(const string& str)
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

std::shared_ptr<Node>
parseFloat(const string& str)
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

bool isBoolean(const std::string& str)
{
    if (str == "true" || str == "TRUE" ||
        str == "false" || str == "FALSE")
        return true;
    return false;
}

std::shared_ptr<Node>
parseBoolean(const std::string& str)
{
    bool value;

    if (str == "true" || str == "TRUE") {value = true;}
    else {value = false;}

    return std::make_shared<Scalar<bool>>(value);
}

enum class DatetimeType
{
    NotDatetime = 0,
    Date,
    Time,
    DateTime
};

DatetimeType isDateTime(const std::string& str)
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

std::shared_ptr<Node>
parseDatetime(const std::string& str)
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


std::shared_ptr<Node>
parseOthers(const std::string& str)
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



// int main()
// {
//     cout << parseInt("+99")->asString() << endl;
//     cout << parseInt("42")->asString() << endl;
//     cout << parseInt("0")->asString() << endl;
//     cout << parseInt("0x23")->asString() << endl;
//     cout << parseInt("0o23")->asString() << endl;
//     cout << parseInt("0b01")->asString() << endl;
//     cout << parseInt("-17")->asString() << endl;
//     cout << parseInt("-1.7")->asString() << endl;
//     cout << parseInt("17e10")->asString() << endl;
//     cout << "folat:" << endl;
//     cout << parseFloat("+1.0")->asString() << endl;
//     cout << parseFloat("3.124")->asString() << endl;
//     cout << parseFloat("5e22")->asString() << endl;
//     cout << parseFloat("-2E-2")->asString() << endl;
//     cout << parseFloat("5.6e-34")->asString() << endl;
//     cout << std::fixed <<parseFloat("224_617.334_454")->asString() << endl;
//     cout << parseFloat("nan")->asString() << endl;
//     cout << parseFloat("inf")->asString() << endl;
//     if (isBoolean("true"))
//         cout << parseBoolean("true")->asString() << endl;
//     if (isBoolean("false"))
//         cout << parseBoolean("false")->asString() << endl;
//     if (isBoolean("+true"))
//         cout << parseBoolean("+true")->asString() << endl;
//     if (isBoolean("TRUE"))
//         cout << parseBoolean("TRUE")->asString() << endl;

//     cout << "datetime" << endl;
//     cout << parseDatetime("2022-12-23")->asString() << endl;
//     cout << parseDatetime("22:33:22.343535")->asString() << endl;
//     cout << parseDatetime("2022-12-23T23:33:33")->asString() << endl;
//     cout << parseDatetime("2022-12-23z23:33:33")->asString() << endl;
// }

#endif
