/**
 * @file ITransport.hpp
 * @brief Pure virtual interface abstracting the entire communication stack.
 *
 * @details This interface decouples the server from any specific transport
 * implementation (Boost.Asio SSL, gRPC, plain TCP, etc.). All communication
 * goes through this abstraction, keeping the server layer transport-agnostic.
 */

#ifndef ITRANSPORT_HPP
#define ITRANSPORT_HPP

#include <cstdint>
#include <vector>

/// @brief Alias for a raw binary buffer used in transport operations.
using RawBuffer = std::vector<uint8_t>;

/**
 * @class ITransport
 * @brief Abstract transport interface for send/receive operations.
 *
 * Implement this interface to provide a concrete transport (e.g., Boost.Asio
 * SSL, gRPC, plain TCP). Inject the concrete instance into the server facade.
 */
class ITransport
{
public:
    /// @brief Virtual destructor to ensure proper cleanup of derived classes.
    virtual ~ITransport() = default;

    /**
     * @brief Send a raw binary buffer over the transport.
     * @param buffer The data to transmit.
     */
    virtual void send(const RawBuffer& buffer) = 0;

    /**
     * @brief Receive a raw binary buffer from the transport.
     * @return The received data.
     */
    virtual RawBuffer receive() = 0;

    /**
     * @brief Start the transport (e.g., begin accepting connections).
     */
    virtual void start() = 0;

    /**
     * @brief Stop the transport and release all resources.
     */
    virtual void stop() = 0;

    // TODO: EXTEND — Add new transport methods here (e.g., isConnected(),
    //               setOptions(), sendAsync()) when the communication protocol
    //               requires additional capabilities beyond send/receive.
};

#endif // ITRANSPORT_HPP
