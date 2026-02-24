/**
 * @file CommandRegistry.cpp
 * @brief Implementation of CommandRegistry — maps RequestType to CommandFactory.
 */

#include "server/CommandRegistry.hpp"

#include <stdexcept>

void CommandRegistry::registerCommand(RequestType type, CommandFactory factory)
{
    m_factories[type] = std::move(factory);

    // TODO: REGISTER — Register your new command factory here.
    //       This method is called from the application bootstrap code (main.cpp
    //       or a dedicated wiring function). Example:
    //
    //         registry.registerCommand(
    //             RequestType::MY_NEW_TYPE,
    //             [&myService](const Request& r) {
    //                 return std::make_unique<MyNewCommand>(myService, r);
    //             });
}

std::unique_ptr<ICommand> CommandRegistry::create(const Request& request) const
{
    auto it = m_factories.find(request.type);
    if (it == m_factories.end())
    {
        throw std::out_of_range("No command registered for the given RequestType");
    }
    return it->second(request);
}
