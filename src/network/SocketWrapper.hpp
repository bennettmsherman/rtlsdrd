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
using BoostStreamBuff = boost::asio::streambuf;

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
    const std::string getIpAddressAndPort();

    // Functions for I/O on the socketPtr
    size_t sendData(const std::string& dataToSend);
    void sendDataVoidReturn(const std::string& dataToSend);
    size_t receiveData(std::string& receivedData, BoostStreamBuff& readBuff,
            const std::string& charToReadUntil);

private:
    TcpSocketSharedPtr socketPtr;
    std::mutex writeMutex;
    std::string ipAddr;
    uint16_t portNum;
};

#endif /* NETWORK_SOCKETWRAPPER_HPP_ */
