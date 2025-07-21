#pragma once

#include "parse.hpp"
#include "types.hpp"
#include <cstddef>
#include <expected>
#include <functional>

namespace stdx {

template <typename... Ts, std::size_t... Indx>
std::expected<details::scan_result<Ts...>, details::scan_error>
scan_impl(const std::vector<std::string_view>& inputs, const std::vector<std::string_view>& fmts, std::index_sequence<Indx...>) {
    
    auto parsed_tuple = std::make_tuple(details::parse_value_with_format<Ts>(inputs[Indx], fmts[Indx])...);

    std::string scan_error;
    std::apply([&](auto const&... tuple_args) 
    {
        (std::invoke([&]() {
            if (!tuple_args.has_value()) {
                scan_error.append(tuple_args.error().message_);
            }
        }), ...);
    }, parsed_tuple);

    if(!scan_error.empty()) {
        return std::unexpected(details::scan_error{"Dumb implementation"});
    }

    return std::unexpected(details::scan_error{"Dumb implementation"});//std::expected(details::scan_result<Ts...>(std::move(parsed_tuple)));
}

template <typename... Ts>
std::expected<details::scan_result<Ts...>, details::scan_error> scan(std::string_view input, std::string_view format) {
    auto parse_value = details::parse_sources(input, format);

    if(!parse_value.has_value()) {
        return std::unexpected(parse_value.error());
    }
    
    const auto& parse_format_vec = parse_value.value().first;
    const auto& parse_input_vec = parse_value.value().second;
   
    if(parse_value.value().first.size() != parse_value.value().second.size() || parse_value.value().first.size() != sizeof...(Ts)) {
        return std::unexpected(details::scan_error{"Dumb implementation"});
    }

    return scan_impl<Ts...>(parse_input_vec, parse_format_vec, std::index_sequence_for<Ts...>{});
}

} // namespace stdx
