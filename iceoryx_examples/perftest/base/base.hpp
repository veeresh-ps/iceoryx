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

#ifndef COM_PERFTESTS_PERFTEST_BASE_HPP
#define COM_PERFTESTS_PERFTEST_BASE_HPP

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

namespace perftest
{
namespace base
{
struct PerfTopic
{
    uint32_t payloadSize{0};
    bool run{true};
};

static constexpr uint32_t ONE_KILOBYTE = 1024U;

class PerfTestBase
{
  public:
    virtual void init() noexcept = 0;
    virtual void shutdown() noexcept = 0;

  protected:
    // Send desired payload to partner
    // runFlag : Flag to control start/stop of the server
    // client sets this flag & server responds to this flag
    virtual int64_t sendPerfTopic(uint32_t payloadSizeInBytes, bool runFlag) noexcept = 0;
    // Blocks until receives response message from server
    virtual PerfTopic receivePerfTopic() noexcept = 0;
};

class PerfTestClient : public PerfTestBase
{
  public:
    PerfTestClient() = default;
    // This is blocking call, returns after ping pong to server is successful
    bool syncWithServer() noexcept;
    double measureLatency(int64_t payLoadSize, int64_t numRoundtrips) noexcept;
    void stop() noexcept;
};

class PerfTestServer : public PerfTestBase
{
  public:
    PerfTestServer() = default;
    // Receive frame of specific length & respond immediately with the same length
    void pingPong() noexcept;
};
} // namespace base
} // namespace perftest

#endif // COM_PERFTESTS_PERFTEST_BASE_HPP