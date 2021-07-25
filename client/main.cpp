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
        asio::async_read( socket, asio::buffer( msg.raw.data(), msg.header_size ),
        [this]( std::error_code ec, std::size_t length ){
            log("Reading Header");
            if ( !ec ) {
                log("Recived: " + std::to_string(length) + " : " + std::to_string( msg.header() ) );
                prime_read_body(msg.header());
            } else {
                logError( ec.message() );
            }
        } );
    }

    void prime_read_body( size_t size ) {
        asio::async_read( socket, asio::buffer(msg.raw.data() + msg.header_size, size),
        [this]( std::error_code ec, std::size_t length ){
            log("Reading Body");
            if ( !ec ) {
                std::string s(data);
                log("Recived: " + std::to_string(length) + " : "  + msg.body() );
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