#ifndef SERVER_CORE
#define SERVER_CODE

#include <asio.hpp>
#include <memory>
#include <string>

#include "../helpers/helper.hpp"
#include "../helpers/message.hpp"

class server_proto {
    public:
    virtual void sendall( server_message msg )=0;
};

class connection : public std::enable_shared_from_this<connection> {
    public:
    connection( asio::ip::tcp::socket sock, server_proto* master_ ) 
    : socket( std::move(sock) ), master( master_ )  {}

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
              log( "Recieved Header: " + std::to_string(input_msg.body_size) );
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
              master->sendall( input_msg );
              read_head();
              
          } else {
              logError( ec.message() );
          }
        });
    }

    private:
    asio::ip::tcp::socket socket;
    server_message input_msg;
    server_proto* master;


    char user[4];
};

class server : public server_proto {
    public:
    server( asio::io_context& context, asio::ip::tcp::endpoint& endpoint ) 
        : acceptor(context, endpoint) {
        prime_accept();
    }

    void sendall( server_message msg ) {
        for ( std::shared_ptr<connection> c : connections ) {
            c->send( msg );
        }
    }

    private:
    void prime_accept() {
        acceptor.async_accept( [this](std::error_code ec, asio::ip::tcp::socket socket) {
            if ( !ec ) {
                log("Accepted Connection");
                
                std::shared_ptr<connection> x = std::make_shared<connection>( std::move( socket ), this );

                x->read_head();
                connections.push_back(x);

            } else {
                log("Failed Connection: " + ec.message());
            }

            prime_accept();
        } );
    }

    
    asio::ip::tcp::acceptor acceptor;
    std::vector<std::shared_ptr<connection>> connections;
};

#endif