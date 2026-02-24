Feature: Financial Calculations
  As a trading client
  I want to run financial calculations on the server
  So that I can evaluate positions and risk without client-side computation

  Scenario: Calculating P&L for a given position
    Given the server is running
    And a calculation service is available
    When I send a CALCULATE request with a position payload
    Then I receive a successful response
    And the response payload contains the P&L result

  Scenario: Calculating VaR for a portfolio
    Given the server is running
    And a calculation service is available
    When I send a CALCULATE request with a portfolio payload
    Then I receive a successful response
    And the response payload contains the VaR estimate

  Scenario: Calculation with invalid input returns an error
    Given the server is running
    And a calculation service is available
    When I send a CALCULATE request with an empty payload
    Then I receive a failure response
    And the response message describes the validation error
