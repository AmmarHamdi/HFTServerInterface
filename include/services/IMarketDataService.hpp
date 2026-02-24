/**
 * @file IMarketDataService.hpp
 * @brief Pure virtual interface for market data operations.
 *
 * @details Provides access to real-time and historical market data for trading
 * symbols. Implementations communicate with the database or external market
 * data feeds.
 */

#ifndef IMARKETDATASERVICE_HPP
#define IMARKETDATASERVICE_HPP

#include <string>

#include "models/Request.hpp"
#include "models/Response.hpp"

/**
 * @class IMarketDataService
 * @brief Abstract interface for retrieving and subscribing to market data.
 */
class IMarketDataService
{
public:
    /// @brief Virtual destructor to ensure proper cleanup of derived classes.
    virtual ~IMarketDataService() = default;

    /**
     * @brief Retrieve market data according to the parameters in the request.
     * @param request Contains the symbol, time range, and other query params.
     * @return A Response with the serialized market data payload.
     */
    virtual Response getData(const Request& request) = 0;

    /**
     * @brief Subscribe to real-time updates for a trading symbol.
     * @param symbol The instrument identifier (e.g., "AAPL", "EURUSD").
     */
    virtual void subscribe(const std::string& symbol) = 0;

    /**
     * @brief Unsubscribe from real-time updates for a trading symbol.
     * @param symbol The instrument identifier to stop tracking.
     */
    virtual void unsubscribe(const std::string& symbol) = 0;

    // TODO: EXTEND — Add new market data operations here (e.g., getOrderBook(),
    //               getHistoricalData(), getBestBidAsk()) as data requirements grow.
};

#endif // IMARKETDATASERVICE_HPP
