Feature: End of Day Report
  As a trading operations manager
  I want to generate an end-of-day report for a given date range
  So that I can review daily trading activity and P&L

  Scenario: Generating an end-of-day report for a single day
    Given the server is running
    And a report service is available
    When I send a GENERATE_REPORT request for report type "EndOfDay" from "2024-01-15" to "2024-01-15"
    Then I receive a successful response
    And the response payload contains the end-of-day summary

  Scenario: Generating an end-of-day report for a date range
    Given the server is running
    And a report service is available
    When I send a GENERATE_REPORT request for report type "EndOfDay" from "2024-01-01" to "2024-01-31"
    Then I receive a successful response
    And the response payload contains summaries for all days in the range

  Scenario: Report request with an invalid date range returns an error
    Given the server is running
    And a report service is available
    When I send a GENERATE_REPORT request with dateFrom "2024-01-31" and dateTo "2024-01-01"
    Then I receive a failure response
    And the response message indicates the date range is invalid

  Scenario: Report for an unknown report type returns an error
    Given the server is running
    And a report service is available
    When I send a GENERATE_REPORT request for report type "UnknownReport"
    Then I receive a failure response
    And the response message indicates the report type is not supported
