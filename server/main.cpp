#include <asio.hpp>
#include <memory>
#include <string>

#include "../helpers/helper.hpp"
#include "../helpers/message.hpp"

#include "server.cpp"

int main() {
    log("Started");
    try {
    // Create context
    asio::io_context io_context;
    

    // Create endpoint for server to listen on
    asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), 80);

    server server_(io_context, endpoint);

    // Run Server
    io_context.run();

    }catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

    return 0;
}