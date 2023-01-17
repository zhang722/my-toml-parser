#include <iostream>

#include "toml.h"

int main()
{
    toml::Parser parser("../example.toml");
    auto doc = parser.parse();
    std::cout << doc.asString() << std::endl;
    return 0;
}