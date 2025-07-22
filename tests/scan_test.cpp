#include <cstdint>
#include <gtest/gtest.h>
#include <print>

#include "scan.hpp"

TEST(ScanTest, SimpleTestNumeric) {
    {
        auto result = stdx::scan<int16_t, int8_t, int16_t>("1234 5 1000", "{%d} {%d} {%d}");
        ASSERT_EQ(std::get<0>(result->values()), 1234);
        ASSERT_EQ(std::get<1>(result->values()), 5);
        ASSERT_EQ(std::get<2>(result->values()), 1000);
    }
}

TEST(ScanTest, SimpleTestNaturalNumeric) {
    {
        auto result = stdx::scan<uint8_t>("5", "{%u}");
        ASSERT_EQ(std::get<0>(result->values()), 5);
        std::get<0>(result->values()) = 10;
        ASSERT_EQ(std::get<0>(result->values()), 10);
    }
    {
        auto result = stdx::scan<uint8_t, uint16_t>("255 1024", "{%u} {%u}");
        ASSERT_EQ(std::get<0>(result->values()), 255);
        ASSERT_EQ(std::get<1>(result->values()), 1024);
    }
}

TEST(ScanTest, SimpleTestString) {
    {
        auto result = stdx::scan<std::string>("Hello", "{%s}");
        ASSERT_EQ(std::get<0>(result->values()), "Hello");
    }
    {
        auto result = stdx::scan<const std::string, std::string>("Hello Crowley!", "{%s} {%s}");
        ASSERT_EQ(std::get<0>(result->values()), "Hello");
        ASSERT_EQ(std::get<1>(result->values()), "Crowley!");
        //std::get<0>(result->values()) = "Hi";
        std::get<1>(result->values()) = "Mr.Crowley!";
        ASSERT_EQ(std::get<1>(result->values()), "Mr.Crowley!");
    }
}

TEST(ScanTest, SimpleTestFloat) {
    {
        auto result = stdx::scan<float>("5.05", "{%f}");
        ASSERT_TRUE(std::abs(std::get<0>(result->values()) - 5.05) < 0.00001);
    }
}

TEST(ScanTest, SimpleTestDouble) {
    {
        auto result = stdx::scan<double>("5.0005", "{%f}");
        ASSERT_TRUE(std::abs(std::get<0>(result->values()) - 5.0005) < 0.000000001);
    }
}

TEST(ScanTest, SimpleTestNumericConst) {
    {
        auto result = stdx::scan<const uint16_t>("8800", "{%u}");
        ASSERT_EQ(std::get<0>(result->values()), 8800);
        //std::get<0>(result->values()) = 10;
    }
    {
        auto result = stdx::scan<const uint16_t, int8_t>("8800 5", "{%u} {%d}");
        ASSERT_EQ(std::get<0>(result->values()), 8800);
        ASSERT_EQ(std::get<1>(result->values()), 5);
        std::get<1>(result->values()) = 10;
        ASSERT_EQ(std::get<1>(result->values()), 10);
    }
}

TEST(ScanTest, TestNumericError) {
    {
        auto result = stdx::scan<int8_t>("-129", "{%d}");
        ASSERT_FALSE(result.has_value());
        ASSERT_EQ(result.error().message_, "value -129 is overflow for save type");
    }
    {
        auto result = stdx::scan<int16_t, int8_t, int16_t>("1234 128 1000", "{%d} {%d} {%d}");
        ASSERT_FALSE(result.has_value());
        ASSERT_EQ(result.error().message_, "value 128 is overflow for save type");
    }
}

TEST(ScanTest, TestNaturalError) {
    {
        auto result = stdx::scan<uint8_t>("-10", "{%u}");
        ASSERT_FALSE(result.has_value());
        ASSERT_EQ(result.error().message_, "value -10 is overflow for save type");
    }
    {
        auto result = stdx::scan<uint16_t, uint32_t, uint8_t>("1234 128 1000", "{%u} {%u} {%u}");
        ASSERT_FALSE(result.has_value());
        ASSERT_EQ(result.error().message_, "value 1000 is overflow for save type");
    }
}

TEST(ScanTest, TestFloatError) {
    {
        auto result = stdx::scan<float>("0.000009", "{%f}");
        ASSERT_FALSE(result.has_value());
        ASSERT_EQ(result.error().message_, "value 0.000009 is overflow for save type");
    }
    {
        auto result = stdx::scan<float>("0.000011", "{%f}");
        ASSERT_TRUE(result.has_value());
        ASSERT_TRUE(std::abs(std::get<0>(result->values()) - 0.000011) < 0.00001);
    }
}

TEST(ScanTest, TestStringError) {
    {
        auto result = stdx::scan<std::string>("Hello", "{%d}");
        ASSERT_FALSE(result.has_value());
        ASSERT_EQ(result.error().message_, "converison specifiers not support: %d for type std::string or std::string_view");
    }
}

TEST(ScanTest, TestFormatString) {
    {
        auto result = stdx::scan<std::string, float>("I want to sum 42 and 3.14 numbers.", "I want to sum {} and {%f} numbers.");
        ASSERT_TRUE(result.has_value());
        ASSERT_EQ(std::get<0>(result->values()), "42");
        ASSERT_TRUE(std::abs(std::get<1>(result->values()) - 3.14) < 0.00001);
    }
    {
        auto result = stdx::scan<int8_t, float>("I want to sum 42 and 3.14 numbers.", "I want to sum {} and {%f} numbers.");
        ASSERT_TRUE(result.has_value());
        ASSERT_EQ(std::get<0>(result->values()), 42);
        ASSERT_TRUE(std::abs(std::get<1>(result->values()) - 3.14) < 0.00001);
    }
}