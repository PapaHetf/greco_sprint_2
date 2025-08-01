#pragma once

#include <cstddef>
#include <string>
#include <expected>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>
#include <concepts>
#include <charconv>

#include "types.hpp"

namespace stdx::details {

template <typename T>
concept isString = std::is_convertible_v<T, std::string> || std::is_convertible_v<T, std::string_view>;

template <typename T>
requires std::integral<T> || std::floating_point<T>
std::expected<T, scan_error> parse_value(std::string_view input, std::string_view fmt) {
    if(fmt == "%d" || fmt == "%u" || fmt == "%f" ||fmt == "") {
        T res;
        
        auto [_, ec] = std::from_chars(input.data(), input.data() + input.size(), res);

        if(ec == std::errc{}) {
            return T(res);
        }
        else if (ec == std::errc::invalid_argument){
            return std::unexpected(scan_error{"value " +  std::to_string(res) + " is not a number"});
        } 
        else if (ec == std::errc::result_out_of_range) {
            return std::unexpected(scan_error{"value " +  std::to_string(res) + " is larger than an save type"});
        }
    }

    return std::unexpected(scan_error{"converison specifiers not support: " + std::string(fmt) + " for type numeric"});
}
/*
template <isString T>
std::expected<T, scan_error> parse_value(std::string_view input, std::string_view fmt) {
    if(fmt == "%s" || fmt == "") {
        auto pos = input.find_last_not_of(' ');
        auto res = input.substr(0, pos + 1);
        return T(res);
    }

    return std::unexpected(scan_error{"converison specifiers not support: " + std::string(fmt) + " for type std::string or std::string_view"});
}*/

// Функция для парсинга значения с учетом спецификатора формата
template <typename T>
std::expected<T, scan_error> parse_value_with_format(std::string_view input, std::string_view fmt) {
    //if constexpr (std::is_const_v<T>) {
        //return parse_value<std::remove_cv_t<T>>(input, fmt);
        return parse_value<T>(input, fmt);
    //}
    //else {
        //return parse_value<T>(input, fmt);
    //}
}

// Функция для проверки корректности входных данных и выделения из обеих строк интересующих данных для парсинга
template <typename... Ts>
std::expected<std::pair<std::vector<std::string_view>, std::vector<std::string_view>>, scan_error>
parse_sources(std::string_view input, std::string_view format) {
    std::vector<std::string_view> format_parts;  // Части формата между {}
    std::vector<std::string_view> input_parts;
    size_t start = 0;
    while (true) {
        size_t open = format.find('{', start);
        if (open == std::string_view::npos) {
            break;
        }
        size_t close = format.find('}', open);
        if (close == std::string_view::npos) {
            break;
        }

        // Если между предыдущей } и текущей { есть текст,
        // проверяем его наличие во входной строке
        if (open > start) {
            std::string_view between = format.substr(start, open - start);
            auto pos = input.find(between);
            if (input.size() < between.size() || pos == std::string_view::npos) {
                return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
            }
            if (start != 0) {
                input_parts.emplace_back(input.substr(0, pos));
            }

            input = input.substr(pos + between.size());
        }

        // Сохраняем спецификатор формата (то, что между {})
        format_parts.push_back(format.substr(open + 1, close - open - 1));
        start = close + 1;
    }

    // Проверяем оставшийся текст после последней }
    if (start < format.size()) {
        std::string_view remaining_format = format.substr(start);
        auto pos = input.find(remaining_format);
        if (input.size() < remaining_format.size() || pos == std::string_view::npos) {
            return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
        }
        input_parts.emplace_back(input.substr(0, pos));
        input = input.substr(pos + remaining_format.size());
    } else {
        input_parts.emplace_back(input);
    }
    return std::pair{format_parts, input_parts};
}

} // namespace stdx::details