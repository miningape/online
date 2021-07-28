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
                logError( ec.message() );
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

bool done = false;

void worker( ) {
    using namespace std::literals::chrono_literals;

    while (!done) {
        std::cout << "Worker thread id=" << std::this_thread::get_id() << std::endl;
        std::this_thread::sleep_for(1s);
    }
}

int main() {
    std::error_code ec;

    // Create context
    asio::io_context io_context;
    asio::io_service::work _work(io_context);

    asio::ip::tcp::endpoint endpoint( asio::ip::make_address_v4("127.0.0.1", ec), 80);

    if (!ec) {
        //asio::ip::tcp::socket socket( io_context );

        client *user = new client(io_context, endpoint);

        std::thread network([&io_context](){ io_context.run(); });

        std::thread gameloop( worker );

        std::string input = "";

        while (input != "close") {     
            std::getline(std::cin, input);
            server_message message(input);
            user->send( message );
        }

        // Run Server
    //user.close();
        network.join();
        gameloop.join();


    } else {
        logError( ec.message() );
    }    
    
    return 0;
}