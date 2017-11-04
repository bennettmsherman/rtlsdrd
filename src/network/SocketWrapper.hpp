/*
 * SocketWrapper.hpp
 *
 *  Created on: Oct 18, 2017
 *      Author: bensherman
 */

#ifndef NETWORK_SOCKETWRAPPER_HPP_
#define NETWORK_SOCKETWRAPPER_HPP_

// System Includes
#include <boost/asio.hpp>
#include <mutex>
#include <stdint.h>

// Typedef/using statements for convenience
using TcpSocketSharedPtr = std::shared_ptr<boost::asio::ip::tcp::socket>;
using TcpSocket = boost::asio::ip::tcp::socket;

class SocketWrapper
{
public:
    SocketWrapper(TcpSocketSharedPtr& socketPtr);
    SocketWrapper(const SocketWrapper& toCopy);
    SocketWrapper& operator=(const SocketWrapper& base);
    bool operator==(const SocketWrapper& other);

    TcpSocket& getSocket();
    std::mutex& getWriteMutex();
    const std::string& getIpAddress();
    uint16_t getPortNumber();

private:
    TcpSocketSharedPtr socketPtr;
    std::mutex writeMutex;
    std::string ipAddr;
    uint16_t portNum;
};

#endif /* NETWORK_SOCKETWRAPPER_HPP_ */