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

#ifndef COM_PERFTESTS_UDP_HPP
#define COM_PERFTESTS_UDP_HPP

#include "base/base.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
namespace perftest
{
namespace udp
{
using PerfTopic = perftest::base::PerfTopic;

static constexpr int32_t ERROR_CODE = -1;
static constexpr int32_t INVALID_FD = -1;
// Using value greater than 64kB causes memory error , resulting in program crash
static constexpr uint32_t MAX_MESSAGE_SIZE = 64 * perftest::base::ONE_KILOBYTE;

static constexpr int32_t DEFAULT_PORT = 8080;
static constexpr char const* DEFAULT_IP = "127.0.0.1";


class UDPCommon
{
  protected:
    ssize_t send(const char* buffer,
                 uint32_t length,
                 const struct sockaddr_in* destAddress,
                 socklen_t addressLength = sizeof(sockaddr_in)) noexcept;
    ssize_t receive(char* buffer, struct sockaddr_in* sourceAddress, socklen_t* sourceAddressLength) noexcept;

    int m_socket{INVALID_FD};
    // @@todo : to increase the buffer capacity, try to make this static
    char m_message[MAX_MESSAGE_SIZE];
};

class UDPClient : public perftest::base::PerfTestClient, public UDPCommon
{
  public:
    UDPClient() = default;
    UDPClient(char const* server_ip, uint32_t server_port);

  protected:
    // these APIs could be moved to UDPCommon
    int64_t sendPerfTopic(uint32_t payloadSizeInBytes, bool runFlag) noexcept override;
    PerfTopic receivePerfTopic() noexcept override;
    void init() noexcept override;
    void shutdown() noexcept override;

  private:
    struct sockaddr_in m_sockAddrServer;
    const uint32_t m_serverPort{DEFAULT_PORT};
    char const* m_serverIp{DEFAULT_IP};
};

class UDPServer : public perftest::base::PerfTestServer, public UDPCommon
{
  public:
    UDPServer() = default;
    UDPServer(uint32_t localPort);

  protected:
    // these APIs could be moved to UDPCommon
    int64_t sendPerfTopic(uint32_t payloadSizeInBytes, bool runFlag) noexcept override;
    PerfTopic receivePerfTopic() noexcept override;
    void init() noexcept override;
    void shutdown() noexcept override;

  private:
    struct sockaddr_in m_sourceAddress;
    struct sockaddr_in m_sockAddrLocal;
    socklen_t m_length{sizeof(sockaddr_in)};
    const uint32_t m_localPort{DEFAULT_PORT};
};
} // namespace udp
} // namespace perftest


#endif // COM_PERFTESTS_UDP_HPP