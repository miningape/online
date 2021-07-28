#ifndef HELPER
#define HELPER

#include <iostream>


void log( std::string text ) {
    std::cout << "== " << text << " ==" << std::endl;
}

template<typename T>
void logVector ( std::vector<T>& vector ) {
    for (auto i = vector.begin(); i != vector.end(); ++i)
        std::cout << *i;
}

void logError( std::string text )  {
    std::cout << "**: " << text << " **" << std::endl;
}

#endif