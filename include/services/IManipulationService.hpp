/**
 * @file IManipulationService.hpp
 * @brief Pure virtual interface for data manipulation and transformation.
 *
 * @details Provides operations to filter, transform, and enrich trading data
 * (e.g., normalisation, aggregation, field projection). Implementations
 * operate on in-memory datasets retrieved from the database layer.
 */

#ifndef IMANIPULATIONSERVICE_HPP
#define IMANIPULATIONSERVICE_HPP

#include "models/Request.hpp"
#include "models/Response.hpp"

/**
 * @class IManipulationService
 * @brief Abstract interface for trading data manipulation operations.
 */
class IManipulationService
{
public:
    /// @brief Virtual destructor to ensure proper cleanup of derived classes.
    virtual ~IManipulationService() = default;

    /**
     * @brief Apply a manipulation operation (e.g., filter, aggregate) to data.
     * @param request Contains the operation type and the data payload.
     * @return A Response with the manipulated data in the payload.
     */
    virtual Response manipulate(const Request& request) = 0;

    /**
     * @brief Apply a structural transformation to the data (e.g., pivot, map).
     * @param request Contains the transformation spec and the data payload.
     * @return A Response with the transformed data in the payload.
     */
    virtual Response transform(const Request& request) = 0;

    // TODO: EXTEND — Add new manipulation operations here (e.g., merge(),
    //               sort(), deduplicate()) as data-processing requirements evolve.
};

#endif // IMANIPULATIONSERVICE_HPP
