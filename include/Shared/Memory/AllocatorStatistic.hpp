// Licensed under the MIT License. Copyright (c) 2021 Gergely Pintér.
#ifndef SHARED_MEMORY_ALLOCATOR_STATISTIC_HPP
#define SHARED_MEMORY_ALLOCATOR_STATISTIC_HPP

#include <Shared/Shared.hpp>

#include <Shared/Platform/Types.hpp>
#include <Shared/Memory/ObjectAllocator.hpp>

#include <atomic>

namespace Memory
{
    /**
     * @brief General allocation statistic class.
     *
     * The AllocatorStatistic class acts as a proxy layer over the actual allocator.
     * At the cost of performance it can provide some diagnostic data based on the API calls which the underlying allocator may not.
     *
     * All calls are concurrently safe, as long as the underlying allocator is also thread safe.
     */
    class SHARED_LIB_API AllocatorStatistic : public ObjectAllocator
    {
    public:

        /**
         * @brief Construct an uninitialized AllocatorStatistic object.
         *
         * Construct an uninitialized AllocatorStatistic object.
         * Calling any allocator member functions will result in an error.
         */
        AllocatorStatistic();

        /**
         * @brief Construct an AllocatorStatistic object.
         *
         * Construct an AllocatorStatistic object using only @ref BasicAllocator functions.
         * Calling any functions inherited from @ref ObjectAllocator will cause errors.
         *
         * @param backing The backing allocator which will do the actual allocations.
         *
         * @see @ref BasicAllocator, @ref ObjectAllocator, @ref AllocatorStatistic::AllocatorStatistic(ObjectAllocator*)
         */
        AllocatorStatistic(BasicAllocator* backing);

        /**
         * @brief Construct an AllocatorStatistic object.
         *
         * Construct an AllocatorStatistic object using only @ref ObjectAllocator functions.
         * All allocation member functions are available.
         *
         * @param backing The backing allocator which will do the actual allocations.
         *
         * @see @ref BasicAllocator, @ref ObjectAllocator, @ref AllocatorStatistic::AllocatorStatistic(BasicAllocator*)
         */
        AllocatorStatistic(ObjectAllocator* backing);

        // -- Statistic API --

        /**
         * @brief Get highest allocator memory usage.
         *
         * Returns the highest reported memory usage since the initialization of the @b AllocatorStatistic object.
         * The value gets updated everytime an allocation or reallocation is done.
         *
         * @return The highest reported allocator memory usage.
         *
         * @see @ref BasicAllocator::getUsedBytes()
         */
        virtual size_t getHighestUsage() const;

        /**
         * @brief Get the smallest allocation done.
         *
         * Get the smallest successful allocation done since the initialization of the @b AllocatorStatistic object.
         * If there were no allocations the return value is always 0.
         * The value gets updated everytime an allocation or reallocation is done.
         *
         * @return The smallest allocation size requested and successfully received.
         *
         * @see @ref alloc(), @ref realloc()
         */
        virtual size_t getSmallestAlloc() const;

        /**
         * @brief Get the largest allocation done.
         *
         * Get the largest successful allocation done since the initialization of the @b AllocatorStatistic object.
         * If there were no allocations the return value is 0.
         * The value gets updated everytime an allocation or reallocation is done.
         *
         * @return The largest allocation size requested and successfully received.
         *
         * @see @ref alloc(), @ref realloc()
         */
        virtual size_t getLargestAlloc() const;

        /**
         * @brief Get the call count of @ref alloc().
         *
         * Get the total amount of times when @ref alloc() was called since the last counter reset.
         * The counter will increment on both successful and unsuccessful allocations.
         *
         * @return The call count of @ref alloc().
         *
         * @see @ref alloc(), @ref getTotalAllocFails()
         */
        virtual uintmax_t getTotalAllocs() const;

        /**
         * @brief Get the amount of failed allocations.
         *
         * Get the total amount of times when @ref alloc() was called but @b nullptr was returned.
         *
         * @return The amount of failed allocations.
         *
         * @see @ref alloc(), @ref getLargestAllocFailed(), @ref getTotalAllocs()
         */
        virtual uintmax_t getTotalAllocFails() const;

        /**
         * @brief Get the largest failed allocation.
         *
         * Get the size of the largest failed allocation.
         * This value is 0 if there were no failed allocations since the initialization of the @b AllocatorStatistic object.
         *
         * @return The size of the largest failed allocation.
         *
         * @see @ref alloc(), @ref getTotalAllocFails()
         */
        virtual size_t getLargestAllocFailed() const;

