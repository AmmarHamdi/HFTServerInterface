/**
 * @file ICalculationService.hpp
 * @brief Pure virtual interface for financial calculation operations.
 *
 * @details Provides on-demand financial computations such as P&L, VaR,
 * Greeks, or other derived metrics. Implementations retrieve raw data from
 * the database layer and apply the relevant algorithms.
 */

#ifndef ICALCULATIONSERVICE_HPP
#define ICALCULATIONSERVICE_HPP

#include "models/Request.hpp"
#include "models/Response.hpp"

/**
 * @class ICalculationService
 * @brief Abstract interface for executing financial calculations.
 */
class ICalculationService
{
public:
    /// @brief Virtual destructor to ensure proper cleanup of derived classes.
    virtual ~ICalculationService() = default;

    /**
     * @brief Perform a financial calculation based on the request parameters.
     * @param request Contains the calculation type and input data payload.
     * @return A Response with the computed result serialized in the payload.
     */
    virtual Response calculate(const Request& request) = 0;

    // TODO: EXTEND — Add new calculation methods here (e.g., calculateVaR(),
    //               calculateGreeks(), calculateImpliedVolatility()) as new
    //               quantitative models are introduced to the system.
};

#endif // ICALCULATIONSERVICE_HPP
