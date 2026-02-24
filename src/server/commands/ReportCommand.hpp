/**
 * @file ReportCommand.hpp
 * @brief ICommand implementation that delegates to IReportService::generateReport().
 */

#ifndef REPORTCOMMAND_HPP
#define REPORTCOMMAND_HPP

#include "server/ICommand.hpp"
#include "services/IReportService.hpp"
#include "models/ReportRequest.hpp"

#include <memory>
#include <utility>

/**
 * @class ReportCommand
 * @brief Command that triggers report generation based on a ReportRequest.
 *
 * @note The constructor accepts a ReportRequest (not a generic Request) because
 * report generation requires structured parameters beyond a raw payload.
 */
class ReportCommand : public ICommand
{
public:
    /**
     * @brief Construct the command with the required service and report request.
     * @param service       Shared pointer to the report service.
     * @param reportRequest The structured report parameters.
     */
    ReportCommand(std::shared_ptr<IReportService> service,
                  ReportRequest                   reportRequest)
        : m_service(std::move(service))
        , m_reportRequest(std::move(reportRequest))
    {}

    /// @copydoc ICommand::execute
    Response execute() override
    {
        return m_service->generateReport(m_reportRequest);
    }

private:
    std::shared_ptr<IReportService> m_service;
    ReportRequest                   m_reportRequest;
};

#endif // REPORTCOMMAND_HPP
