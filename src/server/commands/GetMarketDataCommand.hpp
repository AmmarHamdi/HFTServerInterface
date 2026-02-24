/**
 * @file GetMarketDataCommand.hpp
 * @brief ICommand implementation that delegates to IMarketDataService::getData().
 */

#ifndef GETMARKETDATACOMMAND_HPP
#define GETMARKETDATACOMMAND_HPP

#include "server/ICommand.hpp"
#include "services/IMarketDataService.hpp"
#include "models/Request.hpp"

#include <memory>
#include <utility>

/**
 * @class GetMarketDataCommand
 * @brief Command that retrieves market data for the parameters in the request.
 */
class GetMarketDataCommand : public ICommand
{
public:
    /**
     * @brief Construct the command with the required service and request.
     * @param service Shared pointer to the market data service.
     * @param request The incoming client request.
     */
    GetMarketDataCommand(std::shared_ptr<IMarketDataService> service,
                         const Request&                      request)
        : m_service(std::move(service))
        , m_request(request)
    {}

    /// @copydoc ICommand::execute
    Response execute() override
    {
        return m_service->getData(m_request);
    }

private:
    std::shared_ptr<IMarketDataService> m_service;
    Request                             m_request;
};

#endif // GETMARKETDATACOMMAND_HPP
