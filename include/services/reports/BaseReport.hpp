/**
 * @file BaseReport.hpp
 * @brief Abstract base class implementing the Template Method pattern for reports.
 *
 * @details Defines a fixed three-step pipeline: fetchData → computeReport →
 * format. Concrete report classes (e.g., EndOfDayReport) override each pure
 * virtual step without changing the pipeline sequence.
 */

#ifndef BASEREPORT_HPP
#define BASEREPORT_HPP

#include <string>
#include <vector>

#include "models/ReportRequest.hpp"

/// @brief A generic report payload — a collection of formatted string rows.
using Report = std::vector<std::string>;

/// @brief Intermediate raw data retrieved from the database layer.
using ReportData = std::vector<std::string>;

/**
 * @class BaseReport
 * @brief Abstract base implementing the Template Method pattern for report generation.
 *
 * Call generate() to execute the full pipeline. Override fetchData(),
 * computeReport(), and format() in concrete subclasses to customise each step.
 */
class BaseReport
{
public:
    /// @brief Virtual destructor to ensure proper cleanup of derived classes.
    virtual ~BaseReport() = default;

    /**
     * @brief Execute the full report generation pipeline.
     *
     * Steps (in order):
     * 1. fetchData(request)   — retrieve raw data from the data source.
     * 2. computeReport(data)  — apply business logic / calculations.
     * 3. format(data)         — transform the computed data into the final report.
     *
     * @param request Specifies the report type and date range.
     * @return The fully formatted report.
     *
     * // TODO: EXTEND — Add new pipeline steps here (e.g., validate(request),
     *          enrich(data), postProcess(report)) to support more complex
     *          reporting workflows.
     */
    Report generate(const ReportRequest& request);

protected:
    /**
     * @brief Fetch raw data required for the report.
     * @param request The report request with date range and type.
     * @return Raw data rows from the data source.
     */
    virtual ReportData fetchData(const ReportRequest& request) = 0;

    /**
     * @brief Apply business logic to compute the report content.
     * @param data Raw data rows returned by fetchData().
     * @return Computed data rows ready for formatting.
     */
    virtual ReportData computeReport(const ReportData& data) = 0;

    /**
     * @brief Format computed data into the final human-readable report.
     * @param data Computed data rows returned by computeReport().
     * @return The final formatted report.
     */
    virtual Report format(const ReportData& data) = 0;
};

#endif // BASEREPORT_HPP
