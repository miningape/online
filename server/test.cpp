#include <iostream>
#include <chrono>
#include <thread>

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

std::vector<char> vBuffer(1 * 1024);

void getData_async(asio::ip::tcp::socket& socket) {
    socket.async_read_some( asio::buffer(vBuffer.data(), vBuffer.size()), 
        [&](std::error_code ec, std::size_t length) {
            if ( !ec ) {
                std::cout << "\n\n READ LENGTH: " << length << std::endl;

                for ( int i = 0; i < length; i++ ) {
                    std::cout << vBuffer[i];
                }

                getData_async( socket );
            }
        } );
}


int main() {
    std::cout << "Server Started" << std::endl;

    asio::error_code ec;

    asio::io_context context;

    asio::io_context::work idleWork(context);

    std::thread threadIOContext = std::thread( [&](){ context.run(); } );


    asio::ip::tcp::endpoint endpoint(asio::ip::make_address("51.38.81.49", ec), 80);

    asio::ip::tcp::socket socket(context);

    socket.connect(endpoint, ec);

    if ( !ec ) {
        std::cout << "We connected" << std::endl;
    } else {
        std::cout << "Error: " << ec.message() << std::endl;
    }

    if ( socket.is_open() ) {
        getData_async( socket );

        std::string request =
            "GET /index.html HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "Connection: close\r\n\r\n";

        socket.write_some(asio::buffer(request.data(), request.size()), ec);


        using namespace std::chrono_literals;
        std::this_thread::sleep_for(20000ms);
    
        context.stop();
        if (threadIOContext.joinable()) threadIOContext.join();
    }




    return 0;
}