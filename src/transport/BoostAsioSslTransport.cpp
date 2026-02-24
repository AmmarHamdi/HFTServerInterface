/**
 * @file BoostAsioSslTransport.cpp
 * @brief Implementation of the Boost.Asio + SSL/TLS transport layer.
 *
 * @details Implements a length-prefix framed TCP server over SSL/TLS.
 * Each message on the wire is:
 *   [ 4-byte big-endian uint32_t length ][ <length> bytes payload ]
 *
 * The io_context runs on a dedicated background thread started by start().
 * send() and receive() perform synchronous writes / reads on the active
 * SSL socket and are safe to call from any thread after start() returns.
 */

#include "BoostAsioSslTransport.hpp"

#include <array>
#include <cstring>
#include <iostream>
#include <stdexcept>

namespace
{
    /// Encode a 32-bit value as 4 big-endian bytes.
    std::array<uint8_t, 4> encodeBE32(uint32_t value)
    {
        return {{
            static_cast<uint8_t>((value >> 24) & 0xFF),
            static_cast<uint8_t>((value >> 16) & 0xFF),
            static_cast<uint8_t>((value >>  8) & 0xFF),
            static_cast<uint8_t>( value        & 0xFF)
        }};
    }

    /// Decode 4 big-endian bytes into a 32-bit value.
    uint32_t decodeBE32(const uint8_t* bytes)
    {
        return (static_cast<uint32_t>(bytes[0]) << 24)
             | (static_cast<uint32_t>(bytes[1]) << 16)
             | (static_cast<uint32_t>(bytes[2]) <<  8)
             |  static_cast<uint32_t>(bytes[3]);
    }
} // namespace

// ==========================================================================
// Constructor
// ==========================================================================

BoostAsioSslTransport::BoostAsioSslTransport(const std::string& host,
                                             uint16_t           port,
                                             const std::string& certFile,
                                             const std::string& keyFile)
    : m_host(host)
    , m_port(port)
    , m_certFile(certFile)
    , m_keyFile(keyFile)
    , m_ioContext()
    , m_sslContext(boost::asio::ssl::context::tls_server)
    , m_acceptor(m_ioContext)
    , m_workGuard(boost::asio::make_work_guard(m_ioContext))
{
    // ── SSL context configuration ──────────────────────────────────────────
    m_sslContext.set_options(
        boost::asio::ssl::context::default_workarounds |
        boost::asio::ssl::context::no_sslv2            |
        boost::asio::ssl::context::no_sslv3            |
        boost::asio::ssl::context::single_dh_use);

    m_sslContext.use_certificate_chain_file(m_certFile);
    m_sslContext.use_private_key_file(m_keyFile, boost::asio::ssl::context::pem);
}

// ==========================================================================
// Destructor
// ==========================================================================

BoostAsioSslTransport::~BoostAsioSslTransport()
{
    stop();
}

// ==========================================================================
// start() — bind, listen, launch io_context thread, begin accept loop
// ==========================================================================

void BoostAsioSslTransport::start()
{
    if (m_running.exchange(true))
        return; // already started

    // ── Open and configure the acceptor ───────────────────────────────────
    const boost::asio::ip::tcp::endpoint endpoint(
        boost::asio::ip::make_address(m_host), m_port);

    m_acceptor.open(endpoint.protocol());
    m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    m_acceptor.bind(endpoint);
    m_acceptor.listen(boost::asio::socket_base::max_listen_connections);

    std::cout << "[BoostAsioSslTransport] Listening on "
              << m_host << ":" << m_port << "\n";

    // ── Start accepting connections ────────────────────────────────────────
    acceptNextConnection();

    // ── Run io_context on a dedicated background thread ───────────────────
    m_ioThread = std::thread([this]() {
        try
        {
            m_ioContext.run();
        }
        catch (const std::exception& ex)
        {
            std::cerr << "[BoostAsioSslTransport] io_context error: "
                      << ex.what() << "\n";
        }
    });
}

// ==========================================================================
// stop() — graceful shutdown
// ==========================================================================

void BoostAsioSslTransport::stop()
{
    if (!m_running.exchange(false))
        return; // already stopped

    // Close acceptor so no new connections are accepted.
    boost::system::error_code ec;
    m_acceptor.close(ec);

    // Shut down the active socket if one exists.
    {
        std::lock_guard<std::mutex> lock(m_socketMutex);
        if (m_activeSocket)
        {
            m_activeSocket->lowest_layer().cancel(ec);
            m_activeSocket->lowest_layer().close(ec);
            m_activeSocket.reset();
        }
    }

    // Release the work guard so io_context::run() can return.
    m_workGuard.reset();
    m_ioContext.stop();

    if (m_ioThread.joinable())
        m_ioThread.join();

    std::cout << "[BoostAsioSslTransport] Stopped.\n";
}

