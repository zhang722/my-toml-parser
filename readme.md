## toml
Toml is a library parsing toml file.

## features
- light
- easy

## usage
Given a [TOML] file `example.toml` containting the following:

```toml
[database]
server = "192.168.1.1"
ports = [ 8001, 8001, 8002 ]
connection_max = 5000
enabled = true
table = {a = 1, b = 2, c = 3}

[servers]

  # You can indent as you please. Tabs or spaces. TOML don't care.
  [servers.alpha]
  ip = "10.0.0.1"
  dc = "eqdc10"

  [servers.beta]
  ip = "10.0.0.2"
  dc = "eqdc10"
```

Reading it with toml library:
```cpp
#include "toml.h"
toml::Parser parser;
auto doc = parser.parse("example.toml");

// log content
std::cout << doc.asString() << std::endl;

// get scalar using the key
auto a = doc.get<toml::Scalar<std::string>>("database.server");
std::string b = a->as<std::string>();

// get table
auto table = doc.get<toml::Table>("database.table");
std::cout << table->asString() << std::endl;

// get array
auto array = doc.get<toml::Array>("database.ports");
std::cout << array->asString() << std::endl;
```