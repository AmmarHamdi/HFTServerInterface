/**
 * @file test_ServerBootstrap.cpp
 * @brief Unit tests for the stub service implementations and full server wiring.
 *
 * @details Verifies two things:
 *
 *   1. Each StubXxxService returns a well-formed "not yet implemented" Response
 *      without crashing, so the server binary runs safely before real service
 *      implementations are available.
 *
 *   2. The CommandRegistry + TradingServerFacade bootstrap mirrors what main()
 *      does: all four RequestType values are registered and the facade
 *      dispatches them end-to-end through the stub services without throwing.
 */

#include <gtest/gtest.h>

// Stub services under test
#include "StubServices.hpp"

// Server / command layer
#include "TradingServerFacade.hpp"
#include "server/CommandRegistry.hpp"
#include "server/RequestTypes.hpp"
#include "commands/CalculationCommand.hpp"
#include "commands/GetMarketDataCommand.hpp"
#include "commands/ManipulationCommand.hpp"
#include "commands/ReportCommand.hpp"

// Models
#include "models/ReportRequest.hpp"
#include "models/Request.hpp"
#include "models/Response.hpp"

// ==========================================================================
// StubMarketDataService
// ==========================================================================

TEST(StubMarketDataServiceTest, GetDataReturnsFalseWithMessage)
{
    StubMarketDataService svc;
    Request req;
    req.type = RequestType::GET_MARKET_DATA;
    Response r = svc.getData(req);
    EXPECT_FALSE(r.success);
    EXPECT_FALSE(r.message.empty());
    EXPECT_TRUE(r.data.empty());
}

TEST(StubMarketDataServiceTest, SubscribeDoesNotThrow)
{
    StubMarketDataService svc;
    EXPECT_NO_THROW(svc.subscribe("AAPL"));
}

TEST(StubMarketDataServiceTest, UnsubscribeDoesNotThrow)
{
    StubMarketDataService svc;
    EXPECT_NO_THROW(svc.unsubscribe("AAPL"));
}

// ==========================================================================
// StubCalculationService
// ==========================================================================

TEST(StubCalculationServiceTest, CalculateReturnsFalseWithMessage)
{
    StubCalculationService svc;
    Request req;
    req.type = RequestType::CALCULATE;
    Response r = svc.calculate(req);
    EXPECT_FALSE(r.success);
    EXPECT_FALSE(r.message.empty());
    EXPECT_TRUE(r.data.empty());
}

// ==========================================================================
// StubManipulationService
// ==========================================================================

TEST(StubManipulationServiceTest, ManipulateReturnsFalseWithMessage)
{
    StubManipulationService svc;
    Request req;
    req.type = RequestType::MANIPULATE;
    Response r = svc.manipulate(req);
    EXPECT_FALSE(r.success);
    EXPECT_FALSE(r.message.empty());
}

TEST(StubManipulationServiceTest, TransformReturnsFalseWithMessage)
{
    StubManipulationService svc;
    Request req;
    req.type = RequestType::MANIPULATE;
    Response r = svc.transform(req);
    EXPECT_FALSE(r.success);
    EXPECT_FALSE(r.message.empty());
}

// ==========================================================================
// StubReportService
// ==========================================================================

TEST(StubReportServiceTest, GenerateReportReturnsFalseAndIncludesReportType)
{
    StubReportService svc;
    ReportRequest req{"EndOfDay", "2026-01-01", "2026-01-31"};
    Response r = svc.generateReport(req);
    EXPECT_FALSE(r.success);
    EXPECT_NE(r.message.find("EndOfDay"), std::string::npos);
}

TEST(StubReportServiceTest, GenerateReportWithDifferentTypeIncludesTypeName)
{
    StubReportService svc;
    ReportRequest req{"Blotter", "2026-01-01", "2026-01-31"};
    Response r = svc.generateReport(req);
    EXPECT_FALSE(r.success);
    EXPECT_NE(r.message.find("Blotter"), std::string::npos);
}

// ==========================================================================
// Full bootstrap wiring (mirrors main())
// ==========================================================================

namespace
{
    /// Build a TradingServerFacade exactly as main() does, using all stubs.
    TradingServerFacade buildStubFacade()
    {
        auto mds = std::make_shared<StubMarketDataService>();
        auto cs  = std::make_shared<StubCalculationService>();
        auto ms  = std::make_shared<StubManipulationService>();
        auto rs  = std::make_shared<StubReportService>();

        CommandRegistry registry;

        registry.registerCommand(RequestType::GET_MARKET_DATA,
            [mds](const Request& r) {
                return std::make_unique<GetMarketDataCommand>(mds, r);
            });

        registry.registerCommand(RequestType::CALCULATE,
            [cs](const Request& r) {
                return std::make_unique<CalculationCommand>(cs, r);
            });

        registry.registerCommand(RequestType::MANIPULATE,
            [ms](const Request& r) {
                return std::make_unique<ManipulationCommand>(ms, r);
            });

        registry.registerCommand(RequestType::GENERATE_REPORT,
            [rs](const Request& r) {
                ReportRequest rr{"EndOfDay", "2026-01-01", "2026-12-31"};
                (void)r;
                return std::make_unique<ReportCommand>(rs, rr);
            });

        return TradingServerFacade(mds, cs, ms, rs, std::move(registry));
    }
} // namespace

TEST(ServerBootstrapTest, GetMarketDataRequestDoesNotThrowAndReturnsFalse)
{
    auto facade = buildStubFacade();
    Request req;
    req.type = RequestType::GET_MARKET_DATA;
    Response r;
    EXPECT_NO_THROW(r = facade.handleRequest(req));
    EXPECT_FALSE(r.success);
}

TEST(ServerBootstrapTest, CalculateRequestDoesNotThrowAndReturnsFalse)
{
    auto facade = buildStubFacade();
    Request req;
    req.type = RequestType::CALCULATE;
    Response r;
    EXPECT_NO_THROW(r = facade.handleRequest(req));
    EXPECT_FALSE(r.success);
}

TEST(ServerBootstrapTest, ManipulateRequestDoesNotThrowAndReturnsFalse)
{
    auto facade = buildStubFacade();
    Request req;
    req.type = RequestType::MANIPULATE;
    Response r;
    EXPECT_NO_THROW(r = facade.handleRequest(req));
    EXPECT_FALSE(r.success);
}

TEST(ServerBootstrapTest, GenerateReportRequestDoesNotThrowAndReturnsFalse)
{
    auto facade = buildStubFacade();
    Request req;
    req.type = RequestType::GENERATE_REPORT;
    Response r;
    EXPECT_NO_THROW(r = facade.handleRequest(req));
    EXPECT_FALSE(r.success);
}

TEST(ServerBootstrapTest, AllRequestTypesAreRegistered)
{
    auto facade = buildStubFacade();
    for (auto type : {RequestType::GET_MARKET_DATA,
                      RequestType::CALCULATE,
                      RequestType::MANIPULATE,
                      RequestType::GENERATE_REPORT})
    {
        Request req;
        req.type = type;
        EXPECT_NO_THROW(facade.handleRequest(req))
            << "Request type " << static_cast<int>(type) << " not registered";
    }
}
