#include <asio.hpp>
#include <memory>
#include <string>

#include "../helper.hpp"

class connection : public std::enable_shared_from_this<connection> {
    public:
    connection( asio::ip::tcp::socket sock ) 
    : socket( std::move(sock) )  {
        data = std::to_string(69);
    }

    void send( ) {
        auto self(shared_from_this());
        socket.async_send( asio::buffer( data.data(), 4 ), 
        [this, self]( std::error_code ec, std::size_t length ) {
            if (!ec) log("Sent: " + data );
            else logError( ec.message() );
        } );
    }

    private:
    asio::ip::tcp::socket socket;

    std::string data;

};

class server {
    public:
    server( asio::io_context& context, asio::ip::tcp::endpoint& endpoint ) 
        : acceptor(context, endpoint) {
        prime_accept();
    }

    private:
    void prime_accept() {
        acceptor.async_accept( [this](std::error_code ec, asio::ip::tcp::socket socket) {
            if ( !ec ) {
                log("Accepted Connection");
                
                std::make_shared<connection>( std::move( socket ) )->send();
            } else {
                log("Failed Connection: " + ec.value());
            }

            prime_accept();
        } );
    }

    
    asio::ip::tcp::acceptor acceptor;
};

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