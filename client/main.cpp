#include <asio.hpp>
#include <string>
#include <memory>
#include <json.hpp>
#include <queue>
#include <concurrent_queue.h> 

#include <SDL2/SDL.h>

#include "../helpers/helper.hpp"
#include "../helpers/message.hpp"


#include "client.cpp"

std::string* _msg = new std::string();

std::queue<nlohmann::json> json_queue;
moodycamel::ConcurrentQueue<nlohmann::json> CQ(1024);

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gXOut = NULL;

bool init_SDL () {
    bool success = true;

    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		success = false;
    } else {
        gWindow = SDL_CreateWindow(
            "BoI Clone", 
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN );
        
        if (gWindow == NULL) {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
			success = false;
        } else {
            gScreenSurface = SDL_GetWindowSurface( gWindow );
        }
    }

    return success;
}

bool loadMedia() {
    bool success = true;

    gXOut = SDL_LoadBMP("x.bmp");
    if (gXOut == NULL) {
    	printf( "Unable to load image %s! SDL Error: %s\n", "03_event_driven_programming/x.bmp", SDL_GetError() );
		success = false;
    }

    return success;
}

void close_SDL() {
    SDL_FreeSurface(gXOut);
    gXOut = NULL;

    SDL_DestroyWindow( gWindow );
    gWindow = NULL;

    SDL_Quit();
}


void worker( ) {
    using namespace std::literals::chrono_literals;

    nlohmann::json output;
    bool found;

    while(*_msg != "close") {
        

         
        std::this_thread::sleep_for((1/4)*1s);
    }
    
}

int main(int argc, char* args[]) {
    using namespace std::literals::chrono_literals;
    using json = nlohmann::json;
    std::error_code ec;

    // Create context
    asio::io_context io_context;
    asio::io_service::work _work(io_context);

    asio::ip::tcp::endpoint endpoint( asio::ip::make_address_v4("127.0.0.1", ec), 80);

    if (!ec) {
        client *user = new client(io_context, endpoint, CQ);

        std::thread network([&io_context](){ io_context.run(); });
        //std::thread gameloop( worker );

        if (!init_SDL()) {
            throw "Failed to initialize SDL";
            return 1;
        } else {
            if (!loadMedia()) {
                throw "Failed to load media";
                return 1;
            } else {
                bool quit = false;

                SDL_Event e;

                while (!quit) {     

                    while( SDL_PollEvent(&e) != 0 ) {
                        if (e.type == SDL_QUIT) {
                            quit = true;
                        }
                    }

                    nlohmann::json output;
                    bool found = CQ.try_dequeue(output);
        
                    SDL_Rect r;

                    while (found) {
                        //std::cout << "Queue Popped: " << output["pos"]["x"] << " : " << output["pos"]["y"] << std::endl;
                        r.x = output["pos"]["x"];
                        r.y = output["pos"]["y"];
                        r.w = 50;
                        r.h = 50;
                        SDL_FillRect(gScreenSurface, NULL, 0x000000);
                        //Apply the image
                        SDL_BlitSurface( gXOut, &r, gScreenSurface, &r );
                
                        //Update the surface
                        SDL_UpdateWindowSurface( gWindow );
                    
                        found = CQ.try_dequeue(output);

                    }  

                    
                   
                    

                    //json json_msg;
                    //json_msg["header"] = "Jeff Message";
                    //json_msg["message"] = input;

                    //server_message message(json_msg.dump());

                    //user->send( message );
                }

            }
        }
        
        close_SDL();

        *_msg = "close";
        user->close();

        log("closing network");
        if (network.joinable()) network.join();

        //log("closing game loop");
        //if(gameloop.joinable()) gameloop.join();


    } else {
        logError( ec.message() );
    }    
    
    return 0;
}