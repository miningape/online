#ifndef CLIENT
#define CLIENT

#include <asio.hpp>
#include <json.hpp>
#include <queue>

#include "../helpers/message.hpp"
#include "../helpers/helper.hpp"

class client {
    public:
    client( asio::io_context& context, asio::ip::tcp::endpoint endpoint, std::queue<nlohmann::json> &queue_) 
        : io_context( context ), socket(context), queue(queue_) {
        std::error_code ec;
        
        socket.connect(endpoint, ec);
        msg = server_message();

        if ( !ec ) {
            prime_read_header();
        } else {
            logError( ec.message() );
        }
    }

    void close() {
        std::error_code ec;

        socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
        
        if ( !ec ) {
            log("closed connection");
        } else {
            logError( ec.message() );
        }
    }

    void send( server_message message ) {
        asio::async_write( socket, message.rawBuffer(),
        [this, message](std::error_code ec, std::size_t length) {
            if (!ec) log("Sent: " + message.value() );
            else logError( ec.message() );
        } );
    }

    /*void close() {
        asio::post( io_context, [&]() { log("Closing Client"); socket.close(); } );
    }*/

    private:
    void prime_read_header() {
        msg = server_message();

        asio::async_read( socket, msg.headBuffer(),
        [this]( std::error_code ec, std::size_t length ){
            //log("Reading Header");

            if ( !ec ) {
                // Must be run if you want values before entire object is run
                
                //msg.update_head();

                //log("Recived: " + std::to_string(length) + " : " + std::to_string( msg.body_size ) );
                prime_read_body();
            } else {
                socket.close(ec);
                io_context.stop();
 
                logError( "Head: " + ec.message() );
            }
        } );
    }

    void prime_read_body( ) {
        asio::async_read( socket, msg.bodyBuffer(),
        [this]( std::error_code ec, std::size_t length ){
            // log("Reading Body");
            if ( !ec ) {
                //std::string s(data);
                log("Recived: " + std::to_string(length) + " : "  + msg.value() );
                nlohmann::json j_msg = nlohmann::json::parse(msg.value());
                
                queue.push(j_msg);

                //log(std::string(msg->raw.begin(), msg->raw.end()));
                prime_read_header();
            } else {
                logError( "Body: " + ec.message() );
            }
        } );
    }

    asio::io_context& io_context;
    asio::ip::tcp::socket socket;
    
    char data[4];
    std::queue<nlohmann::json> &queue;

    server_message msg;
};

#endif