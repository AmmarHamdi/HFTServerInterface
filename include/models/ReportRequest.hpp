/**
 * @file ReportRequest.hpp
 * @brief POD-compatible struct representing a report generation request.
 *
 * @details Carries the parameters required to identify and scope a report:
 * the report type (e.g., "EndOfDay") and the inclusive date range. Passed
 * directly to IReportService::generateReport() and into the BaseReport
 * pipeline.
 */

#ifndef REPORTREQUEST_HPP
#define REPORTREQUEST_HPP

#include <string>

/**
 * @struct ReportRequest
 * @brief Specifies the type and date range of a report generation request.
 */
struct ReportRequest
{
    /// @brief Identifies which report to generate (e.g., "EndOfDay", "Blotter").
    std::string reportType;

    /// @brief Inclusive start date in ISO 8601 format (YYYY-MM-DD).
    std::string dateFrom;

    /// @brief Inclusive end date in ISO 8601 format (YYYY-MM-DD).
    std::string dateTo;
};

#endif // REPORTREQUEST_HPP
