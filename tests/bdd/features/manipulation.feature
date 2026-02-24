Feature: Data Manipulation
  As a trading client
  I want to manipulate and transform trading data on the server
  So that I receive only the structured subset of data I need

  Scenario: Filtering trades by symbol
    Given the server is running
    And a manipulation service is available
    When I send a MANIPULATE request to filter trades by symbol "AAPL"
    Then I receive a successful response
    And the response payload contains only trades for "AAPL"

  Scenario: Transforming trade records into a pivot summary
    Given the server is running
    And a manipulation service is available
    When I send a MANIPULATE transform request for a daily pivot
    Then I receive a successful response
    And the response payload contains the pivoted summary

  Scenario: Manipulation with unsupported operation returns an error
    Given the server is running
    And a manipulation service is available
    When I send a MANIPULATE request with an unsupported operation code
    Then I receive a failure response
    And the response message indicates the operation is not supported
