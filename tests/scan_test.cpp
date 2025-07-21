#include <cstdint>
#include <gtest/gtest.h>
#include <print>

#include "scan.hpp"

TEST(ScanTest, SimpleTest) {
    {
        auto result = stdx::scan<int16_t, int8_t, int32_t>("1234 5 1000", "{%d} {%d} {%d}");
        ASSERT_FALSE(result);
    }

    {
        auto result = stdx::scan<std::string, int8_t>("number 5", "{%s} {%d}");
        ASSERT_FALSE(result);
    }

    {
        auto result = stdx::scan<int8_t>("1234", "{%d}");
        ASSERT_FALSE(result);
    }

    {
        auto result = stdx::scan<int8_t>("-128", "{%d}");
        ASSERT_FALSE(result);
    }
}