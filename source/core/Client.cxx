#include <core/Client.hxx>
#include <iostream>

#ifndef VERSION
#define VERSION "Unknown version"
#endif

Client::Client()
{
    std::cout << "Test " << VERSION << std::endl;
}

Client::~Client()
{

}