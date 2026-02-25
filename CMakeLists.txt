cmake_minimum_required(VERSION 3.16)
project(HFTServerInterface VERSION 1.0.0 LANGUAGES CXX)

# ──────────────────────────────────────────────────────────────
# Global C++ standard
# ──────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ──────────────────────────────────────────────────────────────
# Dependencies
# ──────────────────────────────────────────────────────────────
find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)
find_package(GTest QUIET)          # Optional — only needed for tests

# ── spdlog ────────────────────────────────────────────────────
include(FetchContent)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.14.1
)
FetchContent_MakeAvailable(spdlog)

# ── cxxopts ───────────────────────────────────────────────────
FetchContent_Declare(
    cxxopts
    GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
    GIT_TAG        v3.2.0
)
FetchContent_MakeAvailable(cxxopts)

# ──────────────────────────────────────────────────────────────
# Global include paths
# ──────────────────────────────────────────────────────────────
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/shared
    ${Boost_INCLUDE_DIRS}
)

# ──────────────────────────────────────────────────────────────
# Transport library (Boost.Asio + OpenSSL)
# ──────────────────────────────────────────────────────────────
add_library(hft_transport STATIC
    src/transport/BoostAsioSslTransport.cpp
)

target_include_directories(hft_transport PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/transport
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(hft_transport PUBLIC
    Boost::system
    OpenSSL::SSL
    OpenSSL::Crypto
    spdlog::spdlog
)

# ──────────────────────────────────────────────────────────────
# Server library (facade + command registry)
# ──────────────────────────────────────────────────────────────
add_library(hft_server STATIC
    src/server/CommandRegistry.cpp
    src/server/TradingServerFacade.cpp
)

target_include_directories(hft_server PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/server
)

# ──────────────────────────────────────────────────────────────
# Services library (report pipeline)
# ──────────────────────────────────────────────────────────────
add_library(hft_services STATIC
    src/services/reports/BaseReport.cpp
    src/services/reports/EndOfDayReport.cpp
)

target_include_directories(hft_services PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/services/reports
)

# TODO: EXTEND — Add new service source files to hft_services (or create a
#               separate CMake target) when a new concrete service is introduced.

# ──────────────────────────────────────────────────────────────
# Server executable
# ──────────────────────────────────────────────────────────────
add_executable(hft_server_exe
    src/main.cpp
)

target_include_directories(hft_server_exe PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/shared
    ${CMAKE_SOURCE_DIR}/src/server
    ${CMAKE_SOURCE_DIR}/src/transport
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(hft_server_exe PRIVATE
    hft_transport
    hft_server
    hft_services
    spdlog::spdlog
    cxxopts::cxxopts
)

# ──────────────────────────────────────────────────────────────
# Tests
# ──────────────────────────────────────────────────────────────
enable_testing()
add_subdirectory(tests/unit)

# TODO: EXTEND — Add add_subdirectory(tests/bdd) once cucumber-cpp is
#               integrated and the BDD step definitions are implemented.
