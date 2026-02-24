/**
 * @file ManipulationCommand.hpp
 * @brief ICommand implementation that delegates to IManipulationService::manipulate().
 */

#ifndef MANIPULATIONCOMMAND_HPP
#define MANIPULATIONCOMMAND_HPP

#include "server/ICommand.hpp"
#include "services/IManipulationService.hpp"
#include "models/Request.hpp"

#include <memory>
#include <utility>

/**
 * @class ManipulationCommand
 * @brief Command that applies a data manipulation operation based on the request.
 */
class ManipulationCommand : public ICommand
{
public:
    /**
     * @brief Construct the command with the required service and request.
     * @param service Shared pointer to the manipulation service.
     * @param request The incoming client request.
     */
    ManipulationCommand(std::shared_ptr<IManipulationService> service,
                        const Request&                        request)
        : m_service(std::move(service))
        , m_request(request)
    {}

    /// @copydoc ICommand::execute
    Response execute() override
    {
        return m_service->manipulate(m_request);
    }

private:
    std::shared_ptr<IManipulationService> m_service;
    Request                               m_request;
};

#endif // MANIPULATIONCOMMAND_HPP
