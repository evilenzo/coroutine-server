#pragma once

#include <fmt/core.h>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/experimental/as_tuple.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include <coroutine>
#include <thread>

class Server {
  template <typename T>
  using awaitable_t = boost::asio::awaitable<T>;

  template <typename T>
  using request_t = boost::beast::http::request<T>;

  using address_t = boost::asio::ip::address;
  using context_t = boost::asio::io_context;
  using socket_t = boost::asio::ip::tcp::socket;
  using stream_t = boost::beast::tcp_stream;
  using string_body_t = boost::beast::http::string_body;

 public:
  Server(address_t address, ushort port, int concurrency_hint);

  void StartPolling();

 private:
  awaitable_t<void> poll_connections(context_t& ioc);
  awaitable_t<void> poll_socket(socket_t socket);
  awaitable_t<void> handle_request(stream_t& stream,
                                   request_t<string_body_t> request);

 private:
  address_t m_address;
  ushort m_port;
  int m_concurrency_hint;
};
