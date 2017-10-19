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
#include <mutex>
#include <netdb.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <system_error>
#include <thread>
#include <vector>

// Project Includes
#include "CommandParser.hpp"
#include "RtlFmParameterBuilder.hpp"
#include "SocketWrapper.hpp"
#include "TcpServer.hpp"

// Static Initialization
const std::string TcpServer::UPDATED_PARAMETERS_AVAILABLE_STRING = "%UPDATE_AVAILABLE\n";


TcpServer::TcpServer(const uint16_t port) : port(port), ioService(),
            acceptor(ioService, BoostTcp::endpoint(BoostTcp::v4(), port), true)
{
};

/**
 * Prints this machine's hostname, the port this server is running on, and a
 * list of all NICs in this machine and their IP addresses (IPv4 and IPv6)
 */
void TcpServer::printServerInfo()
{
    // Retrieve local network interfaces by name and IP
    std::vector<std::string> interfacesAndIpsStrings = getLocalIpAddresses();

    std::cout << "Rtlsdrd server started on: " <<
            "\n\tIPs: ";

    // Print each NIC's info on a newline
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
 * When this function terminates, the SocketWrapper parameter is removed
 * from the socketWrappersInUse vector.
 */
void TcpServer::connectionHandler(SocketWrapper& sockWrap)
{
    std::string clientIp = sockWrap.getSocket().remote_endpoint().address().to_string();
    uint16_t clientPort = sockWrap.getSocket().remote_endpoint().port();

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
          uint32_t bytesReceived = receiveData(sockWrap.getSocket(), receivedData);

          std::cout << "Received " << bytesReceived << " bytes from: "
                  << clientIp << ":" << clientPort
                  << "\n\tcontent: " << receivedData << std::endl;

          // Parse and execute the command/data from the client
          std::string parseResult = parser.execute(receivedData, rtlFmWrapper);
          parseResult.append("\n");

          // Send the data back to the client
          size_t bytesSent = sendData(sockWrap, parseResult);

          std::cout << "Wrote " << bytesSent << " bytes to: "
                  << clientIp << ":" << clientPort
                  << "\n\tcontent: " << parseResult << std::endl;
      }
    }
    catch (std::exception& exception)
    {
        std::cout << "Client: " << clientIp << ":" << clientPort
                << "-> Exception: " << exception.what() << std::endl;

        removeFromSocketWrappersInUse(sockWrap);
    }
    // Execution should never get here since an exception is thrown by asio
    // to indicate that the client has disconnected

}

/**
 * Sends the data stored in dataToSend to the client associated with socket
 * specified by sockWrap. The call to write() is guarded with the mutex
 * stored in sockWrap.
 */
size_t TcpServer::sendData(SocketWrapper& sockWrap, const std::string& dataToSend)
{
    // Lock the write operation
    std::lock_guard<std::mutex> writeLock{sockWrap.getWriteMutex()};

    return boost::asio::write(sockWrap.getSocket(), boost::asio::buffer(dataToSend));
}

/**
 * Waits for data to be received on sock. Once received, the receivedData is updated
 * with the new data, and the number of bytes received is returned.
 */
size_t TcpServer::receiveData(TcpSocket& sock, std::string& receivedData)
{
    // Buffers to store data off the line
    char buffData[RECEIVE_BUFFER_SIZE];

    // Wait for data from the client
    size_t bytesReceived = sock.receive(boost::asio::buffer(buffData, RECEIVE_BUFFER_SIZE));
    buffData[bytesReceived] = '\0';

    receivedData = std::string(buffData);

    return bytesReceived;
}

/**
 * Allows this server to receive connections. All new connections execute
 * this->connectionHandler in a new thread, and all connected clients
 * are associated with a SocketWrapper instance which is added to
 * socketWrappersInUse.
 */
