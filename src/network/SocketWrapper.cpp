/*
 * SocketWrapper.cpp
 *
 *  Created on: Oct 18, 2017
 *      Author: bensherman
 */

// System Includes
#include <boost/asio.hpp>
#include <mutex>

// Project Includes
#include "SocketWrapper.hpp"

/**
 * Constructs a new SocketWrapper with the socketPtr parameter used
 * to instantiate the socketPtr member. writeMutex is instantiated using its
 * default constructor.
 */
SocketWrapper::SocketWrapper(TcpSocketSharedPtr& socketPtr) : socketPtr(socketPtr)
{

}

/**
 * Copy constructor for SocketWrapper. DOES NOT COPY THE MUTEX STATE!!!
 * That is, the new SocketWrapper will have a new mutex.
 */
SocketWrapper::SocketWrapper(const SocketWrapper& toCopy)
{
    socketPtr = toCopy.socketPtr;
}

/**
 * Assignment operator overload for SocketWrapper. THIS DOES NOT COPY THE MUTEX
 * STATE!!!! Instead, the returned SocketWrapper instance will have ITS OWN
 * MUTEX. The socketPtr parameter is shared across both the base and returned
 * instances.
 */
SocketWrapper& SocketWrapper::operator=(const SocketWrapper& base)
{
    socketPtr = base.socketPtr;
    return *this;
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
 * Equality operator overload, primarily needed for list.remove().
 * Note that THIS ONLY CHECKS EQUALITY AGAINST THE socketPtr VALUES
 */
bool SocketWrapper::operator==(const SocketWrapper& base)
{
    return socketPtr == base.socketPtr;
}
