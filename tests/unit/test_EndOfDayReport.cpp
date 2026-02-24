/**
 * @file test_EndOfDayReport.cpp
 * @brief Unit tests for EndOfDayReport and the BaseReport pipeline.
 *
 * Tests each pipeline step independently using a controllable mock subclass,
 * and validates the full pipeline sequence via EndOfDayReport stubs.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "services/reports/BaseReport.hpp"
#include "models/ReportRequest.hpp"

// Pull in the concrete report (stubs return empty collections).
#include "EndOfDayReport.hpp"

using ::testing::Return;
using ::testing::_;

// ---------------------------------------------------------------------------
// Mock subclass for testing the BaseReport Template Method pipeline
// ---------------------------------------------------------------------------

class MockReport : public BaseReport
{
public:
    MOCK_METHOD(ReportData, fetchData,     (const ReportRequest&),  (override));
    MOCK_METHOD(ReportData, computeReport, (const ReportData&),     (override));
    MOCK_METHOD(Report,     format,        (const ReportData&),     (override));
};

// ---------------------------------------------------------------------------
// Tests for the BaseReport pipeline
// ---------------------------------------------------------------------------

TEST(BaseReportTest, PipelineCallsAllThreeStepsInOrder)
{
    MockReport mock;

    ReportData rawData     = {"raw1", "raw2"};
    ReportData computedData = {"computed1"};
    Report     finalReport  = {"line1", "line2"};

    ::testing::InSequence seq;
    EXPECT_CALL(mock, fetchData(_)).WillOnce(Return(rawData));
    EXPECT_CALL(mock, computeReport(rawData)).WillOnce(Return(computedData));
    EXPECT_CALL(mock, format(computedData)).WillOnce(Return(finalReport));

    ReportRequest req{"EndOfDay", "2024-01-01", "2024-01-31"};
    auto result = mock.generate(req);

    EXPECT_EQ(result, finalReport);
}

TEST(BaseReportTest, EmptyDataProducesEmptyReport)
{
    MockReport mock;

    EXPECT_CALL(mock, fetchData(_)).WillOnce(Return(ReportData{}));
    EXPECT_CALL(mock, computeReport(ReportData{})).WillOnce(Return(ReportData{}));
    EXPECT_CALL(mock, format(ReportData{})).WillOnce(Return(Report{}));

    ReportRequest req{"EndOfDay", "2024-01-01", "2024-01-01"};
    auto result = mock.generate(req);

    EXPECT_TRUE(result.empty());
}

// ---------------------------------------------------------------------------
// Tests for the EndOfDayReport stub implementation
// ---------------------------------------------------------------------------

TEST(EndOfDayReportTest, GenerateReturnsEmptyReportFromStubs)
{
    EndOfDayReport report;
    ReportRequest req{"EndOfDay", "2024-01-01", "2024-01-31"};

    // Stubs return empty collections — no crash expected.
    EXPECT_NO_THROW({
        auto result = report.generate(req);
        EXPECT_TRUE(result.empty());
    });
}

TEST(EndOfDayReportTest, FetchDataReturnsEmptyFromStub)
{
    // Access protected methods via a thin test subclass.
    class TestableEndOfDayReport : public EndOfDayReport
    {
    public:
        using EndOfDayReport::fetchData;
        using EndOfDayReport::computeReport;
        using EndOfDayReport::format;
    };

    TestableEndOfDayReport report;
    ReportRequest req{"EndOfDay", "2024-01-01", "2024-01-31"};

    EXPECT_TRUE(report.fetchData(req).empty());
    EXPECT_TRUE(report.computeReport({}).empty());
    EXPECT_TRUE(report.format({}).empty());
}
