// Licensed under the MIT License. Copyright (c) 2021 Gergely Pintér.
#ifndef SHARED_LIB_BUILDING
#   define SHARED_LIB_BUILDING
#endif
#include <Shared/Memory/AllocatorStatistic.hpp>

#include <stdexcept>

namespace Memory
{
    /*
        AllocatorStatistic definitions
    */

    AllocatorStatistic::AllocatorStatistic() :
        m_basic_backing(nullptr),
        m_object_backing(nullptr),
        m_smallest(std::numeric_limits<size_t>::max()),
        m_largest(0),
        m_highest(0),
        m_allocC(0),
        m_alloc_failC(0),
        m_alloc_largest_fail(0),
        m_allocB(0),
        m_reallocC(0),
        m_realloc_failC(0),
        m_realloc_growthB(0),
        m_realloc_shrinkB(0),
        m_realloc_moveC(0),
        m_realloc_moveB(0),
        m_offerC(0),
        m_offerB(0),
        m_reclaimC(0),
        m_reclaim_failC(0),
        m_reclaimB(0)
    {
    }

    AllocatorStatistic::AllocatorStatistic(BasicAllocator* backing) :
        AllocatorStatistic()
    {
        this->m_basic_backing = backing;
    }

    AllocatorStatistic::AllocatorStatistic(ObjectAllocator* backing)
        : AllocatorStatistic()
    {
        this->m_basic_backing = backing;
        this->m_object_backing = backing;
    }

    size_t AllocatorStatistic::getHighestUsage() const
    {
        return this->m_highest.load(std::memory_order_relaxed);
    }

    size_t AllocatorStatistic::getSmallestAlloc() const
    {
        size_t ret = this->m_smallest.load(std::memory_order_relaxed);
        if (ret != std::numeric_limits<size_t>::max())
        {
            return ret;
        }
        return 0;
    }

    size_t AllocatorStatistic::getLargestAlloc() const
    {
        return this->m_largest.load(std::memory_order_relaxed);
    }

    uintmax_t AllocatorStatistic::getTotalAllocs() const
    {
        return this->m_allocC.load(std::memory_order_relaxed);
    }

    uintmax_t AllocatorStatistic::getTotalAllocFails() const
    {
        return this->m_alloc_failC.load(std::memory_order_relaxed);
    }

    size_t AllocatorStatistic::getLargestAllocFailed() const
    {
        return this->m_alloc_largest_fail.load(std::memory_order_relaxed);
    }

    uintmax_t AllocatorStatistic::getTotalAllocBytes() const
    {
        return this->m_allocB.load(std::memory_order_relaxed);
    }

    uintmax_t AllocatorStatistic::getTotalReallocs() const
    {
        return this->m_reallocC.load(std::memory_order_relaxed);
    }

    uintmax_t AllocatorStatistic::getTotalReallocFails() const
    {
        return this->m_realloc_failC.load(std::memory_order_relaxed);
    }

    uintmax_t AllocatorStatistic::getTotalReallocGrowth() const
    {
        return this->m_realloc_growthB.load(std::memory_order_relaxed);
    }

    uintmax_t AllocatorStatistic::getTotalReallocShrink() const
    {
        return this->m_realloc_shrinkB.load(std::memory_order_relaxed);
    }

    uintmax_t AllocatorStatistic::getTotalReallocMoves() const
    {
        return this->m_realloc_moveC.load(std::memory_order_relaxed);
    }

    uintmax_t AllocatorStatistic::getTotalReallocMoved() const
    {
        return this->m_realloc_moveB.load(std::memory_order_relaxed);
    }

    uintmax_t AllocatorStatistic::getTotalOffers() const
    {
        return this->m_offerC.load(std::memory_order_relaxed);
    }

    uintmax_t AllocatorStatistic::getTotalOfferBytes() const
    {
        return this->m_offerB.load(std::memory_order_relaxed);
    }

    uintmax_t AllocatorStatistic::getTotalReclaims() const
    {
        return this->m_reclaimC.load(std::memory_order_relaxed);
    }

    uintmax_t AllocatorStatistic::getTotalReclaimFails() const
    {
        return this->m_reclaim_failC.load(std::memory_order_relaxed);
    }

