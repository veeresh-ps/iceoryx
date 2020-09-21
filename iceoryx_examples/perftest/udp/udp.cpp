// Copyright (c) 2019 by Robert Bosch GmbH. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "udp.hpp"
#include "iceoryx_utils/cxx/smart_c.hpp"

namespace perftest
{
namespace udp
{
ssize_t UDPCommon::receive(char* buffer, struct sockaddr_in* sourceAddress, socklen_t* sourceAddressLength) noexcept
{
    // @@todo Implement some kind of timeout, if server is not responding
    auto recvCall = iox::cxx::makeSmartC(recvfrom,
                                         iox::cxx::ReturnMode::PRE_DEFINED_ERROR_CODE,
                                         {ERROR_CODE},
                                         {},
                                         m_socket,
                                         buffer,
                                         MAX_MESSAGE_SIZE,
                                         0,
                                         reinterpret_cast<struct sockaddr*>(sourceAddress),
                                         sourceAddressLength);

    if (recvCall.hasErrors())
    {
        std::cout << "ERROR : receive error" << std::endl;
        exit(1);
    }
    return recvCall.getReturnValue();
}


ssize_t UDPCommon::send(const char* buffer,
                        uint32_t length,
                        const struct sockaddr_in* destAddress,
                        socklen_t addressLength) noexcept
{
    auto sendCall = iox::cxx::makeSmartC(sendto,
                                         iox::cxx::ReturnMode::PRE_DEFINED_ERROR_CODE,
                                         {ERROR_CODE},
                                         {},
                                         m_socket,
                                         buffer,
                                         length,
                                         static_cast<int>(0),
                                         reinterpret_cast<const struct sockaddr*>(destAddress),
                                         addressLength);

    if (sendCall.hasErrors())
    {
        std::cout << std::endl << "ERROR : send error" << std::endl;
    }
    return sendCall.getReturnValue();
}

UDPClient::UDPClient(char const* server_ip, uint32_t server_port)
    : m_serverIp{server_ip}
    , m_serverPort{server_port}

{
}

void UDPClient::init() noexcept
{
    memset(&m_sockAddrServer, 0, sizeof(m_sockAddrServer));
    m_sockAddrServer.sin_family = AF_INET;
    // Filling server information
    m_sockAddrServer.sin_port = htons(m_serverPort);
    m_sockAddrServer.sin_addr.s_addr = inet_addr(m_serverIp);

    auto socketCall = iox::cxx::makeSmartC(
        socket, iox::cxx::ReturnMode::PRE_DEFINED_ERROR_CODE, {ERROR_CODE}, {}, AF_INET, SOCK_DGRAM, 0);

    if (socketCall.hasErrors())
    {
        std::cout << "socket error" << std::endl;
        exit(1);
    }

    m_socket = socketCall.getReturnValue();
}


int64_t UDPClient::sendPerfTopic(uint32_t payloadSizeInBytes, bool runFlag) noexcept
{
    auto sample = reinterpret_cast<PerfTopic*>(&m_message[0]);

    sample->payloadSize = payloadSizeInBytes;
    sample->run = runFlag;

    if (payloadSizeInBytes <= MAX_MESSAGE_SIZE)
    {
        return send(m_message, payloadSizeInBytes, (&m_sockAddrServer));
    }
    else
    {
        std::cout << "ERROR : Message larger than supported size" << std::endl;
        return -1;
    }
}

PerfTopic UDPClient::receivePerfTopic() noexcept
{
    auto length = receive(&m_message[0], nullptr, nullptr);

    auto receivedSample = reinterpret_cast<const PerfTopic*>(&m_message[0]);

    if (length != receivedSample->payloadSize)
    {
        std::cout << "ERROR : received data length & expected data length do not match" << std::endl;
    }

    return *receivedSample;
}

void UDPClient::shutdown() noexcept
{
    close(m_socket);
    std::cout << "Shutting down client " << std::endl;
}

UDPServer::UDPServer(uint32_t localPort)
    : m_localPort{localPort}
{
}

void UDPServer::init() noexcept
{
    auto socketCall = iox::cxx::makeSmartC(
        socket, iox::cxx::ReturnMode::PRE_DEFINED_ERROR_CODE, {ERROR_CODE}, {}, AF_INET, SOCK_DGRAM, 0);

    if (socketCall.hasErrors())
    {
        std::cout << "ERROR : socket error" << std::endl;
        exit(1);
    }

    m_socket = socketCall.getReturnValue();

    memset(&m_sourceAddress, 0, sizeof(m_sourceAddress));
    memset(&m_sockAddrLocal, 0, sizeof(m_sockAddrLocal));

    // Local address for binding socket
    m_sockAddrLocal.sin_family = AF_INET;
    m_sockAddrLocal.sin_addr.s_addr = INADDR_ANY;
    m_sockAddrLocal.sin_port = htons(m_localPort);

    auto bindCall = iox::cxx::makeSmartC(bind,
                                         iox::cxx::ReturnMode::PRE_DEFINED_ERROR_CODE,
                                         {ERROR_CODE},
                                         {},
                                         m_socket,
                                         (const struct sockaddr*)&m_sockAddrLocal,
                                         sizeof(m_sockAddrLocal));

    if (bindCall.hasErrors())
    {
        std::cout << "ERROR : socket bind " << std::endl;
        exit(1);
    }
}

int64_t UDPServer::sendPerfTopic(uint32_t payloadSizeInBytes, bool runFlag) noexcept
{
    auto sample = reinterpret_cast<PerfTopic*>(&m_message[0]);

    sample->payloadSize = payloadSizeInBytes;
    sample->run = runFlag;

    if (payloadSizeInBytes <= MAX_MESSAGE_SIZE)
    {
        return send(m_message, payloadSizeInBytes, (&m_sourceAddress));
    }
    else
    {
        std::cout << "ERROR : Message larger than supported size" << std::endl;
        return -1;
    }
}

PerfTopic UDPServer::receivePerfTopic() noexcept
{
    auto length = receive(&m_message[0], &m_sourceAddress, &m_length);

    auto receivedSample = reinterpret_cast<const PerfTopic*>(&m_message[0]);

    if (length != receivedSample->payloadSize)
    {
        std::cout << "ERROR : received data length & expected data length do not match" << std::endl;
    }
    return *receivedSample;
}

void UDPServer::shutdown() noexcept
{
    close(m_socket);
    std::cout << "Shutting down server " << std::endl;
}
} // namespace udp
} // namespace perftest
