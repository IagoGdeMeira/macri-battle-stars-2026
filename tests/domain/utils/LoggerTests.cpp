#include "domain/utils/Logger/Logger.h"

#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <sstream>
#include <string>

class LoggerTestFixture
{
public:
    LoggerTestFixture()
    {
        this->originalBuffer = std::cout.rdbuf();
        std::cout.rdbuf(this->capture.rdbuf());

        Logger::setTimestampEnabled(false);
        Logger::setLevel(Logger::LogLevel::DEBUG);
    }

    ~LoggerTestFixture()
    {
        std::cout.rdbuf(this->originalBuffer);
        Logger::setTimestampEnabled(true);
        Logger::setLevel(Logger::LogLevel::DEBUG);
    }

    std::string getOutput() const { return this->capture.str(); }

private:
    std::stringstream capture;
    std::streambuf* originalBuffer;
};

TEST_CASE_METHOD(LoggerTestFixture, "Logger formats simple placeholders", "[unit][logger]")
{
    LOG_DEBUG("Hello, {}!", "World");
    auto output = this->getOutput();
    REQUIRE(output.find("[DEBUG] Hello, World!") != std::string::npos);
}

TEST_CASE_METHOD(LoggerTestFixture, "Logger handles multiple placeholders", "[unit][logger]")
{
    LOG_DEBUG("Coordinates: ({}, {})", 10, 20);
    auto output = this->getOutput();
    REQUIRE(output.find("[DEBUG] Coordinates: (10, 20)") != std::string::npos);
}

TEST_CASE_METHOD(LoggerTestFixture, "Logger applies float precision with {:.Nf}", "[unit][logger]")
{
    LOG_DEBUG("Pi = {:.2f}", 3.14159f);
    auto output = this->getOutput();
    REQUIRE(output.find("[DEBUG] Pi = 3.14") != std::string::npos);
}

TEST_CASE_METHOD(LoggerTestFixture, "Logger applies different precision {:.1f}", "[unit][logger]")
{
    LOG_DEBUG("Zoom: {:.1f}", 0.86f);
    auto output = this->getOutput();
    REQUIRE(output.find("[DEBUG] Zoom: 0.9") != std::string::npos);
}

TEST_CASE_METHOD(LoggerTestFixture, "Logger restores stream state after formatted argument", "[unit][logger]")
{
    LOG_DEBUG("A={:.2f} B={}", 1.234f, 5.678f);
    auto output = this->getOutput();
    REQUIRE(output.find("[DEBUG] A=1.23 B=5.678") != std::string::npos);
}

TEST_CASE_METHOD(LoggerTestFixture, "Logger ignores invalid format specifiers", "[unit][logger]")
{
    LOG_DEBUG("{:.3x}", 10);
    auto output = this->getOutput();
    REQUIRE(output.find("[DEBUG] 10") != std::string::npos);
}

TEST_CASE_METHOD(LoggerTestFixture, "Logger handles mix of formatted and unformatted arguments", "[unit][logger]")
{
    LOG_DEBUG("Player {} pos=({:.1f}, {:.1f})", 1, 100.26f, 200.75f);
    auto output = this->getOutput();
    REQUIRE(output.find("[DEBUG] Player 1 pos=(100.3, 200.8)") != std::string::npos);
}

TEST_CASE_METHOD(LoggerTestFixture, "Logger works with string arguments", "[unit][logger]")
{
    LOG_DEBUG("{} says {}", "Alice", "Hello");
    auto output = this->getOutput();
    REQUIRE(output.find("[DEBUG] Alice says Hello") != std::string::npos);
}

TEST_CASE_METHOD(LoggerTestFixture, "Logger does not break on missing placeholder", "[unit][logger]")
{
    LOG_DEBUG("Static message without placeholder");
    auto output = this->getOutput();
    REQUIRE(output.find("[DEBUG] Static message without placeholder") != std::string::npos);
}
