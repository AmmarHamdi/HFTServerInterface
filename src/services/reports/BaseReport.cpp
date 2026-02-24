/**
 * @file BaseReport.cpp
 * @brief Implementation of BaseReport::generate() — the Template Method pipeline.
 */

#include "services/reports/BaseReport.hpp"

Report BaseReport::generate(const ReportRequest& request)
{
    // Step 1: Retrieve raw data from the data source.
    ReportData rawData = fetchData(request);

    // Step 2: Apply business logic to compute the report content.
    ReportData computedData = computeReport(rawData);

    // Step 3: Format the computed data into the final report.
    return format(computedData);

    // TODO: EXTEND — Insert additional pipeline steps here (e.g., validate,
    //               enrich, postProcess) by adding virtual methods to
    //               BaseReport.hpp and calling them in this sequence.
}
