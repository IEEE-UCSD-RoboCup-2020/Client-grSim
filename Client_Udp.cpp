#include <string>
#include <thread>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
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
    boost::mutex mu;

    VF_Data datas;
    Vec_2D trans_dis;
    Vec_2D trans_velo;
    float trans_dis_x = 0;
    float trans_dis_y = 0;
    float trans_velo_x = 0;
    float trans_velo_y = 0;
    float rotate_dis = 0;
    float rotate_velo = 0;
    
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
        boost::asio::ip::udp::endpoint ep(boost::asio::ip::address::from_string(ip), port);

        boost::asio::ip::udp::socket* socket = new boost::asio::ip::udp::socket(io_service);
        socket->open(boost::asio::ip::udp::v4());

        std::cout << "Socket now open." << std::endl;

        // send data to the server
        boost::thread data_thread([&]()
        {
            while(true)
            {


                mu.lock();
                std::cout << "Initialize sensor systems? Y or N : ";
                mu.unlock();

                std::cin >> line;

                mu.lock();
                std::cout << std::endl;
                mu.unlock();

                if(toupper(line.at(0)) == 'Y') 
                {
                    commands.set_init(true);
                    commands.SerializeToString(&output);
                    output += "\n";
                    socket->send_to(boost::asio::buffer(output), ep);
                    break;
                }
                else
                {
                    commands.set_init(false);
                    commands.SerializeToString(&output);
                    socket->send_to(boost::asio::buffer(output), ep);
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


                mu.lock();
                std::cout << "Input required: " << std::endl;
                std::cout << "Format: <translational_output_vec: x y> <rotational_output> <kicker_vec: x y> <dribbler on? Y or N>:" 
                    << std::endl;
                mu.unlock();

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
                output += "\n";
                socket->send_to(boost::asio::buffer(output), ep);
                mu.lock();
                std::cout << "Packet sent." << std::endl;
                mu.unlock();
                commands.release_kicker();
                commands.release_translational_output();
            }

        });

        // receive command from the server
        boost::thread cmd_thread([&]()
        {
            boost::array<char, 1024> read_buffer; 
            std::size_t num_received;
            std::string received;

            
                
            while(true){

                num_received = socket->receive_from(boost::asio::buffer(read_buffer), ep);
                received = std::string(read_buffer.begin(), read_buffer.begin() + num_received);

                datas.ParseFromString(received);
                trans_dis = datas.translational_displacement();
                trans_dis_x = trans_dis.x();
                trans_dis_y = trans_dis.y();
                trans_velo = datas.translational_velocity();
                trans_velo_x = trans_velo.x();
                trans_velo_y = trans_velo.y();
                rotate_dis = datas.rotational_displacement();
                rotate_velo = datas.rotational_velocity();

                mu.lock();
                std::cout << "Received!\ntrans_dis = <" << trans_dis_x << ", " << trans_dis_y << ">"
                            << "| trans_velo = <" << trans_velo_x << ", " << trans_velo_y << ">"
                            << "| rotate_dis = " << rotate_dis << "| rotate_velo = " << rotate_velo; 
            
                mu.unlock();
            }
        });

        cmd_thread.join();
        data_thread.join();

    }
    catch(std::exception& e)
    {
        std::cerr << "[Exception]"  << std::string(e.what());
    }

    
    return 0;
    
        
}