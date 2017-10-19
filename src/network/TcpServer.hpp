/*
 * TcpServer.hpp
 *
 *  Created on: Sep 16, 2017
 *      Author: bensherman
 */

#ifndef NETWORK_TCPSERVER_HPP_
#define NETWORK_TCPSERVER_HPP_

// System Includes
#include <boost/asio.hpp>
#include <cstdint>
#include <list>
#include <vector>
#include <string>

// Project Includes
#include "SocketWrapper.hpp"

// Using statements
using BoostTcp = boost::asio::ip::tcp;
using BoostIoService = boost::asio::io_service;

class TcpServer
{
public:
    static TcpServer& getInstance(uint16_t port=2832);

    void informAllClientsOfStateChange();
    void run();
    void terminate();

private:
    // A private constructor is required for the singleton pattern
    TcpServer(const uint16_t port=DEFAULT_PORT);

    // Delete the default copy constructor
    TcpServer(const TcpServer&) = delete;

    // Delete the default assignment operator
    TcpServer& operator=(const TcpServer&) = delete;

    // General info retrieval and print functions
    void printServerInfo();
    std::vector<std::string> getLocalIpAddresses();

    // Socket interface functions
    static size_t sendData(SocketWrapper& sockWrap, const std::string& dataToSend);
    static size_t receiveData(TcpSocket& sock, std::string& receivedData);
    static void sendDataVoidReturn(SocketWrapper& sockWrap, const std::string& dataToSend);

    // Thread to be executed for each client
    void connectionHandler(SocketWrapper& sockWrap);

    // Locked mutators for socketWrappersInUse
    SocketWrapper& appendToSocketWrappersInUse(TcpSocketSharedPtr& newSocket);
    void removeFromSocketWrappersInUse(const SocketWrapper& sockWrapper);

    // The port which this server is associated with
    uint16_t port;

    BoostIoService ioService;

    BoostTcp::acceptor acceptor;

    // List of sockets and associated mutexes
    std::list<SocketWrapper> socketWrappersInUse;

    // Used to synchronize addition and deletion operations
    // on the socketWrappersInUse vector
    std::mutex socketWrappersInUseMutex;

    static const std::string UPDATED_PARAMETERS_AVAILABLE_STRING;
    static const uint16_t DEFAULT_PORT = 2832;
    static const uint16_t RECEIVE_BUFFER_SIZE = 256;
};



#endif /* NETWORK_TCPSERVER_HPP_ */
