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
#include <ctime>
#include <ifaddrs.h>
#include <iostream>
#include <list>
#include <mutex>
#include <netdb.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <system_error>
#include <thread>

// Project Includes
#include "CommandParser.hpp"
#include "RtlFmParameterBuilder.hpp"
#include "SocketWrapper.hpp"
#include "TcpServer.hpp"

// Static Initialization

/**
 * When a client has run EXECUTE, this string followed by a datestamp will be
 * sent to the client
 */
const std::string TcpServer::UPDATED_PARAMETERS_AVAILABLE_STRING_PREFIX = "~UPDATE_AVAILABLE: ";

/**
 * All responses sent from the server end with this string
 */
const std::string TcpServer::END_OF_RESPONSE_STRING = "~EOR";

/**
 * Default on-connect password for this client
 */
const char * const TcpServer::DEFAULT_PASSWORD = "rtlsdrd";

/**
 * Each socket read operation will read up to the newline specifier
 */
const std::string TcpServer::SOCKET_READ_UNTIL_END_SPECIFIER = "\n";

/**
 * Sent when the password is first requested
 */
const std::string TcpServer::REQUESTING_PASSWORD_STRING_PREFIX = "~PASSWORD_REQ";

/**
 * Tells the client their password was correct
 */
const std::string TcpServer::AUTH_SUCCESSFUL_STRING = "~AUTH_SUCCESSFUL";

/**
 * Tells the client their password was invalid
 */
const std::string TcpServer::AUTH_FAILED_STRING = "~AUTH_FAILED";

/**
 * Constructs a new TcpServer instance. If nullptr is specified as the
 * password parameter, then clients won't need to provide a password to
 * interact with this server. If the password parameter is a nonnull string,
 * then the user will be required to provide a password in order to interact
 * with this server.
 */
TcpServer::TcpServer(const uint16_t port, const char * const password) :
        port(port), requirePassword(password ? true : false),
        password(password ? password : ""),
        ioService(),
        acceptor(ioService, BoostTcp::endpoint(BoostTcp::v4(), port), true)
{
};

/**
 * Builds a string with this machine's hostname, the port this server is
 * running on, and a list of all NICs in this machine and their IP addresses
 * (IPv4 and IPv6)
 */
