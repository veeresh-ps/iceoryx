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

#include "base.hpp"
#include "ac3log/simplelogger.hpp"

namespace perftest
{
namespace base
{
bool PerfTestClient::syncWithServer() noexcept
{
    sendPerfTopic(sizeof(PerfTopic), true);
    receivePerfTopic();
    INFO_PRINTF("synced with server");
    return true;
}

double PerfTestClient::measureLatency(int64_t payLoadSize, int64_t numRoundtrips) noexcept
{
    // test if requested payload is possible
    if (sendPerfTopic(payLoadSize, true) == -1)
    {
        ERR_PRINTF("Lower layer doesnt support payload %d", payLoadSize);
        return 0;
    }
    else
    {
        // Receive the response to request, in case test passes
        receivePerfTopic();
    }

    auto start = std::chrono::high_resolution_clock::now();

    // run the performance test
    for (auto i = 0U; i < numRoundtrips; ++i)
    {
        sendPerfTopic(payLoadSize, true);
        auto perfTopic = receivePerfTopic();
    }

    auto finish = std::chrono::high_resolution_clock::now();

    constexpr int64_t TRANSMISSIONS_PER_ROUNDTRIP{2};
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start);
    auto latencyInNanoSeconds = (duration.count() / (numRoundtrips * TRANSMISSIONS_PER_ROUNDTRIP));
    auto latencyInMicroSeconds = static_cast<double>(latencyInNanoSeconds) / 1000;
    return latencyInMicroSeconds;
}

void PerfTestClient::stop() noexcept
{
    INFO_PRINTF("sending stop command to server");
    sendPerfTopic(sizeof(PerfTopic), false);
    receivePerfTopic();
}

void PerfTestServer::pingPong() noexcept
{
    while (true)
    {
        auto perfTopic = receivePerfTopic();

        sendPerfTopic(perfTopic.payloadSize, true);

        // stop replying when no more run
        if (!perfTopic.run)
        {
            break;
        }
    }
}
} // namespace base
} // namespace perftest
