/**
 * @file test_CommandRegistry.cpp
 * @brief Unit tests for CommandRegistry.
 *
 * Tests: command registration, command creation, and unknown-type handling.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "server/CommandRegistry.hpp"
#include "server/ICommand.hpp"
#include "server/RequestTypes.hpp"
#include "models/Request.hpp"
#include "models/Response.hpp"

// ---------------------------------------------------------------------------
// Minimal stub command used by the tests
// ---------------------------------------------------------------------------

class StubCommand : public ICommand
{
public:
    explicit StubCommand(bool successValue) : m_success(successValue) {}
    Response execute() override { return Response{m_success, "stub", {}}; }
private:
    bool m_success;
};

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

TEST(CommandRegistryTest, RegisterAndCreateReturnsCorrectCommand)
{
    CommandRegistry registry;
    registry.registerCommand(RequestType::GET_MARKET_DATA,
        [](const Request&) {
            return std::make_unique<StubCommand>(true);
        });

    Request req;
    req.type = RequestType::GET_MARKET_DATA;

    auto cmd = registry.create(req);
    ASSERT_NE(cmd, nullptr);
    auto response = cmd->execute();
    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.message, "stub");
}

TEST(CommandRegistryTest, CreateForUnknownTypeThrows)
{
    CommandRegistry registry;
    // Register nothing — any request type should throw.

    Request req;
    req.type = RequestType::CALCULATE;

    EXPECT_THROW(registry.create(req), std::out_of_range);
}

TEST(CommandRegistryTest, OverwriteRegistrationUsesLatestFactory)
{
    CommandRegistry registry;

    registry.registerCommand(RequestType::CALCULATE,
        [](const Request&) { return std::make_unique<StubCommand>(false); });

    // Overwrite with a factory that returns success = true.
    registry.registerCommand(RequestType::CALCULATE,
        [](const Request&) { return std::make_unique<StubCommand>(true); });

    Request req;
    req.type = RequestType::CALCULATE;
    auto cmd = registry.create(req);
    EXPECT_TRUE(cmd->execute().success);
}

TEST(CommandRegistryTest, AllDefaultRequestTypesCanBeRegistered)
{
    CommandRegistry registry;

    auto factory = [](const Request&) { return std::make_unique<StubCommand>(true); };

    registry.registerCommand(RequestType::GET_MARKET_DATA, factory);
    registry.registerCommand(RequestType::CALCULATE,       factory);
    registry.registerCommand(RequestType::MANIPULATE,      factory);
    registry.registerCommand(RequestType::GENERATE_REPORT, factory);

    for (auto type : {RequestType::GET_MARKET_DATA,
                      RequestType::CALCULATE,
                      RequestType::MANIPULATE,
                      RequestType::GENERATE_REPORT})
    {
        Request req;
        req.type = type;
        EXPECT_NO_THROW(registry.create(req));
    }
}