std::string TcpServer::getServerInfo()
{
    std::string netInfo = "\nIPs: ";
    // Retrieve local network interfaces by name and IP
    for (std::string interfaceAndIp : getLocalIpAddresses())
    {
        netInfo.append("\n\t" + interfaceAndIp);
    }
    netInfo.append("\nHost: "  + boost::asio::ip::host_name());
    netInfo.append("\nPort: " + std::to_string(port));

    if (requirePassword)
    {
        netInfo.append("\nPassword: " + password);
    }

    return netInfo;
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
 * Performs authentication for new clients. This function requests a password
 * from the client, and if the proper password is provided, true is returned.
 * If an invalid password is provided, false is returned.
 */
bool TcpServer::authenticate(SocketWrapper& socketWrap, BoostStreamBuff& socketReadStreamBuff)
{
    socketWrap.sendData(REQUESTING_PASSWORD_STRING_PREFIX +
            ": Provide the password to access this server\n" +
            END_OF_RESPONSE_STRING + "\n");

    std::string receivedData;

    // Get the password supplied by the client
    socketWrap.receiveData(receivedData, socketReadStreamBuff, SOCKET_READ_UNTIL_END_SPECIFIER);

    std::cout << socketWrap.getIpAddressAndPort() <<
            " responded to password prompt with: " << receivedData << std::endl;

    // The substring gets rid of the trailing newline in the from-socket data
    if (password.compare(receivedData.substr(0, receivedData.size()-1)) == 0)
    {
        std::cout << socketWrap.getIpAddressAndPort() << " authenticated successfully" << std::endl;
        socketWrap.sendData(AUTH_SUCCESSFUL_STRING + "\n" + END_OF_RESPONSE_STRING + "\n");
        return true;
    }
    else
    {
        std::cout << socketWrap.getIpAddressAndPort() << " provided the wrong password" << std::endl;
        socketWrap.sendData(AUTH_FAILED_STRING + "\n" + END_OF_RESPONSE_STRING + "\n");
        return false;
    }
}

/**
 * Interacts with a connected client. This function represents is executed by
 * a thread which is dedicated to a single client. It will receive and parse
 * strings from the client, and will return status information to it.
 * When this function terminates, the SocketWrapper parameter is removed
 * from the socketWrappersInUse list.
 */
void TcpServer::connectionHandler(SocketWrapper& sockWrap)
{
    const std::string& clientIp = sockWrap.getIpAddress();
    uint16_t clientPort = sockWrap.getPortNumber();
    std::cout << "New client: " << clientIp << ":" << clientPort << std::endl;

    try
    {
        // Used by receivedData to buffer data from the socket
        BoostStreamBuff socketReadStreamBuff;

        // Request a password from the client if this instance of rtlsdrd
        // requires a password, kill the connection if their response is invalid
        if (requirePassword && !authenticate(sockWrap, socketReadStreamBuff))
        {
            removeFromSocketWrappersInUse(sockWrap);
            return;
        }

        // The parameter builder and parser are used for parsing and executing
        // commands sent by the client
        RtlFmParameterBuilder rtlFmWrapper;
        const CommandParser& parser = CommandParser::getInstance();

        while (true)
        {
            // Receive data from the client
            std::string receivedData;
            uint32_t bytesReceived = sockWrap.receiveData(receivedData,
                                         socketReadStreamBuff,
                                         SOCKET_READ_UNTIL_END_SPECIFIER);

            std::cout << "Received " << bytesReceived << " bytes from: "
                      << clientIp << ":" << clientPort
                      << "\n\tcontent: " << receivedData << std::endl;

            // Parse and execute the command/data from the client
            std::string parseResult = parser.execute(receivedData, rtlFmWrapper);
            parseResult.append("\n" + END_OF_RESPONSE_STRING + "\n");

            // Send the data back to the client
            size_t bytesSent = sockWrap.sendData(parseResult);

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
    removeFromSocketWrappersInUse(sockWrap);
}

/**
 * Allows this server to receive connections. All new connections execute
 * this->connectionHandler in a new thread, and all connected clients
 * are associated with a SocketWrapper instance which is added to
 * socketWrappersInUse.
 */
void TcpServer::run()
{
    std::cout << "Rtlsdrd server started on:" << getServerInfo() << std::endl;

    while (true)
    {
      TcpSocketSharedPtr tcpSocketPtr(new TcpSocket(ioService));
      acceptor.accept(*tcpSocketPtr);

      // Create a new SocketWrapper for this client, add it to
      // socketWrappersInUse, and send its reference as a param to the client
      // thread.
      boost::thread clientHandler(boost::bind(&TcpServer::connectionHandler, this,
              boost::ref(appendToSocketWrappersInUse(tcpSocketPtr))));
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
 * socketWrappersInUse list. This access to socketWrappersInUse is locked
 * with the socketWrappersInUseMutex mutex. A reference to the newly created
 * SocketWrapper, within the list, is returned.
 */
SocketWrapper& TcpServer::appendToSocketWrappersInUse(TcpSocketSharedPtr& newSocket)
{
    // Lock access to socketWrappersInUse
    std::lock_guard<std::mutex> lock{socketWrappersInUseMutex};

    socketWrappersInUse.push_back(SocketWrapper(newSocket));

    return socketWrappersInUse.back();
}

/**
 * Removes the socketWrapper parameter from the socketWrappersInUse list.
 * The socketWrappersInUseMutex is used to lock this access to the list.
 * If the SocketWrapper parameter wasn't actually deleted from the list,
 * a std::runtime_error is thrown.
 */
void TcpServer::removeFromSocketWrappersInUse(const SocketWrapper& socketWrapper)
{
    // Lock access to socketWrappersInUse
    std::lock_guard<std::mutex> lock{socketWrappersInUseMutex};

    auto preDeleteSize = socketWrappersInUse.size();

    // Remove the socketWrapper param
    socketWrappersInUse.remove(socketWrapper);

    // If the size hasn't changed, then there must've been an error deleting the value
    if (preDeleteSize-socketWrappersInUse.size() != 1)
    {
        throw std::runtime_error("Attempt to remove socket from"
                " socketWrappersInUse resulted in nonchanging list size;"
                " socketWrapper not deleted successfully");
    }
}

/**
 * Iterates over the socketWrappersInUse list and writes the string contained
 * in UPDATED_PARAMETERS_AVAILABLE_STRING, with an additional timestamp, to the
 * socket corresponding to each element in the list. Each socket's write
 * operation is performed in a new thread.
 */
void TcpServer::informAllClientsOfStateChange()
{
    std::time_t currTime = std::time(nullptr);

    // Send out the update message string + a timestamp
    const std::string updateMsg = UPDATED_PARAMETERS_AVAILABLE_STRING_PREFIX +
            std::asctime(std::localtime(&currTime));

    for (SocketWrapper& socketWrapper : socketWrappersInUse)
    {
        boost::thread sendMsgThread(&SocketWrapper::sendDataVoidReturn,
                socketWrapper, updateMsg);
    }
}

/**
 * Used to instantiate and access the TcpServer singleton. When this function
 * is first executed, a new TcpServer is created using the specified port.
 * All subsequent calls to this function will return a reference to the
 * original instance, AND WILL IGNORE the contents of the parameter.
 *
 * If this function is called for the first time with nullptr as the parameter,
 * a std::invalid_argument is thrown.
 */
TcpServer& TcpServer::getInstance(const TcpServerBuilder* tcpServerBuilder)
{
    static TcpServerUniquePtr server = nullptr;

    if (!server && !tcpServerBuilder)
    {
        throw std::invalid_argument("Attempted to construct TcpServer using"
                " nullptr");
    }
    else if (!server)
    {
        server = tcpServerBuilder->build();
    }
    return *server;
}

/**
 * For the SVR_ADDR command, retrieve a list of this server's NICs (and their
 * IPs), hostname, and port, and append them to clientReturnableInfo
 */
void TcpServer::getAddressInfoHandler(const std::string& UNUSED, std::string* clientReturnableInfo)
{
    (void) UNUSED;
    clientReturnableInfo->append(getServerInfo());
    clientReturnableInfo->append("\n");
}

/**
 * Appends to clientReturnableInfo each connected client's IP address and
 * port number. Access to socketWrappersInUse is locked using the
 * socketWrappersInUseMutex.
 */
void TcpServer::getClientsInfoHandler(const std::string& UNUSED,
        std::string* clientReturnableInfo)
{
    (void) UNUSED;

    // Lock access to socketWrappersInUse
    std::lock_guard<std::mutex> lock{socketWrappersInUseMutex};

    for (SocketWrapper socketWrapper : socketWrappersInUse)
    {
        clientReturnableInfo->append("\nIP: " + socketWrapper.getIpAddress());
        clientReturnableInfo->append("; Port: " + std::to_string(socketWrapper.getPortNumber()));
    }
}

/**
 * Specifies the TCP port number which this rtlsdrd instance will communicate
 * through
 */
TcpServer::TcpServerBuilder& TcpServer::TcpServerBuilder::withPort(uint16_t port)
{
    this->port = port;
    return *this;
}

/**
 * Specifies the password which this rtlsdrd instance will require clients to
 * provide before they can gain control access
 */
TcpServer::TcpServerBuilder& TcpServer::TcpServerBuilder::withPassword(const std::string& password)
{
    usePassword = true;
    this->password = password;
    return *this;
}

/**
 * Dynamically allocates a new TcpServer instance, containing it within a
 * std::unique_ptr, which is returned
 */
TcpServerUniquePtr TcpServer::TcpServerBuilder::build() const
{
    if (usePassword)
    {
        return TcpServerUniquePtr(new TcpServer(port, password.c_str()));
    }
    else
    {
        return TcpServerUniquePtr(new TcpServer(port, nullptr));
    }
}

/**
 * Specifies that users connecting to rtlsdrd will not need to provide a
 * password
 */
TcpServer::TcpServerBuilder& TcpServer::TcpServerBuilder::withoutPassword()
{
    usePassword = false;
    return *this;
}
