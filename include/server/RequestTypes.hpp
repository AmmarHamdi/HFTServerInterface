/**
 * @file RequestTypes.hpp
 * @brief Enumeration of all supported client request types.
 *
 * @details This enum class is the primary key used by the CommandRegistry to
 * dispatch incoming requests to the correct command factory. Extend this enum
 * whenever a new business operation must be exposed to the client.
 */

#ifndef REQUESTTYPES_HPP
#define REQUESTTYPES_HPP

#include <cstdint>

/**
 * @enum RequestType
 * @brief Identifies the intended operation of an incoming client request.
 */
enum class RequestType : uint32_t
{
    GET_MARKET_DATA  = 0, ///< Fetch current or historical market data.
    CALCULATE        = 1, ///< Run a financial calculation (e.g., P&L, VaR).
    MANIPULATE       = 2, ///< Transform or filter trading data.
    GENERATE_REPORT  = 3, ///< Generate a structured report (e.g., end-of-day).

    // TODO: EXTEND — Add new request types here and register the
    //               corresponding command factory in CommandRegistry.
    //               Example:
    //                 STREAM_QUOTES = 4,
    //                 PLACE_ORDER   = 5,
};

#endif // REQUESTTYPES_HPP
