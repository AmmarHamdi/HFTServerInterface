/**
 * @file test_CalculationService.cpp
 * @brief Unit test scaffold for ICalculationService.
 *
 * Uses GMock to generate a mock of ICalculationService and verifies that
 * calculate() is callable and returns the expected response.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "services/ICalculationService.hpp"
#include "models/Request.hpp"
#include "models/Response.hpp"

using ::testing::Return;
using ::testing::_;

// ---------------------------------------------------------------------------
// GMock class
// ---------------------------------------------------------------------------

class MockCalculationService : public ICalculationService
{
public:
    MOCK_METHOD(Response, calculate, (const Request&), (override));
};

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

TEST(CalculationServiceTest, CalculateReturnsSuccess)
{
    MockCalculationService mock;
    EXPECT_CALL(mock, calculate(_))
        .WillOnce(Return(Response{true, "calculated", {}}));

    Request req;
    req.type = RequestType::CALCULATE;
    auto response = mock.calculate(req);

    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.message, "calculated");
}

TEST(CalculationServiceTest, CalculateReturnsFailureOnError)
{
    MockCalculationService mock;
    EXPECT_CALL(mock, calculate(_))
        .WillOnce(Return(Response{false, "error", {}}));

    Request req;
    req.type = RequestType::CALCULATE;
    auto response = mock.calculate(req);

    EXPECT_FALSE(response.success);
    EXPECT_EQ(response.message, "error");
}
