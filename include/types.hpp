#pragma once

#include <tuple>

namespace stdx::details {

// Класс для хранения ошибки неуспешного сканирования

struct scan_error {
    std::string message_;
};

// Шаблонный класс для хранения результатов успешного сканирования

template <typename... Ts>
struct scan_result {
    scan_result() = delete;

    scan_result(Ts &&...parameter): result_(std::forward<Ts>(parameter)...) {}
    
    void values();
    
    private:
        std::tuple<Ts...> result_;
};

} // namespace stdx::details
