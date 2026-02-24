/**
 * @file CalculationCommand.hpp
 * @brief ICommand implementation that delegates to ICalculationService::calculate().
 */

#ifndef CALCULATIONCOMMAND_HPP
#define CALCULATIONCOMMAND_HPP

#include "server/ICommand.hpp"
#include "services/ICalculationService.hpp"
#include "models/Request.hpp"

#include <memory>
#include <utility>

/**
 * @class CalculationCommand
 * @brief Command that executes a financial calculation based on the request.
 */
class CalculationCommand : public ICommand
{
public:
    /**
     * @brief Construct the command with the required service and request.
     * @param service Shared pointer to the calculation service.
     * @param request The incoming client request.
     */
    CalculationCommand(std::shared_ptr<ICalculationService> service,
                       const Request&                       request)
        : m_service(std::move(service))
        , m_request(request)
    {}

    /// @copydoc ICommand::execute
    Response execute() override
    {
        return m_service->calculate(m_request);
    }

private:
    std::shared_ptr<ICalculationService> m_service;
    Request                              m_request;
};

#endif // CALCULATIONCOMMAND_HPP
