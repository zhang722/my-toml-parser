#include <iostream>
#include <string>

#include "toml.h"

int main()
{
    toml::Parser parser("../example.toml");
    auto doc = parser.parse();
    // std::cout << doc.asString() << std::endl;

    auto a = doc.get<toml::Scalar<std::string>>("database.server");
    std::string b = a->as<std::string>();
    std::cout << b << std::endl;
    return 0;
}