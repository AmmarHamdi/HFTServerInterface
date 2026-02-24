/**
 * @file TradingServerFacade.hpp
 * @brief Concrete implementation of IServerFacade for the HFT trading server.
 *
 * @details Receives requests from the transport layer, uses the CommandRegistry
 * to create the appropriate command, and executes it. All four service
 * dependencies are injected via the constructor.
 */

#ifndef TRADINGSERVERFACADE_HPP
#define TRADINGSERVERFACADE_HPP

#include "server/IServerFacade.hpp"
#include "server/CommandRegistry.hpp"
#include "services/IMarketDataService.hpp"
#include "services/ICalculationService.hpp"
#include "services/IManipulationService.hpp"
#include "services/IReportService.hpp"

#include <memory>

/**
 * @class TradingServerFacade
 * @brief Routes incoming requests to the correct service via CommandRegistry.
 *
 * Instantiate once at application startup, injecting all service
 * implementations and a pre-configured CommandRegistry.
 */
class TradingServerFacade : public IServerFacade
{
public:
    /**
     * @brief Construct the facade with all required service dependencies.
     * @param marketDataService  Market data service implementation.
     * @param calculationService Calculation service implementation.
     * @param manipulationService Manipulation service implementation.
     * @param reportService      Report service implementation.
     * @param registry           Pre-populated CommandRegistry.
     *
     * // TODO: EXTEND — Add new service parameters here when a new service
     *          interface is introduced. Store it as a shared_ptr member and
     *          pass it through to the corresponding CommandFactory lambda
     *          registered in the CommandRegistry.
     */
    TradingServerFacade(
        std::shared_ptr<IMarketDataService>   marketDataService,
        std::shared_ptr<ICalculationService>  calculationService,
        std::shared_ptr<IManipulationService> manipulationService,
        std::shared_ptr<IReportService>       reportService,
        CommandRegistry                       registry);

    ~TradingServerFacade() override = default;

    /// @copydoc IServerFacade::handleRequest
    Response handleRequest(const Request& request) override;

private:
    std::shared_ptr<IMarketDataService>   m_marketDataService;
    std::shared_ptr<ICalculationService>  m_calculationService;
    std::shared_ptr<IManipulationService> m_manipulationService;
    std::shared_ptr<IReportService>       m_reportService;
    CommandRegistry                       m_registry;
};

#endif // TRADINGSERVERFACADE_HPP
