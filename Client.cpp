#include <string>
#include <thread>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include "proto/message.pb.h"

int main(int argc, char* argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    example::Message message;

    unsigned int port = 0;
    std::string ip = "";

    try
    {
        if(argc != 3)
        {
            std::cerr << "Usage: client <host IPv4> <port#>\n";
            return 1;
        }

        ip = std::string(argv[1]);
        port = std::stoi(std::string(argv[2]), nullptr, 10);

        boost::asio::io_service io_service;
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(ip), port);
        boost::asio::ip::tcp::socket socket(io_service);
        socket.open(boost::asio::ip::tcp::v4());
        socket.connect(ep);

        std::cout << "Successsfully connected to " << ep.address() << " port " << ep.port() << std::endl;

        std::string line;
        std::string output;

        while(true)
        {
            std::cin >> line;

            message.set_mesg(line);
            message.SerializeToString(&output);
            socket.write_some(boost::asio::buffer(output));
        }

    }
    catch(std::exception& e)
    {
        std::cerr << "[Exception]"  << std::string(e.what());
    }

    return 0;
        
}