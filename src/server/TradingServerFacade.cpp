/**
 * @file TradingServerFacade.cpp
 * @brief Implementation of TradingServerFacade.
 */

#include "TradingServerFacade.hpp"

#include <stdexcept>

TradingServerFacade::TradingServerFacade(
    std::shared_ptr<IMarketDataService>   marketDataService,
    std::shared_ptr<ICalculationService>  calculationService,
    std::shared_ptr<IManipulationService> manipulationService,
    std::shared_ptr<IReportService>       reportService,
    CommandRegistry                       registry)
    : m_marketDataService(std::move(marketDataService))
    , m_calculationService(std::move(calculationService))
    , m_manipulationService(std::move(manipulationService))
    , m_reportService(std::move(reportService))
    , m_registry(std::move(registry))
{
}

Response TradingServerFacade::handleRequest(const Request& request)
{
    try
    {
        auto command = m_registry.create(request);
        return command->execute();
    }
    catch (const std::out_of_range& e)
    {
        return Response{false, std::string("Unknown request type: ") + e.what(), {}};
    }
    catch (const std::exception& e)
    {
        return Response{false, std::string("Internal server error: ") + e.what(), {}};
    }
}
