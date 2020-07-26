#include <string>
#include <thread>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include "proto/vFirmware_API.pb.h"

int main(int argc, char* argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    VF_Commands commands;
    Vec_2D trans_vec;
    Vec_2D kick_vec;
    float trans_vec_x = 0;
    float trans_vec_y = 0;
    float rotate = 0;
    float kick_vec_x = 0;
    float kick_vec_y = 0;
    bool drib = false;
    
    // VF_Data data;

    std::string line;
    std::string output;

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




        while(true)
        {
            std::cout << "Initialize sensor systems? Y or N : ";
            std::cin >> line;
            std::cout << std::endl;
        
            if(toupper(line.at(0)) == 'Y') 
            {
                commands.set_init(true);
                break;
            }
            else
            {
                commands.set_init(false);
                break;
            }
        }

        while(true)
        {
            line = "";
            output = "";
            trans_vec.Clear();
            kick_vec.Clear();
            trans_vec_x = 0;
            trans_vec_y = 0;
            rotate = 0;
            kick_vec_x = 0;
            kick_vec_y = 0;
            drib = false;



            std::cout << "Input required: " << std::endl;
            std::cout << "Format: <translational_output_vec: x y> <rotational_output> <kicker_vec: x y> <dribbler on? Y or N>:" 
                << std::endl;
            
            std::cin >> trans_vec_x >> trans_vec_y >> rotate >> kick_vec_x >> kick_vec_y >> line;
            
            drib = toupper(line.at(0)) == 'Y'? 1 : 0;

            trans_vec.set_x(trans_vec_x);
            trans_vec.set_y(trans_vec_y);
            kick_vec.set_x(kick_vec_x);
            kick_vec.set_y(kick_vec_y);
            commands.set_allocated_translational_output(&trans_vec);
            commands.set_rotational_output(rotate);
            commands.set_allocated_kicker(&kick_vec);
            commands.set_dribbler(drib);
            commands.SerializeToString(&output);
            socket.write_some(boost::asio::buffer(output));
            std::cout << "Packet sent." << std::endl;
            commands.release_kicker();
            commands.release_translational_output();
        }

    }
    catch(std::exception& e)
    {
        std::cerr << "[Exception]"  << std::string(e.what());
    }

    return 0;
        
}