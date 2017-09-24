/*
 * ComandServer.hpp
 *
 *  Created on: Sep 16, 2017
 *      Author: bensherman
 */

#ifndef NETWORK_TCPSERVER_HPP_
#define NETWORK_TCPSERVER_HPP_

// System Includes
#include <cstdint>
#include <boost/asio.hpp>

// Project Includes
// <none>

// Using statements
using BoostTcp = boost::asio::ip::tcp;
using BoostIoService = boost::asio::io_service;
using TcpSocketSharedPtr = std::shared_ptr<BoostTcp::socket>;

class TcpServer
{
public:
    void run();
    void terminate();

    TcpServer(const uint16_t port=DEFAULT_PORT);

private:

    std::string getLocalIpAddresses();
    size_t sendData(const TcpSocketSharedPtr& sock, const std::string& dataToSend);
    size_t receiveData(const TcpSocketSharedPtr& sock, std::string& receivedData);
    void connectionHandler(const TcpSocketSharedPtr& sock);
    void printServerInfo();

    // The port which this server is associated with
    uint16_t port;

    BoostIoService ioService;

    BoostTcp::acceptor acceptor;

    static const uint16_t DEFAULT_PORT = 2832;
    static const uint16_t RECEIVE_BUFFER_SIZE = 256;
};



#endif /* NETWORK_TCPSERVER_HPP_ */
