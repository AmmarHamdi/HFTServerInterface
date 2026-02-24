/**
 * @file BoostAsioSslTransport.hpp
 * @brief Concrete ITransport implementation using Boost.Asio with SSL/TLS.
 *
 * @details Provides a TCP server with SSL/TLS encryption for the transport
 * layer. This is the iteration-1 communication stack. Replace this class
 * with an alternative ITransport implementation to switch protocols without
 * touching the server or service layers.
 */

#ifndef BOOSTASIOSSL_TRANSPORT_HPP
#define BOOSTASIOSSL_TRANSPORT_HPP

#include "transport/ITransport.hpp"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <cstdint>
#include <string>

/**
 * @class BoostAsioSslTransport
 * @brief Boost.Asio + SSL/TLS implementation of ITransport.
 *
 * Manages the io_context, SSL context, and the acceptor/socket lifecycle.
 * All public methods are thread-safe when called from the io_context thread.
 */
class BoostAsioSslTransport : public ITransport
{
public:
    /**
     * @brief Construct the transport.
     * @param host      Bind address (e.g., "0.0.0.0").
     * @param port      TCP port to listen on.
     * @param certFile  Path to the PEM-encoded server certificate.
     * @param keyFile   Path to the PEM-encoded private key.
     */
    explicit BoostAsioSslTransport(const std::string& host,
                                   uint16_t           port,
                                   const std::string& certFile,
                                   const std::string& keyFile);

    ~BoostAsioSslTransport() override;

    /// @copydoc ITransport::send
    void send(const RawBuffer& buffer) override;

    /// @copydoc ITransport::receive
    RawBuffer receive() override;

    /// @copydoc ITransport::start
    void start() override;

    /// @copydoc ITransport::stop
    void stop() override;

private:
    std::string m_host;
    uint16_t    m_port;
    std::string m_certFile;
    std::string m_keyFile;

    boost::asio::io_context                    m_ioContext;
    boost::asio::ssl::context                  m_sslContext;
    boost::asio::ip::tcp::acceptor             m_acceptor;
};

#endif // BOOSTASIOSSL_TRANSPORT_HPP
