#include <asio.hpp>
#include <memory>
#include <string>

#include "../helpers/helper.hpp"
#include "../helpers/message.hpp"

#include <concurrent_queue.h>

#include "ball.hpp"
#include "server.cpp"

int main() {

    log("Started");
    
    try {
    // Create context
    asio::io_context io_context;
    

    // Create endpoint for server to listen on
    asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), 80);

    server server_( 80 );

    // Run Server
    //std::thread server_t([&io_context](){ io_context.run(); });
 
    /*std::thread ball_t([&server_](){ 
        using namespace std::literals::chrono_literals;
        bool running = true;

        log("Creating Ball");
        ball ball_;


        while (running) {

            ball_.update();

            server_.sendall( server_message( ball_.getPosJSON().dump() ) );
            
            std::this_thread::sleep_for( (1/2)*1s );
        }
    });*/

    using namespace std::literals::chrono_literals;
        bool running = true;

        log("Creating Ball");
        ball ball_;


        while (running) {

            ball_.update();

            server_.sendall( server_message( ball_.getPosJSON().dump() ) );
            
            std::this_thread::sleep_for( 0.05s );
        }


    }catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

    return 0;
}