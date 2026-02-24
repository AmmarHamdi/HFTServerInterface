/**
 * @file StubServices.hpp
 * @brief Placeholder implementations of all four service interfaces.
 *
 * @details These stubs satisfy the dependency-injection requirements of
 * TradingServerFacade so the server binary can start without a real database
 * or business-logic layer. Every method returns a "not yet implemented"
 * response.
 *
 * Replace each stub with a concrete implementation once the corresponding
 * service is available, and inject the real object in main() instead.
 *
 * @note Kept in a separate header (rather than inside main.cpp) so that
 * the unit-test suite can also instantiate and exercise these stubs directly.
 */

#ifndef STUBSERVICES_HPP
#define STUBSERVICES_HPP

#include <iostream>
#include <string>

#include "services/ICalculationService.hpp"
#include "services/IManipulationService.hpp"
#include "services/IMarketDataService.hpp"
#include "services/IReportService.hpp"

// ─────────────────────────────────────────────────────────────────────────────

/**
 * @class StubMarketDataService
 * @brief Placeholder IMarketDataService — always returns "not yet implemented".
 *
 * // TODO: IMPLEMENT — Replace with a real implementation that queries the
 *          PostgreSQL database (or a live market-data feed) and returns
 *          serialised MarketData payloads.
 */
class StubMarketDataService : public IMarketDataService
{
public:
    Response getData(const Request&) override
    {
        return {false, "MarketDataService: not yet implemented", {}};
    }

    void subscribe(const std::string& symbol) override
    {
        std::cout << "[StubMarketDataService] subscribe: " << symbol << "\n";
    }

    void unsubscribe(const std::string& symbol) override
    {
        std::cout << "[StubMarketDataService] unsubscribe: " << symbol << "\n";
    }
};

// ─────────────────────────────────────────────────────────────────────────────

/**
 * @class StubCalculationService
 * @brief Placeholder ICalculationService — always returns "not yet implemented".
 *
 * // TODO: IMPLEMENT — Replace with a real implementation that runs P&L,
 *          VaR, Greeks, or other quantitative models against live positions.
 */
class StubCalculationService : public ICalculationService
{
public:
    Response calculate(const Request&) override
    {
        return {false, "CalculationService: not yet implemented", {}};
    }
};

// ─────────────────────────────────────────────────────────────────────────────

/**
 * @class StubManipulationService
 * @brief Placeholder IManipulationService — always returns "not yet implemented".
 *
 * // TODO: IMPLEMENT — Replace with a real implementation that filters,
 *          aggregates, or transforms in-memory trading datasets.
 */
class StubManipulationService : public IManipulationService
{
public:
    Response manipulate(const Request&) override
    {
        return {false, "ManipulationService: not yet implemented", {}};
    }

    Response transform(const Request&) override
    {
        return {false, "ManipulationService::transform: not yet implemented", {}};
    }
};

// ─────────────────────────────────────────────────────────────────────────────

/**
 * @class StubReportService
 * @brief Placeholder IReportService — always returns "not yet implemented".
 *
 * // TODO: IMPLEMENT — Replace with a real implementation that instantiates
 *          the appropriate BaseReport subclass based on ReportRequest::reportType.
 */
class StubReportService : public IReportService
{
public:
    Response generateReport(const ReportRequest& req) override
    {
        return {false,
                "ReportService: not yet implemented for " + req.reportType,
                {}};
    }
};

#endif // STUBSERVICES_HPP
