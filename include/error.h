#include <string>

class Error
{
protected:
    size_t line_;
    std::string msg_;
public:
    Error(size_t line) : line_(line), msg_("") {}
    Error(size_t line, std::string msg) : line_(line), msg_(msg) {}
    virtual ~Error() = default;

    virtual std::string toString();
};
