#include <asio.hpp>
#include <memory>
#include <string>

#include "../helper.hpp"
#include "../message.hpp"



class connection : public std::enable_shared_from_this<connection> {
    public:
    connection( asio::ip::tcp::socket sock ) 
    : socket( std::move(sock) )  {
        data = "69";
        msg = new server_message();
        msg->insert("69");
    }

    void send( server_message* message ) {
        auto self(shared_from_this());
        int size = message->size();
        socket.async_send( asio::buffer( message->raw.data(),  size), 
        [this, self, message]( std::error_code ec, std::size_t length ) {
            std::string str(message->body());

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

    server_message* msg;
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
                
                //server_message* me;

                server_message* m1 = new server_message();
                server_message* m2 = new server_message();
                m1->insert("101");
                m2->insert("sexy beans");

                x->read();
                x->send( m1 );
                x->send( m2 );
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