// ==========================================================================
// send() — length-prefix frame then write payload to active SSL socket
// ==========================================================================

void BoostAsioSslTransport::send(const RawBuffer& buffer)
{
    std::lock_guard<std::mutex> lock(m_socketMutex);

    if (!m_activeSocket)
        throw std::runtime_error("[BoostAsioSslTransport] send(): no active connection");

    // Build the 4-byte length header + payload in a single gather-write.
    const auto header = encodeBE32(static_cast<uint32_t>(buffer.size()));

    const std::array<boost::asio::const_buffer, 2> bufs = {{
        boost::asio::buffer(header),
        boost::asio::buffer(buffer)
    }};

    boost::system::error_code ec;
    boost::asio::write(*m_activeSocket, bufs, ec);

    if (ec)
        throw std::runtime_error(
            "[BoostAsioSslTransport] send() failed: " + ec.message());
}

// ==========================================================================
// receive() — read length-prefix header then payload from active SSL socket
// ==========================================================================

RawBuffer BoostAsioSslTransport::receive()
{
    std::lock_guard<std::mutex> lock(m_socketMutex);

    if (!m_activeSocket)
        throw std::runtime_error("[BoostAsioSslTransport] receive(): no active connection");

    // ── Read the 4-byte length header ──────────────────────────────────────
    std::array<uint8_t, 4> header{};
    boost::system::error_code ec;

    boost::asio::read(*m_activeSocket,
                      boost::asio::buffer(header),
                      boost::asio::transfer_exactly(4),
                      ec);
    if (ec)
        throw std::runtime_error(
            "[BoostAsioSslTransport] receive() header read failed: " + ec.message());

    const uint32_t payloadLen = decodeBE32(header.data());

    if (payloadLen == 0)
        return {};

    // ── Read the payload ───────────────────────────────────────────────────
    RawBuffer payload(payloadLen);
    boost::asio::read(*m_activeSocket,
                      boost::asio::buffer(payload),
                      boost::asio::transfer_exactly(payloadLen),
                      ec);
    if (ec)
        throw std::runtime_error(
            "[BoostAsioSslTransport] receive() payload read failed: " + ec.message());

    return payload;
}

// ==========================================================================
// acceptNextConnection() — async accept → SSL handshake → set active socket
// ==========================================================================

void BoostAsioSslTransport::acceptNextConnection()
{
    auto socket = std::make_shared<SslSocket>(m_ioContext, m_sslContext);

    m_acceptor.async_accept(
        socket->lowest_layer(),
        [this, socket](const boost::system::error_code& ec)
        {
            if (ec)
            {
                if (ec != boost::asio::error::operation_aborted)
                    std::cerr << "[BoostAsioSslTransport] accept error: "
                              << ec.message() << "\n";
                return; // acceptor was closed — do not re-arm
            }

            std::cout << "[BoostAsioSslTransport] Accepted connection from "
                      << socket->lowest_layer().remote_endpoint() << "\n";

            // Disable Nagle for low-latency HFT traffic.
            socket->lowest_layer().set_option(
                boost::asio::ip::tcp::no_delay(true));

            doHandshake(socket);
        });
}

// ==========================================================================
// doHandshake() — async TLS server handshake, then promote to active socket
// ==========================================================================

void BoostAsioSslTransport::doHandshake(std::shared_ptr<SslSocket> socket)
{
    socket->async_handshake(
        boost::asio::ssl::stream_base::server,
        [this, socket](const boost::system::error_code& ec)
        {
            if (ec)
            {
                std::cerr << "[BoostAsioSslTransport] TLS handshake failed: "
                          << ec.message() << "\n";
                // Re-arm accept so the server keeps running.
                if (m_running)
                    acceptNextConnection();
                return;
            }

            std::cout << "[BoostAsioSslTransport] TLS handshake complete.\n";

            {
                std::lock_guard<std::mutex> lock(m_socketMutex);
                m_activeSocket = socket;
            }

            // Re-arm accept so the server is ready for the next client
            // once the current one disconnects.
            if (m_running)
                acceptNextConnection();
        });
}
