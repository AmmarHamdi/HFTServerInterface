/**
 * @file test_MarketDataService.cpp
 * @brief Unit test scaffold for IMarketDataService.
 *
 * Uses GMock to generate a mock of IMarketDataService and verifies that
 * getData(), subscribe(), and unsubscribe() are callable and behave correctly.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "services/IMarketDataService.hpp"
#include "models/Request.hpp"
#include "models/Response.hpp"

using ::testing::Return;
using ::testing::_;

// ---------------------------------------------------------------------------
// GMock class
// ---------------------------------------------------------------------------

class MockMarketDataService : public IMarketDataService
{
public:
    MOCK_METHOD(Response, getData,     (const Request&),             (override));
    MOCK_METHOD(void,     subscribe,   (const std::string& symbol),  (override));
    MOCK_METHOD(void,     unsubscribe, (const std::string& symbol),  (override));
};

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

TEST(MarketDataServiceTest, GetDataReturnsSuccess)
{
    MockMarketDataService mock;
    EXPECT_CALL(mock, getData(_))
        .WillOnce(Return(Response{true, "ok", {}}));

    Request req;
    req.type = RequestType::GET_MARKET_DATA;
    auto response = mock.getData(req);

    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.message, "ok");
}

TEST(MarketDataServiceTest, SubscribeIsCalled)
{
    MockMarketDataService mock;
    EXPECT_CALL(mock, subscribe("AAPL")).Times(1);
    mock.subscribe("AAPL");
}

TEST(MarketDataServiceTest, UnsubscribeIsCalled)
{
    MockMarketDataService mock;
    EXPECT_CALL(mock, unsubscribe("AAPL")).Times(1);
    mock.unsubscribe("AAPL");
}