        /**
         * @brief Get the total bytes of memory allocated.
         *
         * Get the cumulative memory size in bytes successfully obtained since the initialization of the @b AllocatorStatistic object.
         * The value gets updated everytime a successful allocation or a relocating reallocation is done.
         * The value gets incremented by the size received rather than the size requested.
         *
         * @return The total bytes of memory allocated.
         *
         * @see @ref alloc(), @ref realloc(), @ref getAllocSize(), @ref getTotalAllocs()
         */
        virtual uintmax_t getTotalAllocBytes() const;

        /**
         * @brief Get the total amount of times when @ref realloc() was called.
         * 
         * Get the total amount of times when @ref realloc() was called since the last counter reset.
         * The counter will increment on both successful and unsuccessful reallocations.
         * 
         * @return The call count of @ref realloc().
         * 
         * @see @ref realloc(), @ref getTotalReallocFails()
         */
        virtual uintmax_t getTotalReallocs() const;

        /**
         * @brief Get the amount of failed reallocations.
         *
         * Get the total amount of times when @ref realloc() was called but @b nullptr was returned.
         * When a reallocation fails it will also set the largest failed allocation size counter.
         *
         * @return The amount of failed reallocations.
         *
         * @see @ref realloc(), @ref getTotalReallocs(), @ref getLargestAllocFailed()
         */
        virtual uintmax_t getTotalReallocFails() const;

        /**
         * @brief Get the allocation size growth in bytes.
         *
         * Get the cumulative memory size in bytes that was used to extend previous allocations.
         * Every time a @ref realloc() where the new size is greater than the old allocation size happens, the counter gets incremented by the size difference.
         *
         * @return The total allocation size growth.
         *
         * @see @ref realloc(), @ref getTotalReallocShrink()
         */
        virtual uintmax_t getTotalReallocGrowth() const;

        /**
         * @brief Get the allocation size shrinkage in bytes.
         *
         * The cumulative memory size in bytes that got freed after a realloc that resulted in the shrinking of previous allocations.
         * Every time a @ref realloc() where the new size is less than the old allocation size happens, the counter gets incremented by the size difference.
         *
         * @return The total allocation size shrinkage.
         *
         * @see @ref realloc(), @ref getTotalReallocGrowth()
         */
        virtual uintmax_t getTotalReallocShrink() const;

        /**
         * @brief Get the total amount of times when realloc() had to move the object.
         *
         * Get the total amount of times where the @ref realloc() couldn't simply extend the allocation size and had to move it's contents.
         * Relocation usually happens when the @ref realloc() extends the allocation size, but some allocator may end up moving the allocation contents even when it' shrinking it.
         * The counter gets incremented when the returned pointer mismatches the original pointer.
         * 
         * @return The amount of times @ref realloc() relocated the allocation.
         *
         * @see @ref realloc(), @ref getTotalReallocMoved()
         */
        virtual uintmax_t getTotalReallocMoves() const;

        /**
         * @brief Get the total amount of memory in bytes that got relocated.
         *
         * Get the total amount of memory in bytes that needed to be moved because of a @ref realloc().
         * Relocation usually happens when the @ref realloc() extends the allocation size, but some allocator may end up moving the allocation contents even when it' shrinking it.
         * The counter gets incremented when the returned pointer mismatches the original pointer.
         *
         * @return The amount of bytes @ref realloc() relocated.
         *
         * @see @ref realloc(), @ref getTotalReallocMoved()
         */
        virtual uintmax_t getTotalReallocMoved() const;

        /**
         * @brief Get the call count of @ref offer().
         *
         * Get the total amount of times @ref offer() was called since the last counter reset.
         *
         * @return The call count of @ref offer().
         *
         * @see @ref offer(), @ref getTotalOfferBytes(), @ref reclaim()
         */
        virtual uintmax_t getTotalOffers() const;

        /**
         * @brief Get the cumulative size of memory offered in bytes.
         *
         * Get the total amount of memory that were passed to @ref offer().
         * Each time @ref offer() is called, the counter gets incremented by the allocation's size.
         *
         * @return The total offered memory in bytes.
         *
         * @see @ref offer(), @ref getTotalOffers(), @ref reclaim()
         */
        virtual uintmax_t getTotalOfferBytes() const;

