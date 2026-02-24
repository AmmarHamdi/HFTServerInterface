/**
 * @file main.cpp
 * @brief Entry point for the HFT Trading Server.
 *
 * @details Wires up all layers of the 3-tier architecture:
 *   - Stub service implementations (replace with real ones when available)
 *   - CommandRegistry populated with all four command factories
 *   - TradingServerFacade backed by the registry and services
 *   - BoostAsioSslTransport bound to the configured host/port
 *
 * Usage:
 *   hft_server [port] [cert.pem] [key.pem]
 *
 *   port      TCP port to listen on          (default: 8443)
 *   cert.pem  Path to PEM server certificate (default: server.crt)
 *   key.pem   Path to PEM private key        (default: server.key)
 */

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

// ── Transport ──────────────────────────────────────────────────────────────
#include "BoostAsioSslTransport.hpp"

// ── Server / Command layer ─────────────────────────────────────────────────
#include "TradingServerFacade.hpp"
#include "server/CommandRegistry.hpp"
#include "server/RequestTypes.hpp"

#include "commands/CalculationCommand.hpp"
#include "commands/GetMarketDataCommand.hpp"
#include "commands/ManipulationCommand.hpp"
#include "commands/ReportCommand.hpp"

// ── Stub service implementations (replace with real ones when ready) ──────
#include "StubServices.hpp"

// ── Models ─────────────────────────────────────────────────────────────────
#include "models/ReportRequest.hpp"
#include "models/Request.hpp"
#include "models/Response.hpp"

// ==========================================================================
// Graceful shutdown
// ==========================================================================

namespace
{
    // Raw pointer so the signal handler (C linkage) can reach it.
    ITransport* g_transport = nullptr;

    void onShutdownSignal(int signum)
    {
        std::cout << "\n[main] Caught signal " << signum
                  << " — stopping server...\n";
        if (g_transport)
            g_transport->stop();
    }
} // namespace

// ==========================================================================
// main
// ==========================================================================

int main(int argc, char* argv[])
{
    // ── Parse command-line arguments ───────────────────────────────────────
    const uint16_t    port     = (argc > 1) ? static_cast<uint16_t>(std::stoul(argv[1])) : 8443;
    const std::string certFile = (argc > 2) ? argv[2] : "certs/server.crt";
    const std::string keyFile  = (argc > 3) ? argv[3] : "certs/server.key";
    const std::string host     = "0.0.0.0";

    std::cout << "[main] Starting HFT server on " << host << ":" << port << "\n"
              << "[main] Certificate : " << certFile << "\n"
              << "[main] Private key : " << keyFile  << "\n";

    // ── Build service layer ────────────────────────────────────────────────
    // TODO: Replace stubs with real implementations once they are available.
    auto marketDataService   = std::make_shared<StubMarketDataService>();
    auto calculationService  = std::make_shared<StubCalculationService>();
    auto manipulationService = std::make_shared<StubManipulationService>();
    auto reportService       = std::make_shared<StubReportService>();

    // ── Populate the CommandRegistry ───────────────────────────────────────
    CommandRegistry registry;

    registry.registerCommand(
        RequestType::GET_MARKET_DATA,
        [marketDataService](const Request& req) {
            return std::make_unique<GetMarketDataCommand>(marketDataService, req);
        });

    registry.registerCommand(
        RequestType::CALCULATE,
        [calculationService](const Request& req) {
            return std::make_unique<CalculationCommand>(calculationService, req);
        });

    registry.registerCommand(
        RequestType::MANIPULATE,
        [manipulationService](const Request& req) {
            return std::make_unique<ManipulationCommand>(manipulationService, req);
        });

    registry.registerCommand(
        RequestType::GENERATE_REPORT,
        [reportService](const Request& req) {
            // Deserialise the generic payload into a ReportRequest.
            // TODO: Replace this placeholder deserialization with a proper
            //       implementation once the wire format is defined.
            ReportRequest reportReq;
            reportReq.reportType = "EndOfDay";
            reportReq.dateFrom   = "2026-01-01";
            reportReq.dateTo     = "2026-12-31";
            (void)req; // suppress unused-parameter warning until deserialisation is added
            return std::make_unique<ReportCommand>(reportService, reportReq);
        });

    // ── Build server facade ────────────────────────────────────────────────
    auto facade = std::make_shared<TradingServerFacade>(
        marketDataService,
        calculationService,
        manipulationService,
        reportService,
        std::move(registry));

    // ── Build transport ────────────────────────────────────────────────────
    BoostAsioSslTransport transport(host, port, certFile, keyFile);

    // ── Install signal handlers ────────────────────────────────────────────
    g_transport = &transport;
    std::signal(SIGINT,  onShutdownSignal);
    std::signal(SIGTERM, onShutdownSignal);

    // ── Start ──────────────────────────────────────────────────────────────
    try
    {
        std::cout << "[main] Server running. Press Ctrl+C to stop.\n";
        transport.start();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "[main] Fatal error: " << ex.what() << "\n";
        return EXIT_FAILURE;
    }

    std::cout << "[main] Server stopped cleanly.\n";
    return EXIT_SUCCESS;
}
