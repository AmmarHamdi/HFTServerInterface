/**
 * @file EndOfDayReport.hpp
 * @brief Concrete end-of-day report implementing the BaseReport pipeline.
 *
 * @details Produces a daily summary of all trades, positions, and P&L for a
 * given trading session. Overrides all three pipeline steps defined in
 * BaseReport.
 */

#ifndef ENDOFDAYREPORT_HPP
#define ENDOFDAYREPORT_HPP

#include "services/reports/BaseReport.hpp"

/**
 * @class EndOfDayReport
 * @brief Generates a daily trading summary report.
 */
class EndOfDayReport : public BaseReport
{
public:
    EndOfDayReport() = default;
    ~EndOfDayReport() override = default;

protected:
    /**
     * @brief Fetch all trades and positions for the requested date range.
     * @param request The report request specifying the date range.
     * @return Raw rows from the database.
     */
    ReportData fetchData(const ReportRequest& request) override;

    /**
     * @brief Compute P&L totals and aggregate positions.
     * @param data Raw rows returned by fetchData().
     * @return Computed summary rows.
     */
    ReportData computeReport(const ReportData& data) override;

    /**
     * @brief Format computed rows into the final end-of-day report layout.
     * @param data Computed rows returned by computeReport().
     * @return Formatted report lines.
     */
    Report format(const ReportData& data) override;
};

#endif // ENDOFDAYREPORT_HPP
