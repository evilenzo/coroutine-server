#include "server.hpp"

namespace beast = boost::beast;
namespace http = beast::http;

namespace net {
using namespace boost::asio;
using boost::asio::experimental::as_tuple;
}

using tcp = boost::asio::ip::tcp;



Server::Server(address_t address, ushort port, int concurrency_hint)
    : m_address(std::move(address)),
      m_port(port),
      m_concurrency_hint(concurrency_hint) {}

void Server::StartPolling() {
  net::io_context ioc{m_concurrency_hint};
  net::co_spawn(ioc.get_executor(), poll_connections(ioc), net::detached);

  // Run the I/O service on the requested number of threads
  std::vector<std::thread> v;
  v.reserve(m_concurrency_hint - 1);
  for (auto i = m_concurrency_hint - 1; i > 0; --i) {
    v.emplace_back([&ioc] { ioc.run(); });
  }
  ioc.run();
}

auto Server::poll_connections(context_t& ioc) -> awaitable_t<void> {
  tcp::endpoint endpoint{m_address, m_port};

  tcp::acceptor acceptor{ioc};
  acceptor.open(endpoint.protocol());
  acceptor.bind(endpoint);
  acceptor.listen();

  tcp::socket socket{ioc};

  for (;;) {
    co_await acceptor.async_accept(socket, net::use_awaitable);
    co_await poll_socket(std::move(socket));
    socket = tcp::socket(ioc);
  }
}

auto Server::poll_socket(socket_t socket) -> awaitable_t<void> {
  http::request<http::string_body> request;
  beast::tcp_stream stream{std::move(socket)};
  beast::flat_buffer buffer;

  for (;;) {
    request = {};
    auto [ec, size] = co_await http::async_read(
        stream, buffer, request, net::as_tuple(net::use_awaitable));
    if (ec == http::error::end_of_stream) {
      break;
    } else if (ec) {
      fmt::print("{}\n", ec.message());
    }

    bool close{request.need_eof()};

    co_await handle_request(stream, std::move(request));

    if (close) {
      break;
    }
  }

  stream.socket().shutdown(tcp::socket::shutdown_send);
}

auto Server::handle_request(stream_t& stream, request_t<string_body_t> request)
    -> awaitable_t<void> {
  http::response<http::string_body> response;
  response.set(http::field::version, BOOST_BEAST_VERSION_STRING);

  switch (request.method()) {
    case http::verb::get: {
      response.result(http::status::ok);
      response.body() = request.body();
      break;
    }
    default:
      response.result(http::status::bad_request);
  }

  response.prepare_payload();
  co_await http::async_write(stream, response, net::use_awaitable);
}
