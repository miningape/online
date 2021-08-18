#ifndef SERVER_CORE
#define SERVER_CORE

#define NOT !

#include <asio.hpp>
#include <memory>
#include <string>
#include <concurrent_queue.h>
#include <deque>

#include "../helpers/helper.hpp"
#include "../helpers/message.hpp"

class server_proto {
    public:
    virtual void deleteSelf( int connection_id )=0;    
};

class connection : public std::enable_shared_from_this<connection> {
    public:
    connection( asio::ip::tcp::socket sock, asio::io_context& context, server_proto* master_, int id_ ) 
    : socket( std::move(sock) ), io_context(context), master( master_ ), id(id_)  {}


    //moodycamel::ConcurrentQueue<server_message> msg_queue;
    std::deque<server_message> msg_queue;

    void enqueue( server_message message ) {
      //std::cout << connections.size() << std::endl;

      asio::post( io_context, [this, message](){
        server_message item;

        bool writing = !msg_queue.empty();
        msg_queue.push_back(message);

        if(!writing) {
          send();
        }
      } );
    }

    bool isConnected() {
      return socket.is_open();
    }
    

    void send() {
        auto self(shared_from_this());
        asio::async_write( socket, msg_queue.front().rawBuffer(), 
        [this, self]( std::error_code ec, std::size_t length ) {
            //std::string str = message.value();

            if (ec) //log("Sent: " + str );
            {
              logError( "pipe:" + ec.message() );
              //socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
              socket.close();
              
            } else {
              msg_queue.pop_front();

              if ( !msg_queue.empty() )
                send( );
            }
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
              //master->sendall( input_msg );
              read_head();
              
          } else {
              logError( ec.message() );
          
          }
        });
    }

    int getID() {
      return id;
    }

    int id;

    private:
    asio::ip::tcp::socket socket;
    server_message input_msg;
    server_proto* master;

    asio::io_context& io_context;

    

    char user[4];
};

class server : public server_proto {
    public:
    server( uint16_t port ) 
        : acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {
        
        prime_accept();

        sthread = std::thread([this](){ io_context.run(); });
    }

    void sendall(server_message s) {
      for ( auto c : connections ) {
        if ( c && c->isConnected() ) {
          c->enqueue(s);
        } else {
          deleteSelf( c->getID() );
          connections.erase( std::remove( connections.begin(), connections.end(), c ), connections.end() );
        }
      }
    }

    void deleteSelf( int connection_id ) {
      auto it = std::remove_if(connections.begin(), connections.end(), [connection_id](std::shared_ptr<connection> c){
        return c->id == connection_id;
      });

      connections.erase(it, connections.end());

    }


    private:
    void prime_accept() {
        acceptor.async_accept( [this](std::error_code ec, asio::ip::tcp::socket socket) {
            if ( !ec ) {
                log("Accepted Connection");
                
                std::shared_ptr<connection> x = std::make_shared<connection>( std::move( socket ), io_context, this, available_id++ );

                x->read_head();
                connections.push_back(x);

            } else {
                log("Failed Connection: " + ec.message());
            }

            prime_accept();
        } );
    }

    
    
    std::vector<std::shared_ptr<connection>> connections;
    asio::io_context io_context;
    std::thread sthread;
    asio::ip::tcp::acceptor acceptor;
    
    int available_id = 1;
};

#endif