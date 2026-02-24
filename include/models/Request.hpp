/**
 * @file Request.hpp
 * @brief POD-compatible struct representing a decoded client request.
 *
 * @details Transported over the network as a raw binary buffer. The
 * transport layer deserialises the buffer into this struct before passing
 * it to the server facade.
 */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <cstdint>
#include <vector>

#include "server/RequestTypes.hpp"

/**
 * @struct Request
 * @brief Represents a client request with a typed operation and binary payload.
 */
struct Request
{
    /// @brief The type of operation requested by the client.
    RequestType type{RequestType::GET_MARKET_DATA};

    /// @brief Binary payload containing operation-specific parameters.
    std::vector<uint8_t> payload;
};

#endif // REQUEST_HPP
