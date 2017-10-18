/*
 * TcpServer.cpp
 *
 *  Created on: Sep 16, 2017
 *      Author: bensherman
 */

// System Includes
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>
#include <cstdint>
#include <ifaddrs.h>
#include <iostream>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <system_error>

// Project Includes
#include "TcpServer.hpp"
#include "RtlFmParameterBuilder.hpp"
#include "CommandParser.hpp"


TcpServer::TcpServer(const uint16_t port) : port(port), ioService(),
            acceptor(ioService, BoostTcp::endpoint(BoostTcp::v4(), port), true)
{
};


void TcpServer::printServerInfo()
{
    // Retrieve local network interfaces by name and IP
    std::vector<std::string> interfacesAndIpsStrings = getLocalIpAddresses();

    std::cout << "Rtlsdrd server started on: " <<
            "\n\tIPs: ";

    for (std::string interfaceAndIp : interfacesAndIpsStrings)
    {
        std::cout << "\n\t  " << interfaceAndIp;
    }

    std::cout << "\n\tHost: " << boost::asio::ip::host_name() <<
                 "\n\tPort: " << port << std::endl;
}

/**
 * Returns a vector of strings where each entry contains an interface name and
 * its IP in the form "<interface>:<ip>". IPv4 and IPv6 IPs included.
 * Throws std::system_error if the call to getifaddrs() fails.
 * Throws std::runtime_error if the call to getnameinfo() fails.
 */
std::vector<std::string> TcpServer::getLocalIpAddresses()
{
    ifaddrs * interfaces = nullptr;
    std::vector<std::string> interfacesAndIpStringsVect;

    if (!getifaddrs(&interfaces))
    {
        // Have a second pointer so that the original reference to the list
        // head can be used with freeifaddrs()
        ifaddrs * currentInterface = interfaces;
        for (; currentInterface != nullptr; currentInterface = currentInterface->ifa_next)
        {
            char ipAsStr[NI_MAXHOST] = {};
            int getNameInfoRetVal = 0;

            // IPv4
            if (currentInterface->ifa_addr->sa_family == AF_INET)
            {
                getNameInfoRetVal = getnameinfo(currentInterface->ifa_addr,
                                                sizeof(sockaddr_in), ipAsStr,
                                                NI_MAXHOST, nullptr, 0,
                                                NI_NUMERICHOST);
            }
            // IPv6
            else if (currentInterface->ifa_addr->sa_family == AF_INET6)
            {
                getNameInfoRetVal = getnameinfo(currentInterface->ifa_addr,
                                                sizeof(sockaddr_in6), ipAsStr,
                                                NI_MAXHOST, nullptr, 0,
                                                NI_NUMERICHOST);
            }
            // Ignore other interface types
            else
            {
                continue;
            }

            // If an error occurred when executing getnameinfo() throw a
            // std::runtime_error
            if (getNameInfoRetVal)
            {
                freeifaddrs(interfaces);
                std::string exceptionMsg("Error retrieving interface IP with code: ");
                exceptionMsg += std::to_string(getNameInfoRetVal);
                exceptionMsg += " and msg: ";
                exceptionMsg += gai_strerror(getNameInfoRetVal);
                throw std::runtime_error(exceptionMsg);
            }

            // Generate the string and add it to the vector
            std::string interfaceAndIpString(currentInterface->ifa_name);
            interfaceAndIpString += ":";
            interfaceAndIpString += ipAsStr;
            interfacesAndIpStringsVect.push_back(interfaceAndIpString);
        }

        freeifaddrs(interfaces);
    }
    else
    {
        throw std::system_error(errno, std::system_category(), "Error retrieving this machine's network interfaces");
    }

    return interfacesAndIpStringsVect;
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
        std::cout << "Client: " << clientIp << ":" << clientPort
                << "-> Exception: " << exception.what() << std::endl;
    }
}

/**
 * Sends the data stored in dataToSend to the client associated with sock
 */
size_t TcpServer::sendData(const TcpSocketSharedPtr& sock, const std::string& dataToSend)
{
    return boost::asio::write(*sock, boost::asio::buffer(dataToSend));
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

