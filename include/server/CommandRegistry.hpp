/**
 * @file CommandRegistry.hpp
 * @brief Maps RequestType enum values to command factory functions.
 *
 * @details The CommandRegistry owns a map of CommandFactory callables keyed
 * by RequestType. The TradingServerFacade calls create() to obtain a
 * ready-to-execute ICommand instance for every incoming request.
 */

#ifndef COMMANDREGISTRY_HPP
#define COMMANDREGISTRY_HPP

#include <functional>
#include <memory>
#include <unordered_map>

#include "ICommand.hpp"
#include "RequestTypes.hpp"
#include "models/Request.hpp"

/**
 * @brief Factory function type that produces an ICommand from a Request.
 */
using CommandFactory = std::function<std::unique_ptr<ICommand>(const Request&)>;

/**
 * @class CommandRegistry
 * @brief Registry that maps RequestType values to CommandFactory functions.
 *
 * Register all command factories during application startup (typically in
 * main() or a dedicated bootstrap function). At runtime, the facade calls
 * create() to instantiate the appropriate command for each request.
 */
class CommandRegistry
{
public:
    /// @brief Default constructor.
    CommandRegistry() = default;

    /**
     * @brief Register a factory function for the given request type.
     * @param type    The request type to associate with the factory.
     * @param factory A callable that constructs the corresponding ICommand.
     *
     * // TODO: REGISTER — Call this method for every new command you add.
     *          Example:
     *            registry.registerCommand(RequestType::MY_NEW_TYPE,
     *                [&svc](const Request& r) {
     *                    return std::make_unique<MyNewCommand>(svc, r);
     *                });
     */
    void registerCommand(RequestType type, CommandFactory factory);

    /**
     * @brief Create an ICommand instance for the given request.
     * @param request The incoming client request.
     * @return A unique_ptr to the constructed command.
     * @throws std::out_of_range if no factory is registered for the request type.
     */
    std::unique_ptr<ICommand> create(const Request& request) const;

private:
    /// @brief Internal map from RequestType to CommandFactory.
    std::unordered_map<RequestType, CommandFactory> m_factories;
};

#endif // COMMANDREGISTRY_HPP
