# HFTServerInterface

![CI](https://github.com/AmmarHamdi/HFTServerInterface/actions/workflows/ci.yml/badge.svg)

A **C++ Linux server interface** for a High-Frequency Trading (HFT) application
being migrated from a monolithic Windows MFC client to a **3-tier architecture**.
The server exposes all business logic services to a Windows GUI client while
remaining completely agnostic of the communication stack.

---

## Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    CLIENT (Windows MFC)                  │
│                      [OUT OF SCOPE]                     │
└──────────────────────────┬──────────────────────────────┘
                           │ Boost.Asio + SSL + POD
┌──────────────────────────▼──────────────────────────────┐
│                   TRANSPORT LAYER                        │
│              ITransport ◄── BoostAsioSslTransport        │
├─────────────────────────────────────────────────────────┤
│                SERVER INTERFACE LAYER                    │
│         IServerFacade ◄── TradingServerFacade           │
│              CommandRegistry / Dispatcher               │
├─────────────────────────────────────────────────────────┤
│                   SERVICE LAYER                          │
│   IMarketDataService  │  ICalculationService            │
│   IManipulationService│  IReportService                 │
├─────────────────────────────────────────────────────────┤
│                  DATABASE LAYER                          │
│                PostgreSQL [OUT OF SCOPE]                 │
└─────────────────────────────────────────────────────────┘
```

---

## Communication Stack

**Iteration 1:** Boost.Asio + SSL/TLS + binary POD structs
(`shared/pod/TradingPOD.hpp`).

The transport is fully abstracted behind `ITransport`. To swap the
communication stack (e.g., replace Boost.Asio with gRPC):

1. Create a new class that inherits `ITransport`.
2. Implement `send()`, `receive()`, `start()`, `stop()`.
3. Inject the new implementation at startup — no changes to the server
   or service layers are required.

---

## Design Patterns

| Pattern | Layer | Purpose |
|---|---|---|
| **Facade** | Server Interface | Single entry point via `TradingServerFacade` |
| **Command** | Server Interface | Encapsulate each operation as an `ICommand` |
| **Registry / Factory** | Server Interface | `CommandRegistry` maps `RequestType` → factory |
| **Template Method** | Services / Reports | `BaseReport` defines a fixed pipeline; subclasses fill in steps |
| **Dependency Injection** | All | Services injected as `std::shared_ptr` via constructors |
| **Interface Segregation** | All | Each service has its own focused interface |

---

## Services

| Interface | Responsibility |
|---|---|
| `IMarketDataService` | Retrieve real-time / historical prices; subscribe/unsubscribe to symbols |
| `ICalculationService` | Execute financial calculations (P&L, VaR, Greeks) |
| `IManipulationService` | Filter, transform, and aggregate trading data |
| `IReportService` | Generate structured reports (e.g., end-of-day summary) |

---

## Extensibility Guide

### Adding a new service

1. Create `include/services/IMyNewService.hpp` — pure virtual interface.
2. Create `src/services/MyNewService.hpp/.cpp` — concrete implementation.
3. Add `std::shared_ptr<IMyNewService>` parameter to `TradingServerFacade`.
4. Add a new `RequestType` value in `include/server/RequestTypes.hpp`.
5. Create a command in `src/server/commands/MyNewCommand.hpp`.
6. Register the factory in `CommandRegistry` bootstrap code.

### Modifying an existing service

1. Add the new method signature to the relevant `IXxxService.hpp` interface.
2. Implement the method in the concrete service class.
3. Update GMock mock classes in the unit test files.

### Adding a new report type

1. Create `src/services/reports/MyNewReport.hpp/.cpp`.
2. Inherit `BaseReport`; override `fetchData()`, `computeReport()`, `format()`.
3. In the concrete `IReportService` implementation, dispatch on
   `ReportRequest::reportType` to instantiate the new report class.

### Adding a new transport (e.g., gRPC)

1. Create `src/transport/GrpcTransport.hpp/.cpp`.
2. Inherit `ITransport`; implement all four methods.
3. Replace `BoostAsioSslTransport` at the application entry point.

### Adding a new request type

1. Add a new enumerator to `RequestType` in `include/server/RequestTypes.hpp`.
2. Create the corresponding command in `src/server/commands/`.
3. Register the factory in the application bootstrap code using
   `CommandRegistry::registerCommand()`.

---

## Shared Cross-Platform Code (`shared/`)

The `shared/` directory contains headers that are compiled on **both** the
Windows MFC client and the Linux server:

- **`shared/pod/TradingPOD.hpp`** — `#pragma pack(1)` POD structs
  (`MarketDataPOD`, `OrderPOD`, `PositionPOD`, `TradePOD`) transmitted as
  raw binary over the transport layer.
- **`shared/models/MarketData.hpp`** — compact `MarketData` struct for
  real-time price snapshots.

**Rules for shared code:**
- Only plain-old-data (no `std::string`, no virtual methods, no pointers).
- Fixed-size primitive members only.
- Always use `#pragma pack(push, 1)` / `#pragma pack(pop)`.

---

## Testing Strategy

| Level | Framework | Scope | Approach |
|---|---|---|---|
| **Unit** | GTest + GMock | Single class / method | Mock all dependencies; test in isolation |
| **BDD** | Cucumber-cpp | End-to-end feature scenarios | Gherkin feature files + C++ step definitions |

---

## How to Build

```bash
# From the repository root
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

**Requirements:** CMake ≥ 3.16, C++17 compiler, Boost (system), OpenSSL, GTest/GMock.

---

## How to Run Tests

### Unit tests (GTest)

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
cd build
ctest --output-on-failure
# or run directly:
./tests/unit/hft_unit_tests
```

### BDD tests (Cucumber-cpp)

> **Note:** BDD step definitions contain `// TODO: IMPLEMENT` markers.
> Full BDD execution requires cucumber-cpp to be integrated and steps
> to be implemented.

```bash
# After implementing the steps:
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
cd build/tests/bdd
./hft_bdd_tests --format progress
```

---

## Out of Scope

- **Windows MFC GUI client** — The client application consumes the server
  interface but is not part of this repository.
- **PostgreSQL database layer** — Database access is handled by the concrete
  service implementations (stub/TODO stubs provided).

---

## TODO Markers Guide

| Marker | Meaning |
|---|---|
| `// TODO: EXTEND` | Extension point — add new services, commands, methods, or pipeline steps here |
| `// TODO: REGISTER` | Registration point — call `CommandRegistry::registerCommand()` for every new command here |
| `// TODO: IMPLEMENT` | Concrete logic is missing — fill in the actual business / I/O logic here |
