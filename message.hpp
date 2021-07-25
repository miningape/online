#ifndef MESSAGE
#define MESSAGE

#include <iostream>
#include <vector>

class server_message {
    public:
    std::vector<char> raw;
    
    int header_size = 4;

    union bodySizeUnion {
        unsigned int integer;
        unsigned char bytes[4];
    } body_size;

    server_message(): raw(1024) {}

    void insert( std::string str  ) {
        body_size.integer = str.length();

        for (int i = 0; i < 4; i++) {
            raw[i] = body_size.bytes[i];
        }

        std::vector<char> adder(str.begin(), str.end());
        raw.insert(raw.begin() + 4, str.begin(), str.end());
    } 

    int header() {
        std::vector<char> header(raw.begin(), raw.begin() + 4);
        body_size.bytes[0] = header[0];
        body_size.bytes[1] = header[1];
        body_size.bytes[2] = header[2];
        body_size.bytes[3] = header[3];

        return body_size.integer;
    }

    std::string body() {
        return std::string(raw.begin() + 4, raw.end());
    }

    int size() {
        return body_size.integer + header_size;
    }
};

#endif