    uintmax_t AllocatorStatistic::getTotalReclaimBytes() const
    {
        return this->m_reclaimB.load(std::memory_order_relaxed);
    }

    void AllocatorStatistic::resetCounters()
    {
        this->m_smallest = std::numeric_limits<size_t>::max();
        this->m_largest = 0;
        this->m_highest = 0;
        this->m_allocC = 0;
        this->m_alloc_failC = 0;
        this->m_alloc_largest_fail = 0;
        this->m_allocB = 0;
        this->m_reallocC = 0;
        this->m_realloc_failC = 0;
        this->m_realloc_growthB = 0;
        this->m_realloc_shrinkB = 0;
        this->m_realloc_moveC = 0;
        this->m_realloc_moveB = 0;
        this->m_offerC = 0;
        this->m_offerB = 0;
        this->m_reclaimC = 0;
        this->m_reclaim_failC = 0;
        this->m_reclaimB = 0;
    }

    /*
        Overridden wrapped function definitions
    */

    void* AllocatorStatistic::alloc(size_t bytes, size_t align)
    {
        size_t expected;
        this->m_allocC.fetch_add(1, std::memory_order_relaxed);

        try
        {
            void* ret = this->m_basic_backing->alloc(bytes, align);
            size_t current_use = this->getUsedBytes();

            if (ret != nullptr)
            {
                this->m_allocB.fetch_add(bytes, std::memory_order_relaxed);

                expected = this->m_largest.load(std::memory_order_relaxed);
                while (expected < bytes)
                    if (this->m_largest.compare_exchange_weak(expected, bytes, std::memory_order_release, std::memory_order_relaxed))
                        break;

                expected = this->m_smallest.load(std::memory_order_relaxed);
                while (expected > bytes)
                    if (this->m_smallest.compare_exchange_weak(expected, bytes, std::memory_order_release, std::memory_order_relaxed))
                        break;

                expected = this->m_highest.load(std::memory_order_relaxed);
                while (expected < current_use)
                    if (this->m_highest.compare_exchange_weak(expected, current_use, std::memory_order_release, std::memory_order_relaxed))
                        break;
            }
            else
            {
                this->m_alloc_failC.fetch_add(1, std::memory_order_relaxed);

                expected = this->m_alloc_largest_fail.load(std::memory_order_relaxed);
                while (expected < bytes)
                    if (this->m_alloc_largest_fail.compare_exchange_weak(expected, bytes, std::memory_order_release, std::memory_order_relaxed))
                        break;
            }
            return ret;
        }
        catch (...)
        {
            std::exception_ptr eptr = std::current_exception();

            this->m_alloc_failC.fetch_add(1, std::memory_order_relaxed);

            expected = this->m_alloc_largest_fail.load(std::memory_order_relaxed);
            while (expected < bytes)
                if (this->m_alloc_largest_fail.compare_exchange_weak(expected, bytes, std::memory_order_release, std::memory_order_relaxed))
                    break;

            std::rethrow_exception(eptr);
        }
    }

    void* AllocatorStatistic::alloc(size_t bytes, DestructorPtr destructor, size_t align)
    {
        size_t expected;
        this->m_allocC.fetch_add(1, std::memory_order_relaxed);

        try
        {
            void* ret = this->m_object_backing->alloc(bytes, destructor, align);
            size_t current_use = this->getUsedBytes();

            if (ret != nullptr)
            {
                this->m_allocB.fetch_add(bytes, std::memory_order_relaxed);

                expected = this->m_largest.load(std::memory_order_relaxed);
                while (expected < bytes)
                    if (this->m_largest.compare_exchange_weak(expected, bytes, std::memory_order_release, std::memory_order_relaxed))
                        break;

                expected = this->m_smallest.load(std::memory_order_relaxed);
                while (expected > bytes)
                    if (this->m_smallest.compare_exchange_weak(expected, bytes, std::memory_order_release, std::memory_order_relaxed))
                        break;

                expected = this->m_highest.load(std::memory_order_relaxed);
                while (expected < current_use)
                    if (this->m_highest.compare_exchange_weak(expected, current_use, std::memory_order_release, std::memory_order_relaxed))
                        break;
            }
            else
            {
                this->m_alloc_failC.fetch_add(1, std::memory_order_relaxed);

                expected = this->m_alloc_largest_fail.load(std::memory_order_relaxed);
                while (expected < bytes)
                    if (this->m_alloc_largest_fail.compare_exchange_weak(expected, bytes, std::memory_order_release, std::memory_order_relaxed))
                        break;
            }
            return ret;
        }
        catch (...)
        {
            std::exception_ptr eptr = std::current_exception();

            this->m_alloc_failC.fetch_add(1, std::memory_order_relaxed);

            expected = this->m_alloc_largest_fail.load(std::memory_order_relaxed);
            while (expected < bytes)
                if (this->m_alloc_largest_fail.compare_exchange_weak(expected, bytes, std::memory_order_release, std::memory_order_relaxed))
                    break;

            std::rethrow_exception(eptr);
        }
    }