void TcpServer::run()
{
    printServerInfo();
    while (true)
    {
      TcpSocketSharedPtr sock(new BoostTcp::socket(ioService));
      acceptor.accept(*sock);

      // Create a new SocketWrapper for this client, add it to
      // socketWrappersInUse, and send its reference as a param to the client
      // thread.
      boost::thread peerHandler(boost::bind(&TcpServer::connectionHandler, this,
              boost::ref(appendToSocketWrappersInUse(sock))));
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

/**
 * Creates a new SocketWrapper instance containing the TcpSocketSharedPointer
 * parameter as a member. Then, this new SocketWrapper is appended to the
 * socketWrappersInUse vector. This access to socketWrappersInUse is locked
 * with the socketWrappersInUseMutex mutex. A reference to the newly created
 * SocketWrapper, within the vector, is returned.
 */
SocketWrapper& TcpServer::appendToSocketWrappersInUse(TcpSocketSharedPtr& newSocket)
{
    // Lock access to socketWrappersInUse
    std::lock_guard<std::mutex> lock{socketWrappersInUseMutex};

    socketWrappersInUse.push_back(SocketWrapper(newSocket));

    return socketWrappersInUse.back();
}

/**
 * Removes the socketWrapper parameter from the socketWrappersInUse vector.
 * The socketWrappersInUseMutex is used to lock this access to the vector.
 * If the parameter specified is not an element in the vector, a
 * std::out_of_range exception is thrown. If there was any other error such
 * that the socketWrapper wasn't actually deleted from the vector, a
 * std::runtime_error is thrown.
 */
void TcpServer::removeFromSocketWrappersInUse(const SocketWrapper& socketWrapper)
{
    // Lock access to socketWrappersInUse
    std::lock_guard<std::mutex> lock{socketWrappersInUseMutex};

    auto preDeleteSize = socketWrappersInUse.size();

    // Find which index in the vector the socketWrapper param is located at
    int socketWrapperToDeleteIndex = (&socketWrapper - &(*socketWrappersInUse.begin()));

    if (socketWrapperToDeleteIndex < 0 ||
            socketWrapperToDeleteIndex >= static_cast<int>(socketWrappersInUse.size()))
    {
        throw std::out_of_range("Blocked attempt to remove an element from"
                " socketWrappersInUse outside its valid index range");
    }

    // Remove the socketWrapper param
    socketWrappersInUse.erase(socketWrappersInUse.begin() + socketWrapperToDeleteIndex);

    // If the size hasn't changed, then there must've been an error deleting the value
    if (socketWrappersInUse.size() - preDeleteSize != 1)
    {
        throw std::runtime_error("Attempt to remove socket from"
                " socketWrappersInUse resulted in nonchanging vector size");
    }
}

/**
 * Wrapper to call sendData() which doesn't return the number of bytes sent.
 * Instead, this function will throw a std::runtime_exception if
 * dataToSend.size() bytes weren't sent.
 */
void TcpServer::sendDataVoidReturn(SocketWrapper& sockWrap, const std::string& dataToSend)
{
    auto bytesActuallySent = sendData(sockWrap, dataToSend);
    if (bytesActuallySent != dataToSend.size())
    {
        throw std::runtime_error("Not all data written to socket");
    }
}

/**
 * Iterates over the socketWrappersInUse vector and writes the string contained
 * in UPDATED_PARAMETERS_AVAILABLE_STRING to the socket corresponding to each
 * element in the vector. Each socket's write operation is performed in a new
 * thread.
 */
void TcpServer::informAllClientsOfStateChange()
{
    for (SocketWrapper& socketWrapper : socketWrappersInUse)
    {
        boost::thread sendMsgThread(&sendDataVoidReturn,std::ref(socketWrapper),
                UPDATED_PARAMETERS_AVAILABLE_STRING);
    }
}

/**
 * Used to instantiate and access the TcpServer singleton. When this function
 * is first executed, a new TcpServer is created using the specified port.
 * All subsequent calls to this function will return a reference to the
 * original instance, AND WILL IGNORE the contents of the port parameter.
 */
TcpServer& TcpServer::getInstance(uint16_t port)
{
    static TcpServer server(port);
    return server;
}
