/**
 * @file TradingPOD.hpp
 * @brief Cross-platform POD structs shared between Windows client and Linux server.
 *
 * @details All structs are packed with #pragma pack(1) to guarantee identical
 * binary layout on both Windows (MFC client) and Linux (server). These types
 * are serialised directly into the RawBuffer transmitted over the transport
 * layer — no additional serialisation library is required for iteration 1.
 *
 * @note Include this header on both client and server sides. Do NOT add
 *       non-POD members (std::string, pointers, virtual methods, etc.).
 */

#ifndef TRADINGPOD_HPP
#define TRADINGPOD_HPP

#include <cstdint>

#pragma pack(push, 1)

/**
 * @struct MarketDataPOD
 * @brief Point-in-time snapshot of a tradeable instrument.
 */
struct MarketDataPOD
{
    char     symbol[32];  ///< Null-terminated instrument identifier.
    double   bid;         ///< Best bid price.
    double   ask;         ///< Best ask price.
    double   last;        ///< Last traded price.
    uint64_t volume;      ///< Traded volume in the current session.
    int64_t  timestamp;   ///< Unix epoch timestamp in microseconds.
};

/**
 * @struct OrderPOD
 * @brief Represents a trading order submitted by the client.
 */
struct OrderPOD
{
    uint64_t orderId;     ///< Unique order identifier.
    char     symbol[32];  ///< Instrument identifier.
    double   price;       ///< Limit price (0.0 for market orders).
    uint64_t quantity;    ///< Order quantity (number of units).
    uint8_t  side;        ///< 0 = Buy, 1 = Sell.
    uint8_t  type;        ///< 0 = Market, 1 = Limit, 2 = Stop.
    int64_t  timestamp;   ///< Order creation time (Unix epoch, microseconds).
};

/**
 * @struct PositionPOD
 * @brief Current open position for a single instrument.
 */
struct PositionPOD
{
    char     symbol[32];  ///< Instrument identifier.
    int64_t  quantity;    ///< Net position (positive = long, negative = short).
    double   avgPrice;    ///< Volume-weighted average entry price.
    double   unrealisedPnl; ///< Mark-to-market unrealised P&L.
    int64_t  timestamp;   ///< Position snapshot time (Unix epoch, microseconds).
};

/**
 * @struct TradePOD
 * @brief Immutable record of a completed trade (fill).
 */
struct TradePOD
{
    uint64_t tradeId;     ///< Unique trade identifier.
    uint64_t orderId;     ///< Parent order identifier.
    char     symbol[32];  ///< Instrument identifier.
    double   price;       ///< Execution price.
    uint64_t quantity;    ///< Executed quantity.
    uint8_t  side;        ///< 0 = Buy fill, 1 = Sell fill.
    int64_t  timestamp;   ///< Execution time (Unix epoch, microseconds).
};

// TODO: EXTEND — Add new shared POD types here following the same
//               #pragma pack(1) convention. Ensure the struct contains
//               only fixed-size primitive members so the binary layout
//               remains identical on Windows and Linux.

#pragma pack(pop)

#endif // TRADINGPOD_HPP
