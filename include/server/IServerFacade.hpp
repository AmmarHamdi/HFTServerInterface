/**
 * @file IServerFacade.hpp
 * @brief Pure virtual interface for the server entry point.
 *
 * @details Provides a single unified entry point for all client requests.
 * Implementations (e.g., TradingServerFacade) dispatch requests to the
 * appropriate service via the CommandRegistry.
 */

#ifndef ISERVERFACADE_HPP
#define ISERVERFACADE_HPP

#include "models/Request.hpp"
#include "models/Response.hpp"

/**
 * @class IServerFacade
 * @brief Abstract facade interface representing the server's public API.
 *
 * All client requests pass through this interface, which routes them to the
 * correct service using a command dispatcher pattern.
 */
class IServerFacade
{
public:
    /// @brief Virtual destructor to ensure proper cleanup of derived classes.
    virtual ~IServerFacade() = default;

    /**
     * @brief Handle an incoming client request and return a response.
     * @param request The decoded client request.
     * @return A response to be serialized and sent back to the client.
     */
    virtual Response handleRequest(const Request& request) = 0;

    // TODO: EXTEND — Add new entry points here for additional server
    //               capabilities (e.g., handleBatchRequest(), subscribe(),
    //               getServerStatus()) as the API surface grows.
};

#endif // ISERVERFACADE_HPP
