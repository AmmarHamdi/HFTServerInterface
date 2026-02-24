/**
 * @file MarketData.hpp
 * @brief Cross-platform MarketData struct shared between client and server.
 *
 * @details A compact, fixed-size struct used for transmitting real-time
 * market snapshots over the transport layer. Compatible with both Windows
 * MFC client and Linux server without additional serialisation.
 */

#ifndef MARKETDATA_HPP
#define MARKETDATA_HPP

#include <cstdint>

#pragma pack(push, 1)

/**
 * @struct MarketData
 * @brief Real-time market snapshot for a single instrument.
 */
struct MarketData
{
    char     symbol[32]; ///< Null-terminated instrument identifier (e.g., "EURUSD").
    double   price;      ///< Latest traded or mid price.
    uint64_t volume;     ///< Cumulative traded volume in the current session.
    int64_t  timestamp;  ///< Snapshot time as Unix epoch in microseconds.
};

#pragma pack(pop)

#endif // MARKETDATA_HPP
