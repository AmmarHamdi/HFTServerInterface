/**
 * @file report_steps.cpp
 * @brief Cucumber-cpp step definitions for end_of_day_report.feature.
 *
 * @details Each step maps directly to a Gherkin step in
 * tests/bdd/features/end_of_day_report.feature. Concrete logic must be
 * added wherever a TODO: IMPLEMENT marker appears.
 */

#include <cucumber-cpp/autodetect.hpp>

#include "services/IReportService.hpp"
#include "models/ReportRequest.hpp"
#include "models/Response.hpp"

#include <string>

// ---------------------------------------------------------------------------
// Shared test context
// ---------------------------------------------------------------------------

struct ReportContext
{
    ReportRequest lastReportRequest;
    Response      lastResponse;
};

// ---------------------------------------------------------------------------
// Step definitions
// ---------------------------------------------------------------------------

GIVEN("^a report service is available$")
{
    // TODO: IMPLEMENT — Inject or configure the report service mock/stub.
}

WHEN("^I send a GENERATE_REPORT request for report type \"([^\"]*)\" "
     "from \"([^\"]*)\" to \"([^\"]*)\"$",
     (const std::string& reportType,
      const std::string& dateFrom,
      const std::string& dateTo))
{
    ScenarioScope<ReportContext> ctx;
    // TODO: IMPLEMENT — Build a ReportRequest{reportType, dateFrom, dateTo},
    //                   invoke the report service or server facade, and store
    //                   the result in ctx->lastResponse.
    ctx->lastReportRequest = ReportRequest{reportType, dateFrom, dateTo};
    (void)ctx;
}

THEN("^the response payload contains the end-of-day summary$")
{
    ScenarioScope<ReportContext> ctx;
    // TODO: IMPLEMENT — Deserialise ctx->lastResponse.data and verify it
    //                   contains expected report lines/fields.
    (void)ctx;
}

THEN("^the response payload contains summaries for all days in the range$")
{
    ScenarioScope<ReportContext> ctx;
    // TODO: IMPLEMENT — Assert the response contains one summary entry per
    //                   calendar day between dateFrom and dateTo.
    (void)ctx;
}

WHEN("^I send a GENERATE_REPORT request with dateFrom \"([^\"]*)\" "
     "and dateTo \"([^\"]*)\"$",
     (const std::string& dateFrom, const std::string& dateTo))
{
    ScenarioScope<ReportContext> ctx;
    // TODO: IMPLEMENT — Build a ReportRequest with the provided dates and
    //                   invoke the service, expecting a validation failure.
    ctx->lastReportRequest = ReportRequest{"EndOfDay", dateFrom, dateTo};
    (void)ctx;
}

THEN("^the response message indicates the date range is invalid$")
{
    ScenarioScope<ReportContext> ctx;
    // TODO: IMPLEMENT — Assert ctx->lastResponse.message contains an
    //                   appropriate "invalid date range" message.
    (void)ctx;
}

WHEN("^I send a GENERATE_REPORT request for report type \"([^\"]*)\"$",
     (const std::string& reportType))
{
    ScenarioScope<ReportContext> ctx;
    // TODO: IMPLEMENT — Build a ReportRequest with the unknown type and
    //                   invoke the service, expecting a type-not-found error.
    ctx->lastReportRequest = ReportRequest{reportType, "", ""};
    (void)ctx;
}

THEN("^the response message indicates the report type is not supported$")
{
    ScenarioScope<ReportContext> ctx;
    // TODO: IMPLEMENT — Assert ctx->lastResponse.message contains an
    //                   appropriate "not supported" or "unknown report type" message.
    (void)ctx;
}
