/**
 * @file test_ManipulationService.cpp
 * @brief Unit test scaffold for IManipulationService.
 *
 * Uses GMock to generate a mock of IManipulationService and verifies that
 * manipulate() and transform() are callable and return the expected responses.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "services/IManipulationService.hpp"
#include "models/Request.hpp"
#include "models/Response.hpp"

using ::testing::Return;
using ::testing::_;

// ---------------------------------------------------------------------------
// GMock class
// ---------------------------------------------------------------------------

class MockManipulationService : public IManipulationService
{
public:
    MOCK_METHOD(Response, manipulate, (const Request&), (override));
    MOCK_METHOD(Response, transform,  (const Request&), (override));
};

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

TEST(ManipulationServiceTest, ManipulateReturnsSuccess)
{
    MockManipulationService mock;
    EXPECT_CALL(mock, manipulate(_))
        .WillOnce(Return(Response{true, "manipulated", {}}));

    Request req;
    req.type = RequestType::MANIPULATE;
    auto response = mock.manipulate(req);

    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.message, "manipulated");
}

TEST(ManipulationServiceTest, TransformReturnsSuccess)
{
    MockManipulationService mock;
    EXPECT_CALL(mock, transform(_))
        .WillOnce(Return(Response{true, "transformed", {}}));

    Request req;
    req.type = RequestType::MANIPULATE;
    auto response = mock.transform(req);

    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.message, "transformed");
}
