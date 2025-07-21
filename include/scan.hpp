#pragma once

#include "parse.hpp"
#include "types.hpp"
#include <cstddef>
#include <expected>
#include <functional>

namespace stdx {

template <typename... Ts>
std::expected<details::scan_result<Ts...>, details::scan_error> scan(std::string_view input, std::string_view format) {
    auto parse_value = details::parse_sources(input, format);

    if(!parse_value.has_value()) {
        return std::unexpected(parse_value.error());
    }
    
    const auto& parse_format_vec = parse_value.value().first;
    const auto& parse_input_vec = parse_value.value().second;
    size_t numb_args = parse_value.value().first.size();

    if(numb_args != parse_value.value().second.size()) {
        return std::unexpected(details::scan_error{"Dumb implementation"});
    }

    auto input_begin = parse_input_vec.begin();
    auto frmt_begin = parse_format_vec.begin();
    
    details::scan_result res(details::parse_value_with_format<Ts>()...);

    //details::scan_result scan =  std::invoke([&]() {
        //return details::scan_result(details::parse_value_with_format<Ts>(*input_begin++, *frmt_begin++)...);
    //});

    return std::unexpected(details::scan_error{"Dumb implementation"});
}

} // namespace stdx
