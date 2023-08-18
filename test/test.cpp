#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <vector>

int main(int, char**) {
    std::cout << "=== containers retrieved from lua tables ==="
              << std::endl;

    sol::state lua;
    lua.open_libraries();

    lua.set_function("is_vector_int", [](sol::object obj) {
        return obj.is<std::vector<int>>();
    });

    lua.set_function("is_vector_string", [](sol::object obj) {
        return obj.is<std::vector<std::string>>();
    });

    lua.set_function("maybe_optional_vector_int", [](sol::object obj) {
        return static_cast<bool>(obj.as<sol::optional<std::vector<int>>>());
    });

    lua.set_function("maybe_optional_vector_string", [](sol::object obj) {
        return static_cast<bool>(obj.as<sol::optional<std::vector<std::string>>>());
    });

    lua.set_function("vector_int", [](sol::object obj) {
        return obj.as<std::vector<int>>();
    });

    lua.set_function("vector_string", [](sol::object obj) {
        return obj.as<std::vector<std::string>>();
    });

    lua.script(R"(
print('{ "0", 1 } is vector of string? ' .. tostring(is_vector_string({ "0", 1 })))
print('{ "0", 1 } is vector of int? ' .. tostring(is_vector_int({ "0", 1 })))
print('{ "0" } may be vector of string? ' .. tostring(maybe_optional_vector_string({ "0" })))

print('{ "0", 1 } is vector of string? ' .. tostring(is_vector_string({ "0", 1 })))
print('{ "0", 1 } is vector of int? ' .. tostring(is_vector_int({ "0", 1 })))
print('{ 1 } may be vector of int? ' .. tostring(maybe_optional_vector_int({ 1 })))

print('{ "0", 1 } may be vector of int? ' .. tostring(maybe_optional_vector_int({ "0", 1 }))) -- crashes
print('{ "0", 1 } may be vector of string? ' .. tostring(maybe_optional_vector_string({ "0", 1 }))) -- crashes
print('{ "0", 1 } vector of int? ' .. tostring(vector_int({ "0", 1 }))) -- crashes
print('{ "0", 1 } vector of string? ' .. tostring(vector_string({ "0", 1 }))) -- crashes
    )");

    std::cout << std::endl;

    return 0;
}