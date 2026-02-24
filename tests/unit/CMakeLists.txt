cmake_minimum_required(VERSION 3.16)

# Unit test executable for HFTServerInterface.
# Links against GTest and GMock; includes all server and service sources
# so that the tests can instantiate real objects alongside mocks.

find_package(GTest REQUIRED)

# ──────────────────────────────────────────────────────────────
# Include paths (mirror the root CMakeLists.txt)
# ──────────────────────────────────────────────────────────────
set(HFT_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/shared
    ${CMAKE_SOURCE_DIR}/src/server
    ${CMAKE_SOURCE_DIR}/src/transport
    ${CMAKE_SOURCE_DIR}/src/services/reports
)

# ──────────────────────────────────────────────────────────────
# Sources compiled into the unit test binary
# ──────────────────────────────────────────────────────────────
set(HFT_UNIT_TEST_SOURCES
    test_CommandRegistry.cpp
    test_TradingServerFacade.cpp
    test_MarketDataService.cpp
    test_CalculationService.cpp
    test_ManipulationService.cpp
    test_EndOfDayReport.cpp
    test_ServerBootstrap.cpp

    # Server implementation sources
    ${CMAKE_SOURCE_DIR}/src/server/CommandRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/server/TradingServerFacade.cpp

    # Report implementation sources
    ${CMAKE_SOURCE_DIR}/src/services/reports/BaseReport.cpp
    ${CMAKE_SOURCE_DIR}/src/services/reports/EndOfDayReport.cpp
)

add_executable(hft_unit_tests ${HFT_UNIT_TEST_SOURCES})

target_include_directories(hft_unit_tests PRIVATE ${HFT_INCLUDE_DIRS})

target_link_libraries(hft_unit_tests
    PRIVATE
        GTest::GTest
        GTest::Main
        GTest::gmock
        GTest::gmock_main
)

target_compile_features(hft_unit_tests PRIVATE cxx_std_17)

# Register with CTest
include(GoogleTest)
gtest_discover_tests(hft_unit_tests)

# TODO: EXTEND — Add new test source files here when new services or
#               commands are introduced.
