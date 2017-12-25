/*
 * SocketWrapper.cpp
 *
 *  Created on: Oct 18, 2017
 *      Author: bensherman
 */

// System Includes
#include <boost/asio.hpp>
#include <mutex>
#include <stdint.h>

// Project Includes
#include "SocketWrapper.hpp"

/**
 * Constructs a new SocketWrapper with the socketPtr parameter used
 * to instantiate the socketPtr member. writeMutex is instantiated using its
 * default constructor. The ipAddr and port members are initialized based off
 * of the socket parameter.
 */
SocketWrapper::SocketWrapper(TcpSocketSharedPtr& socketPtr) :
        socketPtr(socketPtr),
        ipAddr(socketPtr->remote_endpoint().address().to_string()),
        portNum(socketPtr->remote_endpoint().port())
{
}

/**
 * Copy constructor for SocketWrapper. DOES NOT COPY THE MUTEX STATE!!!
 * That is, the new SocketWrapper will have a new mutex.
 */
SocketWrapper::SocketWrapper(const SocketWrapper& toCopy)
{
    socketPtr = toCopy.socketPtr;
    ipAddr = toCopy.ipAddr;
    portNum = toCopy.portNum;
}

/**
 * Assignment operator overload for SocketWrapper. THIS DOES NOT COPY THE MUTEX
 * STATE!!!! Instead, the returned SocketWrapper instance will have ITS OWN
 * MUTEX. The socketPtr, ipAddr, and portNum members are copied using their
 * respective assignment operators.
 */
SocketWrapper& SocketWrapper::operator=(const SocketWrapper& base)
{
    socketPtr = base.socketPtr;
    ipAddr = base.ipAddr;
    portNum = base.portNum;
    return *this;
}

/**
 * Equality operator overload, primarily needed for list.remove().
 * Note that THIS ONLY CHECKS EQUALITY AGAINST THE socketPtr, ipAddr,
 * and port VALUES
 */
bool SocketWrapper::operator==(const SocketWrapper& other)
{
    if (socketPtr != other.socketPtr)
    {
        return false;
    }
    if (ipAddr != other.ipAddr)
    {
        return false;
    }
    if (portNum != other.portNum)
    {
        return false;
    }
    return true;
}

/**
 * Returns a reference to the boost::asio::ip::tcp::socket (TcpSocket) pointed
 * to by socketPtr.
 */
TcpSocket& SocketWrapper::getSocket()
{
    return *socketPtr;
}

/**
 * Returns a reference to the writeMutex member.
 */
std::mutex& SocketWrapper::getWriteMutex()
{
    return writeMutex;
}

/**
 * Returns a const reference to this SocketWrapper's IP address
 */
const std::string& SocketWrapper::getIpAddress()
{
    return ipAddr;
}

/**
 * Return this SocketWrapper's port number
 */
uint16_t SocketWrapper::getPortNumber()
{
    return portNum;
}

/**
 * Sends the data stored in dataToSend to the client associated with this
 * socket. The call to write() is guarded with this SocketWrapper's writeMutex
 */
size_t SocketWrapper::sendData(const std::string& dataToSend)
{
    // Lock the write operation
    std::lock_guard<std::mutex> writeLock { writeMutex };
    return boost::asio::write(*socketPtr, boost::asio::buffer(dataToSend));
}

/**
 * Reads data from this SocketWrapper's socket until the charToReadUntil
 * character is read, and stores the data in readBuff.
 * Note that during the read_until() call, it's possible that more characters
 * than just up to and including charToReadUntil will be received and stored in
 * readBuff. In such a case, bytesReceived contains only the number of bytes
 * read in the data up to and including charToReadUntil, although the excess
 * data will remain in the buffer. When this function is called, if readBuff
 * contains <some data>charToReadUntil, it will return immediately.
 * The data up to and including charToReadUntil is then stored in receivedData
 * and removed from readBuff, and this number of bytes is returned.
 */
size_t SocketWrapper::receiveData(std::string& receivedData,
        BoostStreamBuff& readBuff, const std::string& charToReadUntil)
{

    // Read until charToReadUntil has been reached
    size_t bytesReceived = boost::asio::read_until(*socketPtr, readBuff,
            charToReadUntil);

    // Create a stream to read data out of the buffer
    std::istream readBuffInputStream(&readBuff);

    // Pull the new message out of the buffer and store it in receivedData
    // (including the charToReadUntil delimiter)
    std::getline(readBuffInputStream, receivedData);

    return bytesReceived;
}

/*
 * Wrapper to call sendData() which doesn't return the number of bytes sent.
 * Instead, this function will throw a std::runtime_exception if
 * dataToSend.size() bytes weren't sent.
 */
void SocketWrapper::sendDataVoidReturn(const std::string& dataToSend)
{
    auto bytesActuallySent = sendData(dataToSend);
    if (bytesActuallySent != dataToSend.size())
    {
        throw std::runtime_error("Not all data written to socket");
    }
}

/**
 * Returns the client's IP address and port number in the form
 * <IP Addr>:<Port>
 */
const std::string SocketWrapper::getIpAddressAndPort()
{
    std::string ipAndPort = "";
    ipAndPort += ipAddr + ":" + std::to_string(portNum);
    return ipAndPort;
}
