#include <asio.hpp>
#include <memory>
#include <string>

#include "../helper.hpp"
#include "../message.hpp"



class connection : public std::enable_shared_from_this<connection> {
    public:
    connection( asio::ip::tcp::socket sock ) 
    : socket( std::move(sock) )  {}

    void send( server_message message ) {
        auto self(shared_from_this());
        socket.async_send( message.rawBuffer(), 
        [this, self, message]( std::error_code ec, std::size_t length ) {
            std::string str = message.value();

            if (!ec) log("Sent: " + str );

        
            else logError( ec.message() );
            //send();
            //read();
        } );
    }

    void read_head() {
        input_msg.clear();
        auto self(shared_from_this());

        asio::async_read( socket, input_msg.headBuffer(),
        [this, self] (std::error_code ec, std::size_t length) {
            if (!ec)
          {
              input_msg.update_head();
              log( "Recieved Header: " + input_msg.body_size );
              read_body();
          } else {
              logError( ec.message() );
          }
        } );
    }

    void read_body() {
        auto self(shared_from_this());

        asio::async_read( socket, input_msg.bodyBuffer(),
        [this, self] (std::error_code ec, std::size_t length) {
            if (!ec)
          {
              log( "Recieved Body: " + input_msg.value() );
              read_head();
          } else {
              logError( ec.message() );
          }
        });
    }

    private:
    asio::ip::tcp::socket socket;
    server_message input_msg;


    char user[4];
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

                server_message m1("I Love ");
                server_message m2("My ");

                m2.clear();
                m2.append("Hong Miong Mao");

                x->read_head();
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