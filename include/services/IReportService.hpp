/**
 * @file IReportService.hpp
 * @brief Pure virtual interface for report generation operations.
 *
 * @details Provides a single entry point for generating all structured
 * reports (e.g., end-of-day summary, trade blotter, P&L report). Each
 * concrete implementation delegates to a BaseReport-derived object that
 * implements the Template Method pipeline.
 */

#ifndef IREPORTSERVICE_HPP
#define IREPORTSERVICE_HPP

#include "models/ReportRequest.hpp"
#include "models/Response.hpp"

/**
 * @class IReportService
 * @brief Abstract interface for triggering report generation.
 */
class IReportService
{
public:
    /// @brief Virtual destructor to ensure proper cleanup of derived classes.
    virtual ~IReportService() = default;

    /**
     * @brief Generate a report based on the provided report request.
     * @param request Specifies the report type and the date range.
     * @return A Response containing the serialized report or an error message.
     */
    virtual Response generateReport(const ReportRequest& request) = 0;

    // TODO: EXTEND — Add new report entry points here (e.g., scheduleReport(),
    //               exportReport()) as reporting requirements grow.
};

#endif // IREPORTSERVICE_HPP
