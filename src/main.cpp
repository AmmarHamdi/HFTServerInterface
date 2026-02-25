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
 *   hft_server_exe [options]
 *
 * Run with --help to see all available options.
 */

#include <atomic>
#include <csignal>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <chrono>

#include <cxxopts.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

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
    /// Set to true by the signal handler; polled by the main thread.
    std::atomic<bool> g_shutdown{false};

    /// Raw pointer so the signal handler (C linkage) can reach the transport.
    ITransport* g_transport = nullptr;

    void onShutdownSignal(int signum)
    {
        // Only async-signal-safe operations: atomic store + nothing else.
        g_shutdown.store(true, std::memory_order_relaxed);
        (void)signum;
    }
} // namespace

// ==========================================================================
// main
// ==========================================================================

int main(int argc, char* argv[])
{
    // ── Set up a named, coloured console logger ────────────────────────────
    auto logger = spdlog::stdout_color_mt("hft");
    spdlog::set_default_logger(logger);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] %v");

    // ── Parse command-line arguments with cxxopts ──────────────────────────
    cxxopts::Options options("hft_server_exe", "HFT Trading Server — Boost.Asio SSL/TLS");

    options.add_options()
        ("p,port",  "TCP port to listen on",
            cxxopts::value<uint16_t>()->default_value("8443"))
        ("H,host",  "Bind address",
            cxxopts::value<std::string>()->default_value("0.0.0.0"))
        ("c,cert",  "Path to PEM server certificate",
            cxxopts::value<std::string>()->default_value("certs/server.crt"))
        ("k,key",   "Path to PEM private key",
            cxxopts::value<std::string>()->default_value("certs/server.key"))
        ("l,log-level", "Log level: trace|debug|info|warn|error|critical",
            cxxopts::value<std::string>()->default_value("info"))
        ("h,help",  "Print this help message and exit");

    cxxopts::ParseResult args;
    try
    {
        args = options.parse(argc, argv);
    }
    catch (const cxxopts::exceptions::exception& ex)
    {
        spdlog::error("Argument error: {}", ex.what());
        std::cerr << options.help() << "\n";
        return EXIT_FAILURE;
    }

    if (args.count("help"))
    {
        std::cout << options.help() << "\n";
        return EXIT_SUCCESS;
    }

    // Apply requested log level.
    const auto logLevelStr = args["log-level"].as<std::string>();
    const auto logLevel    = spdlog::level::from_str(logLevelStr);
    if (logLevel == spdlog::level::off && logLevelStr != "off")
    {
        spdlog::warn("Unknown log level '{}', defaulting to 'info'", logLevelStr);
        spdlog::set_level(spdlog::level::info);
    }
    else
    {
        spdlog::set_level(logLevel);
    }

    const uint16_t    port     = args["port"].as<uint16_t>();
    const std::string host     = args["host"].as<std::string>();
    const std::string certFile = args["cert"].as<std::string>();
    const std::string keyFile  = args["key"].as<std::string>();

    spdlog::info("Starting HFT server on {}:{}", host, port);
    spdlog::info("Certificate : {}", certFile);
    spdlog::info("Private key : {}", keyFile);
    spdlog::debug("Log level   : {}", logLevelStr);

    // ── Build service layer ────────────────────────────────────────────────
    // TODO: Replace stubs with real implementations once they are available.
    auto marketDataService   = std::make_shared<StubMarketDataService>();
    auto calculationService  = std::make_shared<StubCalculationService>();
    auto manipulationService = std::make_shared<StubManipulationService>();
    auto reportService       = std::make_shared<StubReportService>();

    spdlog::debug("Service layer created (stubs)");

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
            // TODO: Replace placeholder deserialisation with wire format parser.
            ReportRequest reportReq;
            reportReq.reportType = "EndOfDay";
            reportReq.dateFrom   = "2026-01-01";
            reportReq.dateTo     = "2026-12-31";
            (void)req;
            return std::make_unique<ReportCommand>(reportService, reportReq);
        });

    spdlog::debug("CommandRegistry populated ({} commands)",
                  static_cast<int>(RequestType::GENERATE_REPORT) + 1);

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
    // NOTE: Only async-signal-safe operations are used inside the handler.
    //       The main thread polls g_shutdown and calls transport.stop() safely.
    g_transport = &transport;
    std::signal(SIGINT,  onShutdownSignal);
    std::signal(SIGTERM, onShutdownSignal);

    // ── Start transport (non-blocking — runs io_context on a background thread)
    try
    {
        transport.start();
    }
    catch (const std::exception& ex)
    {
        spdlog::critical("Failed to start transport: {}", ex.what());
        return EXIT_FAILURE;
    }

    spdlog::info("Server running. Press Ctrl+C to stop.");

    // ── Block main thread until a shutdown signal is received ──────────────
    // transport.start() is non-blocking (background thread). Without this loop
    // main() would return immediately and the process would exit, which is why
    // the server appeared to start and stop instantly.
    while (!g_shutdown.load(std::memory_order_relaxed))
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // ── Graceful shutdown ──────────────────────────────────────────────────
    spdlog::info("Shutdown signal received — stopping server...");
    try
    {
        transport.stop();
    }
    catch (const std::exception& ex)
    {
        spdlog::error("Error during shutdown: {}", ex.what());
    }

    spdlog::info("Server stopped cleanly.");
    return EXIT_SUCCESS;
}

