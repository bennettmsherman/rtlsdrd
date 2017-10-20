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
SocketWrapper::SocketWrapper(TcpSocketSharedPtr& socketPtr) : socketPtr(socketPtr),
    ipAddr(this->socketPtr->remote_endpoint().address().to_string()),
    portNum(this->socketPtr->remote_endpoint().port())
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
