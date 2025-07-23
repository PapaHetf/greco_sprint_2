#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <expected>
#include <string_view>
#include <utility>
#include <vector>

#include "types.hpp"

#include <limits>

namespace stdx::details {

template <typename T>
bool numeric_lim(int value) {
    return (std::numeric_limits<T>::min() <= value) && (value <= std::numeric_limits<T>::max());
}

template <typename T, T epsilon = static_cast<T>(0.00001)>
bool float_lim(float value) {
    return (std::abs(std::numeric_limits<T>::min() - value) > epsilon) && (std::abs(std::numeric_limits<T>::max() - value) > epsilon);
}

template <typename T, T epsilon = static_cast<T>(0.000000001)>
bool double_lim(double value) {
    return (std::abs(std::numeric_limits<T>::min() - value) > epsilon) && (std::abs(std::numeric_limits<T>::max() - value) > epsilon);
}

template <typename T>
concept isNumeric = requires {
    requires std::same_as<T, int8_t> || std::same_as<T, int16_t> || std::same_as<T, int32_t> || std::same_as<T, int64_t>;
};

template <typename T>
concept isNatural = requires {
    requires std::same_as<T, uint8_t> || std::same_as<T, uint16_t> || std::same_as<T, uint32_t> || std::same_as<T, uint64_t>;
};

template <typename T>
concept isFloat =std::same_as<T, float>;

template <typename T>
concept isDouble = std::same_as<T, double>;

template <typename T>
concept isString = std::same_as<T, std::string> || std::same_as<T, std::string_view>;

template <isNumeric T>
std::expected<T, scan_error> parse_value(std::string_view input, std::string_view fmt) {
    if(fmt == "%d" || fmt == "") {
        auto res = std::stoi(input.data());
       
        if(numeric_lim<T>(res)) {
            return T(res);
        }

        return std::unexpected(scan_error{"value " +  std::to_string(res) + " is overflow for save type"});
    }

    return std::unexpected(scan_error{"converison specifiers not support: " + std::string(fmt) + " for type numeric"});
}

template <isNatural T>
std::expected<T, scan_error> parse_value(std::string_view input, std::string_view fmt) {
    if(fmt == "%u" || fmt == "") {
        auto res = std::stoll(input.data());
       
        if(numeric_lim<T>(res)) {
            return T(res);
        }

        return std::unexpected(scan_error{"value " +  std::to_string(res) + " is overflow for save type"});
    }

    return std::unexpected(scan_error{"converison specifiers not support: " + std::string(fmt) + " for type natural numeric"});
}

template <isFloat T>
std::expected<T, scan_error> parse_value(std::string_view input, std::string_view fmt) {
    if(fmt == "%f" || fmt == "") {
        auto res = std::stof(input.data());
       
        if(float_lim<T>(res)) {
            return T(res);
        }

        return std::unexpected(scan_error{"value " +  std::to_string(res) + " is overflow for save type"});
    }

    return std::unexpected(scan_error{"converison specifiers not support: " + std::string(fmt) + " for type float"});
}

template <isDouble T>
std::expected<T, scan_error> parse_value(std::string_view input, std::string_view fmt) {
    if(fmt == "%f" || fmt == "") {
        auto res = std::stod(input.data());
       
        if(double_lim<T>(res)) {
            return T(res);
        }

        return std::unexpected(scan_error{"value " +  std::to_string(res) + " is overflow for save type"});
    }

    return std::unexpected(scan_error{"converison specifiers not support: " + std::string(fmt) + " for type double"});
}

template <isString T>
std::expected<T, scan_error> parse_value(std::string_view input, std::string_view fmt) {
    if(fmt == "%s" || fmt == "") {
        auto pos = input.find_last_not_of(' ');
        auto res = input.substr(0, pos + 1);
        return T(res);
    }

    return std::unexpected(scan_error{"converison specifiers not support: " + std::string(fmt) + " for type std::string or std::string_view"});
}

// Функция для парсинга значения с учетом спецификатора формата
template <typename T>
std::expected<T, scan_error> parse_value_with_format(std::string_view input, std::string_view fmt) {
    if constexpr (std::is_const_v<T>) {
        return parse_value<std::remove_cv_t<T>>(input, fmt);
    }
    else {
        return parse_value<T>(input, fmt);
    }
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