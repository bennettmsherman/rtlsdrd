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
    static TcpServer& getInstance(uint16_t port=DEFAULT_PORT, const char * const password=DEFAULT_PASSWORD);

    void informAllClientsOfStateChange();
    void run();
    void terminate();

    // Interpreter-executable commands
    void getAddressInfoHandler(const std::string& UNUSED, std::string* clientReturnableInfo);
    void getClientsInfoHandler(const std::string& UNUSED, std::string* clientReturnableInfo);

private:
    // A private constructor is required for the singleton pattern
    TcpServer(const uint16_t port=DEFAULT_PORT, const char * const password=DEFAULT_PASSWORD);

    // Delete the default copy constructor
    TcpServer(const TcpServer&) = delete;

    // Delete the default assignment operator
    TcpServer& operator=(const TcpServer&) = delete;

    // General info retrieval and print functions
    std::string getServerInfo();
    std::vector<std::string> getLocalIpAddresses();

    // Thread to be executed for each client
    void connectionHandler(SocketWrapper& sockWrap);

    // Locked mutators for socketWrappersInUse
    SocketWrapper& appendToSocketWrappersInUse(TcpSocketSharedPtr& newSocket);
    void removeFromSocketWrappersInUse(const SocketWrapper& sockWrapper);

    // Authentication of clients which are trying to connect
    bool authenticate(SocketWrapper& socketWrap, BoostStreamBuff& socketReadStreamBuff);

    // The port which this server is associated with
    uint16_t port;

    // The password for this server
    const std::string password;

    BoostIoService ioService;

    BoostTcp::acceptor acceptor;

    // List of sockets and associated mutexes
    std::list<SocketWrapper> socketWrappersInUse;

    // Used to synchronize addition and deletion operations
    // on the socketWrappersInUse vector
    std::mutex socketWrappersInUseMutex;

    static const std::string UPDATED_PARAMETERS_AVAILABLE_STRING_PREFIX;
    static const std::string REQUESTING_PASSWORD_STRING_PREFIX;
    static const std::string END_OF_RESPONSE_STRING;
    static const std::string SOCKET_READ_UNTIL_END_SPECIFIER;
    static const uint16_t DEFAULT_PORT = 2832;
    static const char * const DEFAULT_PASSWORD;
    static const std::string AUTH_SUCCESSFUL_STRING;
    static const std::string AUTH_FAILED_STRING;

};

#endif /* NETWORK_TCPSERVER_HPP_ */
