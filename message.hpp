/*
Reason:
We dont know how long each message will be,
but what we can do instead is have the first part of any
message be of a guaranteed size, which tells us exactly
how big the rest of the body is.

This is needed for us to be able to read memory properly,
as we can only read into memory set amounts of info.

Memory:
= RAW =
This is the entire block of memory we are using, it 
is seperated between the head and the body. The head
heas a fixed size, and the body has a variable size.

Raw is just an array of chars lmao, current max size,
is 1024 bytes (chars). But this could probably be changed.

= HEAD = 
Head contains the size of the body.

Size is exactly 4 bytes because an int is 4 bytes,
but needs to be stored as a char, so we need to convert
it back and forth

= BODY =
Contains the rest of the raw object, its size can be changed
and can contain any information that can be encoded as a
string.

Design:
End user should never need to interact with the header.
and should not need to know it is there.

server_message jeff = server_message();
server_message jeff = server_message("Initial Message");

jeff.append(" more message ");      // Appends to message
jeff.clear();                       // Completely empties
jeff.value();                       // Returns value of body

/* Returns buffer to appropriate area * /
jeff.headBuffer();      // For writing to head
jeff.bodyBuffer();      // For writing to body
jeff.rawBuffer();       // For reading entire object

jeff.update_head();     // Reads the header and calculates the body size
*/

#ifndef MESSAGE
#define MESSAGE

#include <iostream>
#include <vector>

#include <asio.hpp>

class server_message {
    public:
    int head_size = 4;
    int body_size = 0;

    server_message(): raw(1024) {};
    server_message( std::string str ): raw(1024) { append(str); }

    void append( std::string str ) {
        std::vector<char> adder(str.begin(), str.end());
        raw.insert(raw.begin() + raw_size(), str.begin(), str.end());

        body_size += str.length();
        set_header();
    }

    void clear() {
        // ! Probably not efficient
        // ! Can be replaced with " = server_message()"
        std::fill( raw.begin(), raw.end(), '\0' );
        
        body_size = 0;
        set_header();
    }

    std::string value() {
        return std::string(raw.begin() + 4, raw.end());
    }

    const std::string value() const {
        return std::string(raw.begin() + 4, raw.end());
    }

    asio::mutable_buffers_1 bodyBuffer() {
        read_header();
        return asio::buffer( raw.data() + head_size, body_size );
    }
    
    asio::mutable_buffers_1 headBuffer() {
        return asio::buffer( raw.data(), head_size );
    }

    asio::mutable_buffers_1 rawBuffer() {
        return asio::buffer( raw.data(), raw_size() );
    }

    void update_head() { read_header(); }


    private:
    std::vector<char> raw;

    void read_header() {
        header_union.bytes[0] = raw[0];
        header_union.bytes[1] = raw[1];
        header_union.bytes[2] = raw[2];
        header_union.bytes[3] = raw[3];

        body_size = header_union.number;
    }

    void set_header( ) {
        header_union.number = body_size;

        raw[0] = header_union.bytes[0];
        raw[1] = header_union.bytes[1];
        raw[2] = header_union.bytes[2];
        raw[3] = header_union.bytes[3];
    }

    int raw_size() { 
        int s = head_size + body_size;
        return s; 
    }

    union HU {
        unsigned int number;
        unsigned char bytes[4];
    } header_union;
};

#endif