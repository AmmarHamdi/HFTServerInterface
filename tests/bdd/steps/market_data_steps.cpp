/**
 * @file market_data_steps.cpp
 * @brief Cucumber-cpp step definitions for market_data.feature.
 *
 * @details Each step maps directly to a Gherkin step in
 * tests/bdd/features/market_data.feature. Concrete logic must be added
 * wherever a TODO: IMPLEMENT marker appears.
 */

#include <cucumber-cpp/autodetect.hpp>

#include "server/CommandRegistry.hpp"
#include "server/RequestTypes.hpp"
#include "models/Request.hpp"
#include "models/Response.hpp"

#include <string>

// ---------------------------------------------------------------------------
// Shared test context
// ---------------------------------------------------------------------------

struct MarketDataContext
{
    Request  lastRequest;
    Response lastResponse;
    bool     serverRunning{false};
};

// ---------------------------------------------------------------------------
// Step definitions
// ---------------------------------------------------------------------------

GIVEN("^the server is running$")
{
    ScenarioScope<MarketDataContext> ctx;
    // TODO: IMPLEMENT — Start or connect to a test server instance.
    ctx->serverRunning = true;
}

GIVEN("^a market data service is available$")
{
    // TODO: IMPLEMENT — Inject or configure the market data service mock/stub.
}

WHEN("^I send a GET_MARKET_DATA request for symbol \"([^\"]*)\"$",
     (const std::string& symbol))
{
    ScenarioScope<MarketDataContext> ctx;
    // TODO: IMPLEMENT — Build a Request with type GET_MARKET_DATA, serialise
    //                   the symbol into the payload, invoke the facade/transport,
    //                   and store the response in ctx->lastResponse.
    (void)symbol;
}

THEN("^I receive a successful response$")
{
    ScenarioScope<MarketDataContext> ctx;
    // TODO: IMPLEMENT — Assert ctx->lastResponse.success == true.
    (void)ctx;
}

THEN("^the response payload contains data for \"([^\"]*)\"$",
     (const std::string& symbol))
{
    ScenarioScope<MarketDataContext> ctx;
    // TODO: IMPLEMENT — Deserialise ctx->lastResponse.data and verify it
    //                   contains a record matching the expected symbol.
    (void)ctx;
    (void)symbol;
}

WHEN("^I subscribe to symbol \"([^\"]*)\"$", (const std::string& symbol))
{
    // TODO: IMPLEMENT — Call IMarketDataService::subscribe(symbol) via the
    //                   server and store any acknowledgement.
    (void)symbol;
}

THEN("^the subscription is acknowledged$")
{
    // TODO: IMPLEMENT — Assert that the subscribe call succeeded without error.
}

GIVEN("^I am already subscribed to symbol \"([^\"]*)\"$",
      (const std::string& symbol))
{
    // TODO: IMPLEMENT — Set up prior subscription state for the scenario.
    (void)symbol;
}

WHEN("^I unsubscribe from symbol \"([^\"]*)\"$", (const std::string& symbol))
{
    // TODO: IMPLEMENT — Call IMarketDataService::unsubscribe(symbol) via the
    //                   server and store any acknowledgement.
    (void)symbol;
}

THEN("^the unsubscription is acknowledged$")
{
    // TODO: IMPLEMENT — Assert that the unsubscribe call succeeded without error.
}

THEN("^I receive a failure response$")
{
    ScenarioScope<MarketDataContext> ctx;
    // TODO: IMPLEMENT — Assert ctx->lastResponse.success == false.
    (void)ctx;
}

THEN("^the response message indicates the symbol was not found$")
{
    ScenarioScope<MarketDataContext> ctx;
    // TODO: IMPLEMENT — Assert ctx->lastResponse.message contains an
    //                   appropriate "not found" or "unknown symbol" message.
    (void)ctx;
}
