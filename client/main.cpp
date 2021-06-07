#include <asio.hpp>
#include <string>

#include "../helper.hpp"


class client {
    public:
    client( asio::io_context& context, asio::ip::tcp::endpoint endpoint ) 
        : io_context( context ), socket(context) {
        std::error_code ec;
        

        socket.connect( endpoint );
        

        if ( !ec ) {
            prime_read();
        } else {
            logError( ec.message() );
        }
    }

    /*void close() {
        asio::post( io_context, [&]() { log("Closing Client"); socket.close(); } );
    }*/

    private:
    void prime_read() {
        socket.async_read_some( asio::buffer( data.data(), 4 ),
        [this]( std::error_code ec, std::size_t length ){
            log("Reading");
            if ( !ec ) {
                log("Recived:" + std::to_string(length) + " : " + std::to_string(std::stoi(data)) );
                prime_read();
            } else {
                logError( ec.message() );
                //close();
            }
        } );
    }

    asio::io_context& io_context;
    asio::ip::tcp::socket socket;
    
    std::string data;
};

int main() {
    std::error_code ec;

    // Create context
    asio::io_context io_context;

    asio::ip::tcp::endpoint endpoint( asio::ip::make_address_v4("127.0.0.1", ec), 80);

    if (!ec) {
        asio::ip::tcp::socket socket( io_context );

        client user(io_context, endpoint);

        std::thread thread([&io_context](){ io_context.run(); });

        
        std::string input = "null";

        std::cin >> input;


        // Run Server
    //user.close();
    if (thread.joinable()) thread.join();


    } else {
        logError( ec.message() );
    }    
    
    return 0;
}