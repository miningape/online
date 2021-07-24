#include <asio.hpp>
#include <memory>
#include <string>

#include "../helper.hpp"



class connection : public std::enable_shared_from_this<connection> {
    public:
    connection( asio::ip::tcp::socket sock ) 
    : socket( std::move(sock) )  {
        data = "69";
    }

    void send( ) {
        auto self(shared_from_this());
        socket.async_send( asio::buffer( data, 4 ), 
        [this, self]( std::error_code ec, std::size_t length ) {
            std::string str(data);
            if (!ec) log("Sent: " + str );
            else logError( ec.message() );
            //send();
            //read();
        } );
    }

    void read() {
        log("read");
        auto self(shared_from_this());
        async_read( socket, asio::buffer(user, 4),
        [this, self] (std::error_code ec, std::size_t length) {
            if (!ec)
          {
              std::string str(user);
              log( "Recieved: " + str );
            read();
          } else {
              logError( ec.message() );
          }
        } );
    }

    private:
    asio::ip::tcp::socket socket;

    char* data;
    char* user;

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
                
                std::shared_ptr<connection> x = std::make_shared<connection>( std::move( socket ) );
                
                x->read();
                x->send();
                x->send();
            } else {
                log("Failed Connection: " + ec.message());
            }

            prime_accept();
        } );
    }

    
    asio::ip::tcp::acceptor acceptor;
};

/*
class acceptor {
    public:
    acceptor( asio::io_context& context, asio::ip::tcp::endpoint& endpoint ) 
        : acceptor_asio(context, endpoint) {
        prime_accept();
    }

    private:
    void prime_accept() {
        acceptor_asio.async_accept( [this](std::error_code ec, asio::ip::tcp::socket socket) {
            if ( !ec ) {
                log("Accepted Connection");
                
                std::make_shared<server>( std::move( socket ) )->send();
            } else {
                log("Failed Connection: " + ec.message());
            }

            prime_accept();
        } );
    }

    
    asio::ip::tcp::acceptor acceptor_asio;
    server server_;
};*/

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