#ifndef __TOML_TYPES__
#define __TOML_TYPES__

#include <map>
#include <unordered_map>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <memory>
#include <type_traits>
#include <initializer_list>
#include <exception>

namespace toml {

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

struct Date
{
    using Year  = int;
    using Month = int;
    using Day   = int;

    Year year_;
    Month month_;
    Day day_;

    Date() = default;
    Date(Year year, Month month, Day day) : year_(year), month_(month), day_(day) {}
    virtual ~Date() = default;
    
    std::string asString()
    {
        std::stringstream ss;
        ss << year_ << '-' << month_ << '-' << day_;
        return ss.str(); 
    }
};
struct Time
{
    using Hour   = int;
    using Minute = int;
    using Second = double;

    Hour hour_;
    Minute minute_;
    Second second_;

    Time() = default;
    Time(Hour hour, Minute minute, Second second) : hour_(hour), minute_(minute), second_(second) {}
    virtual ~Time() = default;

    std::string asString() 
    {
        std::stringstream ss;
        ss << hour_ << ':' << minute_ << ':' << second_;
        return ss.str(); 
    }
};

struct Datetime : public Date, public Time
{
    Datetime() = default;

    Datetime(Year year, Month month, Day day)
    : Date(year, month, year), Time(0, 0, 0) {}

    Datetime(Hour hour, Minute minute, Second second)
    : Date(0, 0, 0), Time(hour, minute, second) {}

    Datetime(Year year, Month month, Day day, Hour hour, Minute minute, Second second)
    : Date(year, month, day), Time(hour, minute, second) {}

    Datetime(Date date, Time time) 
    : Date(date.year_, date.month_, date.year_), Time(time.hour_, time.minute_, time.second_) {}

    std::string asString()
    {
        return Date::asString() + "T" + Time::asString();
    }
};

class Error;

class Node
{
public:
    using Ptr = std::shared_ptr<Node>;
protected:
    enum NodeType 
    {
        SCALAR = 0,
        ARRAY,
        TABLE
    };
    NodeType node_type_;
public:
    Node() = default;
    Node(const NodeType& node_type) : node_type_(node_type) {}
    virtual ~Node() = default;
    virtual std::string asString() {return "Node";}
};

template <typename T>
class Scalar : public Node
{
protected:
    enum ScalarType 
    {
        INT = 0,
        FLOAT,
        BOOLEAN,
        STRING,
        DATATIME
    }; 
    ScalarType scalar_type_;

    T data_;
public:
    explicit Scalar(const T& data) : Node(NodeType::SCALAR), data_(data) 
    {
        if constexpr (std::is_same<T, bool>::value) {
            scalar_type_ = ScalarType::BOOLEAN;
        } else if constexpr (std::is_integral<T>::value) {
            scalar_type_ = ScalarType::INT;
        } else if constexpr (std::is_floating_point<T>::value) {
            scalar_type_ = ScalarType::FLOAT;
        } else if constexpr (std::is_same<T, std::string>::value) {
            scalar_type_ = ScalarType::STRING;
        } else if constexpr (std::is_same<T, Datetime>::value) {
            scalar_type_ = ScalarType::DATATIME;
        }
    }

    Scalar(std::initializer_list<T> l) : Node(NodeType::SCALAR)
    {
       
        data_ = *l.begin();
        
        if constexpr (std::is_same<T, bool>::value) {
            scalar_type_ = ScalarType::BOOLEAN;
        } else if constexpr (std::is_integral<T>::value) {
            scalar_type_ = ScalarType::INT;
        } else if constexpr (std::is_floating_point<T>::value) {
            scalar_type_ = ScalarType::FLOAT;
        } else if constexpr (std::is_same<T, std::string>::value) {
            scalar_type_ = ScalarType::STRING;
        } else if constexpr (std::is_same<T, Datetime>::value) {
            scalar_type_ = ScalarType::DATATIME;
        }
    }

    Scalar& operator=(const Scalar& other) 
    {
        scalar_type_ = other.scalar_type_;
        node_type_ = other.node_type_;
        data_ = other.data_;
        return *this;
    }

    Scalar(const Scalar& other) : Node(NodeType::SCALAR)
    {
        scalar_type_ = other.scalar_type_;
        data_ = other.data_;
    }

    Scalar& operator=(Scalar&& ) = delete;

    ~Scalar() {}

