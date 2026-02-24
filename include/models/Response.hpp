/**
 * @file Response.hpp
 * @brief POD-compatible struct representing a server response.
 *
 * @details The server facade returns this struct after executing a command.
 * The transport layer serialises it into a raw binary buffer before
 * transmitting it back to the client.
 */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <cstdint>
#include <string>
#include <vector>

/**
 * @struct Response
 * @brief Represents the server's response to a client request.
 */
struct Response
{
    /// @brief Indicates whether the operation completed successfully.
    bool success{false};

    /// @brief Human-readable status or error message.
    std::string message;

    /// @brief Binary payload containing the operation result data.
    std::vector<uint8_t> data;
};

#endif // RESPONSE_HPP
