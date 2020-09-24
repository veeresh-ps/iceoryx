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

#include "ac3log/simplelogger.hpp"
#include "base/base.hpp"
#include "config/config.hpp"
#include "iceoryx_utils/cxx/convert.hpp"
#include "udp/udp.hpp"


#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

void clientLoop(perftest::base::PerfTestClient& client, int64_t numRoundtrips)
{
    client.init();

    client.syncWithServer();

    std::vector<double> latencyInMicroSeconds;
    // UDS fails for > 4kB payload in QNX
    const std::vector<uint32_t> payloadSizesInKB{1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
    for (const auto payloadSizeInKB : payloadSizesInKB)
    {
        std::cout << std::endl
                  << "Measurement for " << payloadSizeInKB << " kB payload ... " << std::endl
                  << std::endl
                  << std::flush;
        auto payloadSizeInBytes = payloadSizeInKB * perftest::base::ONE_KILOBYTE;

        auto latency = client.measureLatency(payloadSizeInBytes, numRoundtrips);

        latencyInMicroSeconds.push_back(latency);
    }

    client.stop();

    client.shutdown();

    std::cout << std::endl;
    std::cout << "#### Measurement Result ####" << std::endl;
    std::cout << numRoundtrips << " round trips for each payload." << std::endl;
    std::cout << std::endl;
    std::cout << "| Payload Size [kB] | Average Latency [µs] |" << std::endl;
    std::cout << "|------------------:|---------------------:|" << std::endl;
    for (size_t i = 0; i < latencyInMicroSeconds.size(); ++i)
    {
        std::cout << "| " << std::setw(17) << payloadSizesInKB.at(i) << " | " << std::setw(20) << std::setprecision(2)
                  << latencyInMicroSeconds.at(i) << " |" << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Finished!" << std::endl;
}

int main(int argc, char* argv[])
{
    perftest::config::ConfigData data;

    if (argc > 1)
    {
        if (!iox::cxx::convert::fromString(argv[1], data.numRoundtrips))
        {
            ERR_PRINTF("error command line parameter");
            exit(1);
        }
    }

    //INFO_PRINTF("****** UDP - Loopback ********");
    //perftest::udp::UDPClient udp_loopback(data.ip, data.port);
    //clientLoop(udp_loopback, data.numRoundtrips);

    INFO_PRINTF("*********************************");
    INFO_PRINTF("****** UDP - interdomain ********");
    data.ip = "10.0.2.5";
    perftest::udp::UDPClient udp_interdomain(data.ip, data.port);
    clientLoop(udp_interdomain, data.numRoundtrips);


    // ara-com methods (IPC)

    // ara-com methods (intradomain)

    // ara-com event

    return (EXIT_SUCCESS);
}