        /**
         * @brief Get the call count of @ref reclaim().
         *
         * Get the total amount of times @ref reclaim() was called since the last counter reset.
         *
         * @return The call count of @ref reclaim().
         *
         * @see @ref reclaim(), @ref getTotalReclaimFails(), @ref getTotalReclaimBytes(), @ref offer()
         */
        virtual uintmax_t getTotalReclaims() const;

        /**
         * @brief Get the amount of times @ref reclaim() was unsuccessful.
         *
         * Get the total amount of times @ref reclaim() was called but the originally offered memory allocation couldn't be reclaimed.
         *
         * @return The fail count of @ref reclaim().
         *
         * @see @ref reclaim(), @ref getTotalReclaims(), @ref getTotalReclaimBytes(), @ref offer()
         */
        virtual uintmax_t getTotalReclaimFails() const;

        /**
         * @brief Get the cumulative size of memory reclaimed in bytes.
         *
         * Get the total amount of memory that were successfully reclaimed after an offer.
         * Each time @ref relcaim() is called, the counter gets incremented by the allocation's size.
         *
         * @return The total reclaimed memory in bytes.
         *
         * @see @ref reclaim(), @ref getTotalReclaims(), @ref getTotalReclaimFails(), @ref offer(), @ref getTotalOfferBytes()
         */
        virtual uintmax_t getTotalReclaimBytes() const;

        /**
         * @brief Reset all the counters.
         *
         * Reset each and every counter to zero while leaving the allocator itself intact.
         */
        virtual void resetCounters();

        // -- ObjectAllocator API -- All calls are wrapped

        virtual void* alloc(size_t bytes, size_t align = alignof(max_align_t)) override;
        virtual void* alloc(size_t bytes, DestructorPtr destructor, size_t align = alignof(max_align_t)) override;
        virtual void free(void* ptr) override;
        virtual void* realloc(void* ptr, size_t bytes, size_t align = alignof(max_align_t)) override;
        virtual void* realloc(void* ptr, size_t bytes, DestructorPtr destructor, size_t align = alignof(max_align_t)) override;
        virtual size_t getAllocSize(const void* ptr) const override;

        virtual void* offer(void* ptr, uint32_t priority = 0) override;
        virtual void* reclaim(void* ptr) override;

        virtual void reset() override;
        virtual void purge(uint32_t priority = std::numeric_limits<uint32_t>::max()) override;
        virtual void clear() override;

        virtual size_t getFreeBytes() const override;
        virtual size_t getUsedBytes() const override;
        virtual size_t getPendingBytes() const override;
        virtual size_t getTotalBytes() const override;

    protected:

        // Backing data
        BasicAllocator* m_basic_backing;
        ObjectAllocator* m_object_backing;

        // Very basic stats
        std::atomic<size_t> m_smallest; // Smallest allocation done in size
        std::atomic<size_t> m_largest; // Largest allocation done in size
        std::atomic<size_t> m_highest; // Highest reported total usage

        // alloc stats
        std::atomic<uintmax_t> m_allocC; // Total allocation call count
        std::atomic<uintmax_t> m_alloc_failC; // Total allocation call fail count
        std::atomic<size_t> m_alloc_largest_fail; // Largest allocation that failed
        std::atomic<uintmax_t> m_allocB; // Total allocated bytes during lifetime

        // realloc stats
        std::atomic<uintmax_t> m_reallocC; // Total reallocation call count
        std::atomic<uintmax_t> m_realloc_failC; // Total reallocation call fail count
        std::atomic<uintmax_t> m_realloc_growthB; // Total allocation size growth
        std::atomic<uintmax_t> m_realloc_shrinkB; // Total allocation size shrink
        std::atomic<uintmax_t> m_realloc_moveC; // Number of times when allocation got moved
        std::atomic<uintmax_t> m_realloc_moveB; // Total amount of bytes that had to be moved

        // offer/reclaim stats
        std::atomic<uintmax_t> m_offerC; // Total offers
        std::atomic<uintmax_t> m_offerB; // Total offered bytes
        std::atomic<uintmax_t> m_reclaimC; // Total reclaims
        std::atomic<uintmax_t> m_reclaim_failC; // Total failed reclaims
        std::atomic<uintmax_t> m_reclaimB; // Total reclaimed bytes

    };
}

#endif /* SHARED_MEMORY_ALLOCATOR_STATISTIC_HPP */
