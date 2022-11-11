#include <map>
#include <unordered_map>
#include <any>
#include <string>
#include <iostream>
#include <variant>
#include <vector>
#include <initializer_list>
#include <type_traits>

struct Datetime {};
struct Node;

// using Scalar = std::variant<std::string, bool, int, float, Datetime>;
using Scalar = std::any;
using Map = std::unordered_map<Node, Node>;
using Sequence = std::vector<Node>;

struct Node
{
    std::variant<std::monostate, Scalar, Map, Sequence> value; 

    Node() = default;
    ~Node() = default;

    template <
        typename T,	 //
        typename = std::enable_if_t<!std::is_constructible_v<
            std::initializer_list<Map::value_type>, T>>,	//
        typename = std::enable_if_t<!std::is_constructible_v<
            std::initializer_list<Sequence::value_type>, T>>>
    Node(const T& scalar)
    {
        value.emplace<Scalar>().emplace<T>(scalar);
    }
    /** Specialization for literals */
    Node(const char* str)
    {
        // Storing char* is not safe if it points to temporary
        // memory or a stack zone (!):
        value.emplace<Scalar>().emplace<std::string>(str);
    }

    Node(std::initializer_list<Map::value_type> init)
    {
        value.emplace<Map>(init);
    }
    Node(std::initializer_list<Sequence::value_type> init)
    {
        value.emplace<Sequence>(init);
    }
};

static_assert(std::is_constructible<Node, Scalar>::value);
static_assert(std::is_constructible<Node, Map>::value);
static_assert(std::is_constructible<Node, Sequence>::value);

int main()
{
    // Sequence({Map{"a", "1"}, Sequence{"b", "2"}, Datetime{}, true, 32, 123.1f});
}