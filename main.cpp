#include <fmt/core.h>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/experimental/as_tuple.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include <coroutine>
#include <ranges>
#include <thread>

namespace beast = boost::beast;

namespace asio {
using namespace boost::asio;
using boost::asio::experimental::as_tuple;
}

namespace ip = asio::ip;
using tcp = ip::tcp;
namespace http = beast::http;


template <http::status Code, typename Body = http::empty_body>
asio::awaitable<void> respond(beast::tcp_stream& stream, auto... Args) {
  http::response<Body> response;

  response.result(Code);
  if constexpr (std::same_as<Body, http::string_body>) {
    response.body() = std::get<0>(std::forward_as_tuple(Args...));
  }
  response.prepare_payload();

  co_await http::async_write(stream, response, asio::use_awaitable);
}

asio::awaitable<void> handle_request(beast::tcp_stream& stream,
                                     http::request<http::string_body> request) {
  if (request.method() == http::verb::get) {
    co_return co_await respond<http::status::ok, http::string_body>(
        stream, request.body());
  }

  co_await respond<http::status::bad_request>(stream);
}

asio::awaitable<void> poll_socket(tcp::socket socket) {
  http::request<http::string_body> request;
  beast::tcp_stream stream{std::move(socket)};
  beast::flat_buffer buffer;

  for (;;) {
    auto [ec, size] = co_await http::async_read(
        stream, buffer, request, asio::as_tuple(asio::use_awaitable));
    if (ec) {
      if (ec == http::error::end_of_stream) {
        break;
      }
      fmt::print("{}\n", ec.message());
      break;
    }

    bool close{request.need_eof()};

    co_await handle_request(stream, std::move(request));

    if (close) {
      break;
    }

    request = {};
  }

  stream.socket().shutdown(tcp::socket::shutdown_send);
}

asio::awaitable<void> poll_connections(asio::ip::address address, ushort port,
                                       asio::io_context& ioc) {
  tcp::endpoint endpoint{address, port};

  tcp::acceptor acceptor{ioc};
  acceptor.open(endpoint.protocol());
  acceptor.bind(endpoint);
  acceptor.listen();

  tcp::socket socket{ioc};

  for (;;) {
    co_await acceptor.async_accept(socket, asio::use_awaitable);
    co_await poll_socket(std::move(socket));
    socket = tcp::socket(ioc);
  }
}

void start_polling(std::string_view address, ushort port,
                   int concurrency_hint) {
  asio::io_context ioc{concurrency_hint};
  asio::co_spawn(ioc.get_executor(),
                 poll_connections(ip::make_address(address), port, ioc),
                 asio::detached);

  // Run the I/O service on the requested number of threads
  std::vector<std::thread> v;
  v.reserve(concurrency_hint - 1);
  for (int i : std::views::iota(1, concurrency_hint)) {
    v.emplace_back([&ioc] { ioc.run(); });
  }

  ioc.run();
}

int main(int argc, char* argv[]) {
  // Check command line arguments
  if (argc != 4) {
    fmt::print(
        "Usage: coroutine-server <address> <port> <threads>\n"
        "Example:\n"
        "    coroutine-server 127.0.0.1 80 1\n");
    return EXIT_FAILURE;
  }

  start_polling(argv[1], static_cast<ushort>(std::stoi(argv[2])),
                std::max(1, std::stoi(argv[3])));
}
