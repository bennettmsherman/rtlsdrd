/*
 * CommandServer.cpp
 *
 *  Created on: Sep 16, 2017
 *      Author: bensherman
 */

// System Includes
#include <cstdint>
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

// Project Includes
#include <TcpServer.hpp>
#include "RtlFmParameterBuilder.hpp"
#include "CommandParser.hpp"


TcpServer::TcpServer(const uint16_t port) : port(port), ioService(),
            acceptor(ioService, BoostTcp::endpoint(BoostTcp::v4(), port), true)
{
};


void TcpServer::printServerInfo()
{
    std::cout << "Rtlsdrd server started on: " <<
            "\n\tIPs: " << getLocalIpAddresses() <<
            "\n\tHost: " << boost::asio::ip::host_name() <<
            "\n\tPort: " << port << std::endl;
}

/**
 * Returns a space-separated list of the ip addresses of this machine
 * TODO fix showing of localhost
 */
std::string TcpServer::getLocalIpAddresses()
{
    BoostTcp::resolver resolver(ioService);
    BoostTcp::resolver::query query(boost::asio::ip::host_name(), "");
    BoostTcp::resolver::iterator iter = resolver.resolve(query);

    std::string ipAddrs;
    while (iter != BoostTcp::resolver::iterator{})
    {
        BoostTcp::endpoint endpoint = *iter++;
        ipAddrs.append(endpoint.address().to_string());
        ipAddrs.append(" ");
    }
    return ipAddrs;
}

/**
 * Interacts with a connected client. This function represents is executed by
 * a thread which is dedicated to a single client. It will receive and parse
 * strings from the client, and will return status information to it.
 */
void TcpServer::connectionHandler(const TcpSocketSharedPtr& sock)
{
    std::string clientIp = sock->remote_endpoint().address().to_string();
    uint16_t clientPort = sock->remote_endpoint().port();
    // The parameter builder and parser are used for parsing and executing
    // commands sent by the client
    RtlFmParameterBuilder rtlFmWrapper;
    const CommandParser& parser = CommandParser::getInstance();

    std::cout << "New client: " << clientIp << ":" << clientPort << std::endl;

    try
    {
      while (true)
      {
          // Receive data from the client
          std::string receivedData;
          uint32_t bytesReceived = receiveData(sock, receivedData);

          std::cout << "Received " << bytesReceived << " bytes from: "
                  << clientIp << ":" << clientPort
                  << "\n\tcontent: " << receivedData << std::endl;

          // Parse and execute the command/data from the client
          std::string parseResult = parser.execute(receivedData, rtlFmWrapper);
          parseResult.append("\n");

          // Send the data back to the client
          size_t bytesSent = sendData(sock, parseResult);

          std::cout << "Wrote " << bytesSent << " bytes to: "
                  << clientIp << ":" << clientPort
                  << "\n\tcontent: " << parseResult << std::endl;
      }
    }
    catch (std::exception& exception)
    {
        std::cout << exception.what() << std::endl;
    }
}

/**
 * Sends the data stored in dataToSend to the client associated with sock
 */
size_t TcpServer::sendData(const TcpSocketSharedPtr& sock, const std::string& dataToSend)
{
    return boost::asio::write(*sock,
            boost::asio::buffer(dataToSend.c_str(), dataToSend.length()));
}

/**
 * Waits for data to be received on sock. Once received, the receivedData is updated
 * with the new data, and the number of bytes received is returned.
 */
size_t TcpServer::receiveData(const TcpSocketSharedPtr& sock, std::string& receivedData)
{
    // Buffers to store data off the line
    char buffData[RECEIVE_BUFFER_SIZE];

    // Wait for data from the client
    size_t bytesReceived = sock->receive(boost::asio::buffer(buffData, RECEIVE_BUFFER_SIZE));
    buffData[bytesReceived] = '\0';

    receivedData = std::string(buffData);

    return bytesReceived;
}

/**
 * Allows this server to receive connections. All new connections execute
 * this->connectionHandler in a new thread.
 */
void TcpServer::run()
{
    printServerInfo();
    while (true)
    {
      TcpSocketSharedPtr sock(new BoostTcp::socket(ioService));
      acceptor.accept(*sock);
      boost::thread peerHandler(boost::bind(&TcpServer::connectionHandler, this, sock));
    }
}

/**
 * Terminates this TcpServer
 */
void TcpServer::terminate()
{
    std::cout << "Terminating server!" << std::endl;
    acceptor.close();
}

