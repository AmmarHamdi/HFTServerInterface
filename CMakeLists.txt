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
# Tests
# ──────────────────────────────────────────────────────────────
enable_testing()
add_subdirectory(tests/unit)

# TODO: EXTEND — Add add_subdirectory(tests/bdd) once cucumber-cpp is
#               integrated and the BDD step definitions are implemented.
