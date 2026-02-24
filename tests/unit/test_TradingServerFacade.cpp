/**
 * @file test_TradingServerFacade.cpp
 * @brief Unit tests for TradingServerFacade.
 *
 * Uses GMock to mock all four service interfaces and verifies that
 * handleRequest() dispatches to the correct service.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "services/IMarketDataService.hpp"
#include "services/ICalculationService.hpp"
#include "services/IManipulationService.hpp"
#include "services/IReportService.hpp"
#include "server/CommandRegistry.hpp"
#include "server/RequestTypes.hpp"
#include "models/Request.hpp"
#include "models/Response.hpp"
#include "models/ReportRequest.hpp"

// Pull in the concrete facade and commands.
#include "TradingServerFacade.hpp"
#include "commands/GetMarketDataCommand.hpp"
#include "commands/CalculationCommand.hpp"
#include "commands/ManipulationCommand.hpp"
#include "commands/ReportCommand.hpp"

using ::testing::Return;
using ::testing::_;

// ---------------------------------------------------------------------------
// GMock classes for all service interfaces
// ---------------------------------------------------------------------------

class MockMarketDataService : public IMarketDataService
{
public:
    MOCK_METHOD(Response, getData,      (const Request&),             (override));
    MOCK_METHOD(void,     subscribe,    (const std::string& symbol),  (override));
    MOCK_METHOD(void,     unsubscribe,  (const std::string& symbol),  (override));
};

class MockCalculationService : public ICalculationService
{
public:
    MOCK_METHOD(Response, calculate, (const Request&), (override));
};

class MockManipulationService : public IManipulationService
{
public:
    MOCK_METHOD(Response, manipulate, (const Request&), (override));
    MOCK_METHOD(Response, transform,  (const Request&), (override));
};

class MockReportService : public IReportService
{
public:
    MOCK_METHOD(Response, generateReport, (const ReportRequest&), (override));
};

// ---------------------------------------------------------------------------
// Helper: build a wired-up TradingServerFacade
// ---------------------------------------------------------------------------

static TradingServerFacade buildFacade(
    std::shared_ptr<MockMarketDataService>   mds,
    std::shared_ptr<MockCalculationService>  cs,
    std::shared_ptr<MockManipulationService> ms,
    std::shared_ptr<MockReportService>       rs)
{
    CommandRegistry registry;

    registry.registerCommand(RequestType::GET_MARKET_DATA,
        [mds](const Request& r) { return std::make_unique<GetMarketDataCommand>(mds, r); });

    registry.registerCommand(RequestType::CALCULATE,
        [cs](const Request& r) { return std::make_unique<CalculationCommand>(cs, r); });

    registry.registerCommand(RequestType::MANIPULATE,
        [ms](const Request& r) { return std::make_unique<ManipulationCommand>(ms, r); });

    registry.registerCommand(RequestType::GENERATE_REPORT,
        [rs](const Request& r) {
            ReportRequest rr{"EndOfDay", "2024-01-01", "2024-01-31"};
            return std::make_unique<ReportCommand>(rs, rr);
        });

    return TradingServerFacade(mds, cs, ms, rs, std::move(registry));
}

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

TEST(TradingServerFacadeTest, DispatchesGetMarketDataRequest)
{
    auto mds = std::make_shared<MockMarketDataService>();
    auto cs  = std::make_shared<MockCalculationService>();
    auto ms  = std::make_shared<MockManipulationService>();
    auto rs  = std::make_shared<MockReportService>();

    EXPECT_CALL(*mds, getData(_)).WillOnce(Return(Response{true, "market data", {}}));

    auto facade = buildFacade(mds, cs, ms, rs);
    Request req;
    req.type = RequestType::GET_MARKET_DATA;

    auto response = facade.handleRequest(req);
    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.message, "market data");
}

TEST(TradingServerFacadeTest, DispatchesCalculateRequest)
{
    auto mds = std::make_shared<MockMarketDataService>();
    auto cs  = std::make_shared<MockCalculationService>();
    auto ms  = std::make_shared<MockManipulationService>();
    auto rs  = std::make_shared<MockReportService>();

    EXPECT_CALL(*cs, calculate(_)).WillOnce(Return(Response{true, "calculated", {}}));

    auto facade = buildFacade(mds, cs, ms, rs);
    Request req;
    req.type = RequestType::CALCULATE;

    auto response = facade.handleRequest(req);
    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.message, "calculated");
}

TEST(TradingServerFacadeTest, DispatchesManipulateRequest)
{
    auto mds = std::make_shared<MockMarketDataService>();
    auto cs  = std::make_shared<MockCalculationService>();
    auto ms  = std::make_shared<MockManipulationService>();
    auto rs  = std::make_shared<MockReportService>();

    EXPECT_CALL(*ms, manipulate(_)).WillOnce(Return(Response{true, "manipulated", {}}));

    auto facade = buildFacade(mds, cs, ms, rs);
    Request req;
    req.type = RequestType::MANIPULATE;

    auto response = facade.handleRequest(req);
    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.message, "manipulated");
}

TEST(TradingServerFacadeTest, DispatchesGenerateReportRequest)
{
    auto mds = std::make_shared<MockMarketDataService>();
    auto cs  = std::make_shared<MockCalculationService>();
    auto ms  = std::make_shared<MockManipulationService>();
    auto rs  = std::make_shared<MockReportService>();

    EXPECT_CALL(*rs, generateReport(_)).WillOnce(Return(Response{true, "report", {}}));

    auto facade = buildFacade(mds, cs, ms, rs);
    Request req;
    req.type = RequestType::GENERATE_REPORT;

    auto response = facade.handleRequest(req);
    EXPECT_TRUE(response.success);
    EXPECT_EQ(response.message, "report");
}

TEST(TradingServerFacadeTest, ReturnsErrorResponseForUnknownRequestType)
{
    auto mds = std::make_shared<MockMarketDataService>();
    auto cs  = std::make_shared<MockCalculationService>();
    auto ms  = std::make_shared<MockManipulationService>();
    auto rs  = std::make_shared<MockReportService>();

    // Build a facade with an empty registry.
    CommandRegistry emptyRegistry;
    TradingServerFacade facade(mds, cs, ms, rs, std::move(emptyRegistry));

    Request req;
    req.type = RequestType::GET_MARKET_DATA;

    auto response = facade.handleRequest(req);
    EXPECT_FALSE(response.success);
}
