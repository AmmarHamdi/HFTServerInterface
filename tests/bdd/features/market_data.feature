Feature: Market Data
  As a trading client
  I want to retrieve and subscribe to market data
  So that I can make informed trading decisions

  Scenario: Fetching market data for a symbol
    Given the server is running
    And a market data service is available
    When I send a GET_MARKET_DATA request for symbol "AAPL"
    Then I receive a successful response
    And the response payload contains data for "AAPL"

  Scenario: Subscribing to a symbol for real-time updates
    Given the server is running
    And a market data service is available
    When I subscribe to symbol "EURUSD"
    Then the subscription is acknowledged

  Scenario: Unsubscribing from a symbol
    Given the server is running
    And I am already subscribed to symbol "EURUSD"
    When I unsubscribe from symbol "EURUSD"
    Then the unsubscription is acknowledged

  Scenario: Requesting data for an unknown symbol returns an error
    Given the server is running
    And a market data service is available
    When I send a GET_MARKET_DATA request for symbol "UNKNOWN"
    Then I receive a failure response
    And the response message indicates the symbol was not found
