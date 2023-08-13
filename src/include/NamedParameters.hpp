#pragma once

#include <luadef.hpp>

#include <map>
#include <string>

class NamedParameters : public std::map<std::string, sol::object> {
public:
	using Base = std::map<std::string, sol::object>;
	using Table = sol::as_table_t<Base>;

	NamedParameters() = default;

	NamedParameters(Table kwargs) : Base() {
		auto& data = *this;
		const auto& listmap = kwargs.value();
		for (const auto& kvp : listmap) {
			data[kvp.first] = kvp.second;
		}
	}
};

namespace sol {
	template <>
	struct is_container<NamedParameters> : std::false_type { };
} // namespace sol
