#include "server.hpp"

int main(int argc, char* argv[]) {
  // Check command line arguments.
  if (argc != 4) {
    fmt::print(
        "Usage: coroutine-server <address> <port> <threads>\n"
        "Example:\n"
        "    coroutine-server 127.0.0.1 80 1\n");
    return EXIT_FAILURE;
  }

  Server(boost::asio::ip::make_address(argv[1]),
         static_cast<ushort>(std::stoi(argv[2])),
         std::max(1, std::stoi(argv[3])))
      .StartPolling();
}