    void AllocatorStatistic::free(void* ptr)
    {
        this->m_basic_backing->free(ptr);
    }

    void* AllocatorStatistic::realloc(void* ptr, size_t bytes, size_t align)
    {
        size_t expected;
        this->m_reallocC.fetch_add(1, std::memory_order_relaxed);

        try
        {
            size_t last_size = this->getAllocSize(ptr);
            
            void* ret = this->m_basic_backing->realloc(ptr, bytes, align);
            size_t current_use = this->getUsedBytes();

            if (ret != nullptr)
            {
                this->m_allocB.fetch_add(bytes, std::memory_order_relaxed);

                expected = this->m_largest.load(std::memory_order_relaxed);
                while (expected < bytes)
                    if (this->m_largest.compare_exchange_weak(expected, bytes, std::memory_order_release, std::memory_order_relaxed))
                        break;

                expected = this->m_smallest.load(std::memory_order_relaxed);
                while (expected > bytes)
                    if (this->m_smallest.compare_exchange_weak(expected, bytes, std::memory_order_release, std::memory_order_relaxed))
                        break;

                expected = this->m_highest.load(std::memory_order_relaxed);
                while (expected < current_use)
                    if (this->m_highest.compare_exchange_weak(expected, current_use, std::memory_order_release, std::memory_order_relaxed))
                        break;

                if (ret != ptr)
                {
                    this->m_realloc_moveC.fetch_add(1, std::memory_order_relaxed);
                    this->m_realloc_moveB.fetch_add(bytes, std::memory_order_relaxed);
                }

                if (last_size > bytes)
                {
                    this->m_realloc_shrinkB.fetch_add(last_size - bytes, std::memory_order_relaxed);
                }
                else if (last_size < bytes)
                {
                    this->m_realloc_growthB.fetch_add(bytes - last_size, std::memory_order_relaxed);
                }

            }
            else
            {
                this->m_realloc_failC.fetch_add(1, std::memory_order_relaxed);

                expected = this->m_alloc_largest_fail.load(std::memory_order_relaxed);
                while (expected < bytes)
                    if (this->m_alloc_largest_fail.compare_exchange_weak(expected, bytes, std::memory_order_release, std::memory_order_relaxed))
                        break;
            }

            return ret;
        }
        catch (...)
        {
            std::exception_ptr eptr = std::current_exception();

            this->m_realloc_failC.fetch_add(1, std::memory_order_relaxed);

            expected = this->m_alloc_largest_fail.load(std::memory_order_relaxed);
            while (expected < bytes)
                if (this->m_alloc_largest_fail.compare_exchange_weak(expected, bytes, std::memory_order_release, std::memory_order_relaxed))
                    break;

            std::rethrow_exception(eptr);
        }
    }

