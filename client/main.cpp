#include <asio.hpp>
#include <string>
#include <memory>
#include <json.hpp>
#include <queue>

#include "../helper.hpp"
#include "../message.hpp"

#include "client.cpp"

std::string* _msg = new std::string();

std::queue<nlohmann::json> json_queue;


void worker( ) {
    using namespace std::literals::chrono_literals;

    while(*_msg != "close") {
        std::cout << "Worker thread id=" << std::this_thread::get_id() << ": " << *_msg << std::endl;
        while (!json_queue.empty()) {
            auto j1 = json_queue.front();
            std::cout << "Queue Popped: " << j1["header"] << ": " << j1["message"] << std::endl;
            json_queue.pop();
        }
        std::this_thread::sleep_for(5s);
    }
}

int main() {
    using namespace std::literals::chrono_literals;
    using json = nlohmann::json;
    std::error_code ec;

    // Create context
    asio::io_context io_context;
    asio::io_service::work _work(io_context);

    asio::ip::tcp::endpoint endpoint( asio::ip::make_address_v4("127.0.0.1", ec), 80);

    if (!ec) {
        //asio::ip::tcp::socket socket( io_context );

        client *user = new client(io_context, endpoint, json_queue);

        std::thread network([&io_context](){ io_context.run(); });
        std::thread gameloop( worker );

        std::string input = "";

        while (input != "close") {     
            std::getline(std::cin, input);

            json json_msg;
            json_msg["header"] = "Jeff Message";
            json_msg["message"] = input;

            server_message message(json_msg.dump());

            user->send( message );
        }

        *_msg = "close";
        user->close();

        //std::this_thread::sleep_for(2s);
        // Run Server
    //user.close();
        log("closing network");
        network.join();

        log("closing game loop");
        if(gameloop.joinable()) gameloop.join();


    } else {
        logError( ec.message() );
    }    
    
    return 0;
}