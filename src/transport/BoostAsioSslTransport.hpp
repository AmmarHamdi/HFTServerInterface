/**
 * @file BoostAsioSslTransport.hpp
 * @brief Concrete ITransport implementation using Boost.Asio with SSL/TLS.
 *
 * @details Provides a TCP server with SSL/TLS encryption for the transport
 * layer. This is the iteration-1 communication stack. Replace this class
 * with an alternative ITransport implementation to switch protocols without
 * touching the server or service layers.
 *
 * ### Wire protocol (length-prefix framing)
 * Every message is framed as:
 *   [ 4 bytes big-endian uint32_t payload length ][ <length> bytes payload ]
 *
 * ### Connection model
 * The server accepts one client connection at a time. After a client
 * disconnects (or an error occurs) `acceptNextConnection()` is re-invoked
 * automatically so the server is always ready for the next client.
 */

#ifndef BOOSTASIOSSL_TRANSPORT_HPP
#define BOOSTASIOSSL_TRANSPORT_HPP

#include "transport/ITransport.hpp"

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

/**
 * @class BoostAsioSslTransport
 * @brief Boost.Asio + SSL/TLS implementation of ITransport.
 *
 * Manages the io_context, SSL context, and the acceptor/socket lifecycle.
 * `start()` runs the io_context on a dedicated background thread.
 * `send()` and `receive()` operate on the currently active SSL socket.
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
    using SslSocket = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;

    /// Begin an async accept cycle; re-invoked after each connection.
    void acceptNextConnection();

    /// Perform the async SSL handshake then mark the socket as active.
    void doHandshake(std::shared_ptr<SslSocket> socket);

    std::string  m_host;
    uint16_t     m_port;
    std::string  m_certFile;
    std::string  m_keyFile;

    boost::asio::io_context        m_ioContext;
    boost::asio::ssl::context      m_sslContext;
    boost::asio::ip::tcp::acceptor m_acceptor;

    /// Work guard keeps io_context alive even when there are no pending ops.
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_workGuard;

    /// Background thread that drives the io_context event loop.
    std::thread m_ioThread;

    /// The currently active (connected + handshaked) SSL socket.
    std::shared_ptr<SslSocket> m_activeSocket;

    /// Protects m_activeSocket from concurrent access by send/receive.
    std::mutex m_socketMutex;

    std::atomic<bool> m_running{false};
};

#endif // BOOSTASIOSSL_TRANSPORT_HPP
