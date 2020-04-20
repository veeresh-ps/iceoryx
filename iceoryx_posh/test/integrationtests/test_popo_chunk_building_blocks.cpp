// Copyright (c) 2020 by Robert Bosch GmbH. All rights reserved.
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

#include "iceoryx_posh/internal/popo/building_blocks/chunk_distributor.hpp"
#include "iceoryx_posh/internal/popo/building_blocks/chunk_receiver.hpp"
#include "iceoryx_posh/internal/popo/building_blocks/chunk_sender.hpp"
#include "iceoryx_posh/mepoo/mepoo_config.hpp"

#include "test.hpp"

#include <chrono>
#include <stdlib.h>
#include <thread>

using namespace ::testing;
using namespace iox::popo;
using namespace iox::cxx;
using namespace iox::mepoo;
using ::testing::Return;

struct DummySample
{
    uint64_t dummy{42};
};

static constexpr size_t MEMORY_SIZE = 1024 * 1024;
uint8_t m_memory[MEMORY_SIZE];
static constexpr uint32_t NUM_CHUNKS_IN_POOL = 100;
static constexpr uint32_t MAGIC_NUMBER_ITERATIONS = 10000;
static constexpr uint32_t SMALL_CHUNK = 128;
static constexpr uint32_t BIG_CHUNK = 256;
static constexpr uint64_t HISTORY_CAPACITY = 4;
static constexpr uint32_t MAX_NUMBER_QUEUES = 128;

iox::posix::Allocator m_memoryAllocator{m_memory, MEMORY_SIZE};
iox::mepoo::MePooConfig m_mempoolconf;
iox::mepoo::MemoryManager m_memoryManager;

using ChunkDistributorData_t = iox::popo::ChunkDistributorData<MAX_NUMBER_QUEUES, iox::popo::ThreadSafePolicy>;
using ChunkDistributor_t = iox::popo::ChunkDistributor<ChunkDistributorData_t>;

// Publishing part
iox::popo::ChunkSenderData<ChunkDistributorData_t> m_chunkSenderData{&m_memoryManager};
iox::popo::ChunkSender<ChunkDistributor_t> m_chunkSender{&m_chunkSenderData};

// Forwarding part
ChunkDistributorData_t m_chunkDistributorData;
ChunkDistributor_t m_chunkDistributor{&m_chunkDistributorData};
ChunkQueueData m_chunkData{iox::cxx::VariantQueueTypes::SoFi_SingleProducerSingleConsumer};
ChunkQueuePopper m_popper{&m_chunkData};

// Subscribing part
iox::popo::ChunkReceiverData m_chunkReceiverData{iox::cxx::VariantQueueTypes::SoFi_SingleProducerSingleConsumer};
iox::popo::ChunkReceiver m_chunkReceiver{&m_chunkReceiverData};

void publish()
{
    for (size_t i = 0; i < MAGIC_NUMBER_ITERATIONS; i++)
    {
        auto maybeChunkHeader = m_chunkSender.allocate(sizeof(DummySample));
        EXPECT_FALSE(maybeChunkHeader.has_error());

        if (!maybeChunkHeader.has_error())
        {
            auto sample = (*maybeChunkHeader)->payload();
            new (sample) DummySample();
            static_cast<DummySample*>(sample)->dummy = i;
            m_chunkSender.send(*maybeChunkHeader);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        // std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 10));
    }
}

void forward()
{
    while (1)
    {
        /// @todo Add some jitter to make threads breath
        // std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 10));
        auto maybeSharedChunk = m_popper.pop();
        if (maybeSharedChunk.has_value())
        {
            m_chunkDistributor.deliverToAllStoredQueues(maybeSharedChunk.value());
        }
    }
}

void subscribe()
{
    uint64_t counter{0};

    while (true)
    {
        /// @todo Add some jitter to make threads breath
        // std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 10));
        auto maybeChunkHeader = m_chunkReceiver.get();
        if (!maybeChunkHeader.has_error() && maybeChunkHeader.get_value().has_value())
        {
            auto chunkHeader = maybeChunkHeader.get_value().value();

            auto dummySample = *reinterpret_cast<DummySample*>(chunkHeader->payload());
            EXPECT_THAT(dummySample.dummy, Eq(counter));
            if (MAGIC_NUMBER_ITERATIONS - 1 == counter)
            {
                break;
            }
            counter++;
            m_chunkReceiver.release(chunkHeader);
        }
    }
}

/// @todo Remove the fixture (TEST() instead of TEST_F()), make it a typed test? Different timings? Different setups?
class ChunkBuildingBlocks_IntegrationTest : public Test
{
  public:
    ChunkBuildingBlocks_IntegrationTest()
    {
        m_mempoolconf.addMemPool({SMALL_CHUNK, NUM_CHUNKS_IN_POOL});
        m_memoryManager.configureMemoryManager(m_mempoolconf, &m_memoryAllocator, &m_memoryAllocator);
    }
    virtual ~ChunkBuildingBlocks_IntegrationTest()
    {
    }

    void SetUp()
    {
        m_chunkSender.addQueue(&m_chunkData);
        m_chunkDistributor.addQueue(&m_chunkReceiverData);
    }

    void TearDown()
    {
    }
};

TEST_F(ChunkBuildingBlocks_IntegrationTest, TwoHopsThreeThreads)
{
    std::thread publishingThread(publish);
    std::thread forwardingThread(forward);
    std::thread subscribingThread(subscribe);

    // Just detach while (true)
    forwardingThread.detach();

    if (subscribingThread.joinable())
    {
        subscribingThread.join();
    }

    if (publishingThread.joinable())
    {
        publishingThread.join();
    }

    /// @todo Use non global objects? We need to make sure that the d'tors of all objects have been called excluding the
    /// memory manager
    // One chunk is on hold due to the fact that chunkSender and chunkDistributor hold last chunk
    EXPECT_THAT(m_memoryManager.getMemPoolInfo(0).m_usedChunks, Eq(1));
}
