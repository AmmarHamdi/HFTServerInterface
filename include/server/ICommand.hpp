/**
 * @file ICommand.hpp
 * @brief Pure virtual interface for the Command pattern.
 *
 * @details Each concrete command encapsulates a single business operation
 * (e.g., fetch market data, run a calculation). Commands are created by
 * the CommandRegistry and executed by the TradingServerFacade.
 */

#ifndef ICOMMAND_HPP
#define ICOMMAND_HPP

#include "models/Response.hpp"

/**
 * @class ICommand
 * @brief Abstract command interface representing a single server operation.
 *
 * Implement this interface for each request type. The command constructor
 * should receive the required service and request parameters so that
 * execute() is fully self-contained.
 */
class ICommand
{
public:
    /// @brief Virtual destructor to ensure proper cleanup of derived classes.
    virtual ~ICommand() = default;

    /**
     * @brief Execute the command and return the result.
     * @return A Response containing the operation result or an error.
     */
    virtual Response execute() = 0;
};

#endif // ICOMMAND_HPP