    void* AllocatorStatistic::realloc(void* ptr, size_t bytes, DestructorPtr destructor, size_t align)
    {
        size_t expected;
        this->m_reallocC.fetch_add(1, std::memory_order_relaxed);

        try
        {
            size_t last_size = this->getAllocSize(ptr);

            void* ret = this->m_object_backing->realloc(ptr, bytes, destructor, align);
            size_t current_use = this->getUsedBytes();

            if (ret != nullptr)
            {
                this->m_allocB.fetch_add(bytes, std::memory_order_relaxed);

                expected = this->m_largest.load(std::memory_order_relaxed);
                while (expected < bytes)
                    if (this->m_largest.compare_exchange_weak(expected, bytes, std::memory_order_release, std::memory_order_relaxed))
                        break;

                expected = this->m_smallest.load(std::memory_order_relaxed);
                while (expected > bytes)
                    if (this->m_smallest.compare_exchange_weak(expected, bytes, std::memory_order_release, std::memory_order_relaxed))
                        break;

                expected = this->m_highest.load(std::memory_order_relaxed);
                while (expected < current_use)
                    if (this->m_highest.compare_exchange_weak(expected, current_use, std::memory_order_release, std::memory_order_relaxed))
                        break;

                if (ret != ptr)
                {
                    this->m_realloc_moveC.fetch_add(1, std::memory_order_relaxed);
                    this->m_realloc_moveB.fetch_add(bytes, std::memory_order_relaxed);
                }

                if (last_size > bytes)
                {
                    this->m_realloc_shrinkB.fetch_add(last_size - bytes, std::memory_order_relaxed);
                }
                else if (last_size < bytes)
                {
                    this->m_realloc_growthB.fetch_add(bytes - last_size, std::memory_order_relaxed);
                }

            }
            else
            {
                this->m_realloc_failC.fetch_add(1, std::memory_order_relaxed);

                expected = this->m_alloc_largest_fail.load(std::memory_order_relaxed);
                while (expected < bytes)
                    if (this->m_alloc_largest_fail.compare_exchange_weak(expected, bytes, std::memory_order_release, std::memory_order_relaxed))
                        break;
            }

            return ret;
        }
        catch (...)
        {
            std::exception_ptr eptr = std::current_exception();

            this->m_realloc_failC.fetch_add(1, std::memory_order_relaxed);

            expected = this->m_alloc_largest_fail.load(std::memory_order_relaxed);
            while (expected < bytes)
                if (this->m_alloc_largest_fail.compare_exchange_weak(expected, bytes, std::memory_order_release, std::memory_order_relaxed))
                    break;

            std::rethrow_exception(eptr);
        }
    }

    size_t AllocatorStatistic::getAllocSize(const void* ptr) const
    {
        return this->m_basic_backing->getAllocSize(ptr);
    }

    void* AllocatorStatistic::offer(void* ptr, uint32_t priority)
    {
        this->m_offerC.fetch_add(1, std::memory_order_relaxed);
        this->m_offerB.fetch_add(this->getAllocSize(ptr), std::memory_order_relaxed);

        void* ret = this->m_object_backing->offer(ptr, priority);

        return ret;
    }

    void* AllocatorStatistic::reclaim(void* ptr)
    {
        this->m_reclaimC.fetch_add(1, std::memory_order_relaxed);

        void* ret = this->m_object_backing->reclaim(ptr);

        if (ret == nullptr)
        {
            this->m_reclaim_failC.fetch_add(1, std::memory_order_relaxed);
        }
        else
        {
            this->m_reclaimB.fetch_add(this->getAllocSize(ptr), std::memory_order_relaxed);
        }

        return ret;
    }

    void AllocatorStatistic::reset()
    {
        this->resetCounters();
        this->m_basic_backing->reset();
    }

    void AllocatorStatistic::purge(uint32_t priority)
    {
        this->m_object_backing->purge(priority);
    }

    void AllocatorStatistic::clear()
    {
        this->resetCounters();
        this->m_object_backing->clear();
    }

    size_t AllocatorStatistic::getFreeBytes() const
    {
        return this->m_basic_backing->getFreeBytes();
    }

    size_t AllocatorStatistic::getUsedBytes() const
    {
        return this->m_basic_backing->getUsedBytes();
    }

    size_t AllocatorStatistic::getPendingBytes() const
    {
        return this->m_object_backing->getPendingBytes();
    }

    size_t AllocatorStatistic::getTotalBytes() const
    {
        return this->m_basic_backing->getTotalBytes();
    }
}
