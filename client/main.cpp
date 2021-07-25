#include <asio.hpp>
#include <string>
#include <memory>

#include "../helper.hpp"
#include "../message.hpp"

class client {
    public:
    client( asio::io_context& context, asio::ip::tcp::endpoint endpoint ) 
        : io_context( context ), socket(context) {
        std::error_code ec;
        
        socket.connect(endpoint, ec);
        msg = server_message();

        if ( !ec ) {
            prime_read_header();
        } else {
            logError( ec.message() );
        }
    }

    /*void close() {
        asio::post( io_context, [&]() { log("Closing Client"); socket.close(); } );
    }*/

    private:
    void prime_read_header() {
        msg = server_message();

        asio::async_read( socket, msg.headBuffer(),
        [this]( std::error_code ec, std::size_t length ){
            log("Reading Header");

            if ( !ec ) {
                // Must be run if you want values before entire object is run
                msg.update_head();

                log("Recived: " + std::to_string(length) + " : " + std::to_string( msg.body_size ) );
                prime_read_body();
            } else {
                logError( ec.message() );
            }
        } );
    }

    void prime_read_body( ) {
        asio::async_read( socket, msg.bodyBuffer(),
        [this]( std::error_code ec, std::size_t length ){
            log("Reading Body");
            if ( !ec ) {
                //std::string s(data);
                log("Recived: " + std::to_string(length) + " : "  + msg.value() );
                //log(std::string(msg->raw.begin(), msg->raw.end()));
                prime_read_header();
            } else {
                logError( ec.message() );
            }
        } );
    }

    asio::io_context& io_context;
    asio::ip::tcp::socket socket;
    
    char data[4];

    server_message msg;
};

int main() {
    std::error_code ec;

    // Create context
    asio::io_context io_context;
    asio::io_service::work _work(io_context);


    asio::ip::tcp::endpoint endpoint( asio::ip::make_address_v4("127.0.0.1", ec), 80);

    if (!ec) {
        //asio::ip::tcp::socket socket( io_context );

        client user(io_context, endpoint);

        std::thread thread([&io_context](){ io_context.run(); });

        
        std::string input = "notnull";

        std::cin >> input;


        // Run Server
    //user.close();
    if (thread.joinable()) thread.join();


    } else {
        logError( ec.message() );
    }    
    
    return 0;
}