    template <typename T1>
    T as()
    {
        if (std::is_same<T, T1>::value) {
            return data_;
        } else {
            throw Error("[type error]");
        }
    }


    virtual std::string asString() 
    {
        if constexpr (std::is_same<T, Datetime>::value) {
            return data_.asString();
        } else {
            std::stringstream ss;
            ss << std::boolalpha << data_;
            return ss.str(); 
        }
    }
};


class Array : public Node
{
protected:
    std::vector<std::shared_ptr<Node>> data_;
public:
    Array() = default;
    Array(std::vector<std::shared_ptr<Node>>& data)     
    {
        node_type_ = NodeType::ARRAY;
        data_ =  data;
    }
    Array(std::initializer_list<std::shared_ptr<Node>> l)
    {
        node_type_ = NodeType::ARRAY;
        for (auto& i : l) {
            data_.push_back(i);
        }
    }

    ~Array() = default;

    void push_back(std::shared_ptr<Node> value)
    {
        data_.push_back(value);
    }

    template <typename T>
    std::shared_ptr<T> at(size_t idx)
    {
        auto it = data_.at(idx);
        std::shared_ptr<T> tmp = std::dynamic_pointer_cast<T>(it);
        if (!tmp) {
            throw Error("[type error]");
        }
        return tmp;
    }

    virtual std::string asString() 
    {
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < data_.size(); ++i) {
            ss << (*data_[i]).asString();
            if (i + 1 < data_.size()) {
                ss << ",";
            }
        }
        ss << "]";
        return ss.str();
    }
};

class Table : public Node
{
protected:
    std::unordered_map<std::string, std::shared_ptr<Node>> data_;
public:
    Table() = default;
    Table(std::unordered_map<std::string, std::shared_ptr<Node>>& data)     
    {
        node_type_ = NodeType::TABLE;
        data_ =  data;
    }
    Table(std::initializer_list<std::pair<std::string, std::shared_ptr<Node>>> l)
    {
        node_type_ = NodeType::TABLE;
        for (auto& i : l) {
            data_.insert(i);
        }
    }

    ~Table() = default;

    void insert(std::string key, std::shared_ptr<Node> value)
    {
        data_.insert({key, value});
    }

    void insert(std::pair<std::string, std::shared_ptr<Node>> p)
    {
        data_.insert(p);
    }
    
    template <typename T>
    std::shared_ptr<T> get(const std::string& key)
    {
        auto it = data_.find(key);
        if (it == data_.end()) {
            throw Error("[cann't find key: " + key + "]");
        }
        std::shared_ptr<T> tmp = std::dynamic_pointer_cast<T>(it->second);
        if (!tmp) {
            throw Error("[type error]");
        }
        return tmp;
    }

    virtual std::string asString()
    {
        std::stringstream ss;
        ss << "{";
        for (auto& i : data_) {
            ss << i.first << "=" << i.second->asString() << ",";
        }
        
        // TODO: use string_view to acclerate it.
        std::string result = ss.str();
        result = result.substr(0, result.length() - 1);
        result += "}";
        return result;
    }

};



class Doc
{
protected:
    std::map<std::string, std::shared_ptr<Node>, std::less<std::string>> data_;
public:
    Doc() = default;
    Doc(std::map<std::string, std::shared_ptr<Node>>& data)     
    {
        data_ =  data;
    }
    Doc(std::initializer_list<std::pair<std::string, std::shared_ptr<Node>>> l)
    {
        for (auto& i : l) {
            data_.insert(i);
        }
    }

    ~Doc() = default;

    void insert(const std::string& key, std::shared_ptr<Node> value)
    {
        data_.insert({key, value});
    }

    void insert(std::pair<std::string, std::shared_ptr<Node>> p)
    {
        data_.insert(p);
    }

    template <typename T>
    std::shared_ptr<T> get(const std::string& key)
    {
        auto it = data_.find(key);
        if (it == data_.end()) {
            throw Error("[cann't find key: " + key + "]");
        }
        std::shared_ptr<T> tmp = std::dynamic_pointer_cast<T>(it->second);
        if (!tmp) {
            throw Error("[type error]");
        }
        return tmp;
    }

    std::string asString()
    {
        std::stringstream ss;
        for (auto& i : data_) {
            ss << i.first << '=' << i.second->asString() << std::endl;
        }
        return ss.str();
    }
};

} // namespace toml

#endif
