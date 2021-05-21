// Licensed under the MIT License. Copyright (c) 2021 Gergely Pintér.
#ifndef SHARED_MEMORY_BASICALLOCATOR_HPP
#define SHARED_MEMORY_BASICALLOCATOR_HPP

#include <Shared/Platform/Types.hpp>

namespace Memory
{
    /**
     * @brief Interface class for simple memory allocators. 
     * 
     * Interface class for low level memory allocation handlers which feature simplicity.
     * Implementing classes should keep the behavior consistent according to the function documentation.
     * Many of the functions may not be concurrency safe, consult the implementing class' documentation for further information.
     *
     * @see @ref ObjectAllocator, @ref Heap, @ref BasicAllocatorStatistic
     */
    class BasicAllocator
    {
    public:

        /**
         * @brief Allocate a memory block.
         *
         * Allocate an aligned memory block at least @a bytes sized.
         * The returned pointer will be at least @a align bytes aligned. \n
         * The content of the memory at the returned pointer is uninitialized. \n
         * If @a align is not a power of two it may cause undefined behavior and memory corruption. \n
         * Returns @b nullptr on failure.
         *
         * Thread safety depends on actual implementation.
         *
         * @param bytes Amount of bytes requested. May be a zero value.
         * @param align Requested pointer alignedness. Must be a power of two.
         * @return A valid pointer to the beginning of the requested memory block on success, @b nullptr otherwise. 
         *
         * @see @ref free, @ref realloc
         */
        virtual void* alloc(size_t bytes, size_t align = alignof(max_align_t)) = 0;
        
        /**
         * @brief Free a memory block.
         *
         * Free a previously allocated memory block. \n
         * The order in which allocations must be deallocated may be limited by the implementation. \n
         * An invalid non-@b nullptr pointer value passed to @a ptr may cause undefined behavior and memory corruption. \n
         * A pointer received from a different allocator should never be passed unless implementation allows it. \n
         * The content and the accessibility of the freed memory block is undefined.
         *
         * Thread safety depends on actual implementation.
         *
         * @param ptr Pointer to a valid allocated memory block. May be @b nullptr.
         *
         * @see @ref alloc, @ref realloc
         */
        virtual void free(void* ptr) = 0;

        /**
         * @brief Reallocate memory.
         *
         * Request a different sized memory block or a different alignedness while retaining the data at the original allocation. \n
         * If the new size is greater than the original allocation's, the content of the newly allocated extension region is indeterminate.
         * The implementation may deem it necessary to move the allocation to statisfy the new requirements, in which case the old pointer will not match the returned pointer.
         * As a result the old pointer is considered as freed and the content and the accessibility of the old memory block is undefined. \n
         * An invalid pointer passed to @a ptr may cause undefined behavior and memory corruption. \n
         * A pointer received from a different allocator should never be passed unless implementation allows it. \n
         * If @a align is not a power of two it may cause undefined behavior and memory corruption. \n
         * Returns @b nullptr on failure, in which case the original allocation remains unaltered. The implementation may ignore reallocation requests and always return @b nullptr.
         *
         * It is generally not recommended to use realloc as it usually involves memory relocation incurring great performance penalties. Instead a suitably oversized memory block should be allocated, or an appropriate data structure used.
         *
         * Thread safety depends on actual implementation.
         *
         * @param ptr Pointer to a valid allocated memory block.
         * @param bytes Amount of bytes requested. May be a zero value.
         * @param align Requested pointer alignedness. Must be a power of two.
         * @return A valid pointer to the beginning of the adjusted memory block on success, @b nullptr otherwise. 
         *
         * @see @ref alloc, @ref free
         */
        virtual void* realloc(void* ptr, size_t bytes, size_t align = alignof(max_align_t)) = 0;

        /**
         * @brief Get the allocated memory block's size.
         *
         * Query the usable size of the memory block at the given allocation pointer.
         * The whole range indicated by the returned value is safe to access and modify.
         * The returned value may mismatch the value originally passed to @ref alloc or @ref realloc. \n
         * Passing pointer received from a different allocator may produce an erroneous value unless implementation states otherwise.
         *
         * Thread safety depends on actual implementation.
         *
         * @param ptr Pointer to a valid allocated memory block.
         * @return The usable memory block size at @a ptr
         *
         * @see @ref alloc, @ref realloc, @ref free
         */
        virtual size_t getAllocSize(const void* ptr) const = 0;

        /**
         * @brief Reset the memory allocator.
         *
         * Bring the allocator into a default state, invalidating all previous allocations.
         * The content and accessibility of previous allocations are undefined.
         */
        virtual void reset() = 0;

        /**
         * @brief Get the amount of free memory.
         *
         * Get the remaining free memory available for the allocator to provision. 
         * This value is not the largest allocation that can be requested.
         * This value does not represent the total free virtual memory in the system.
         * 
         * Thread safety depends on actual implementation.
         * 
         * @return Free memory size in bytes.
         *
         * @see @ref getUsedBytes, @ref getTotalBytes
         */
        virtual size_t getFreeBytes() const = 0;

        /**
         * @brief Get the amount of used memory.
         *
         * Get the total memory used for allocations inside the allocator.
         * This value may be much larger than the sum of all allocation's requested size as it may include other allocation overheads.
         * This value does not represent the total used virtual memory in the system.
         *
         * Thread safety depends on actual implementation.
         *
         * @return Used memory size in bytes.
         *
         * @see @ref getFreeBytes, @ref getTotalBytes
         */
        virtual size_t getUsedBytes() const = 0;

        /**
         * @brief Get the total size of memory.
         *
         * Get the total memory available inside the allocator, including both allocated and free memory.
         * This value does not represent the total virtual memory in the system.
         *
         * Thread safety depends on actual implementation.
         *
         * @return Total memory size in bytes.
         *
         * @see @ref getFreeBytes, @ref getUsedBytes
         */
        virtual size_t getTotalBytes() const = 0;

        /**
         * @brief Get required alignment offset.
         *
         * Helper function to get the required offset needed to be added to @a ptr to statisfy @a alignment requirement.
         * Will produce erroneous results if @a alignment is not a power of two.
         *
         * @param ptr The pointer to be aligned.
         * @param alignment The required alignment. Must be a power of two.
         * @return Alignment offset value.
         *
         * @see @ref getAlignedPtr
         */
        static inline size_t getAlignedOffset(void* ptr, size_t alignment) noexcept
        {
            return (alignment - (reinterpret_cast<uintptr_t>(ptr) & (alignment - 1))) & (alignment - 1);
        }

        /**
         * @brief Get aligned pointer.
         *
         * Helper function to get the closest aligned pointer to on @a ptr which statisfies @a alignment requirement.
         * Will produce erroneous results if @a alignment is not a power of two.
         *
         * @param ptr The pointer to be aligned.
         * @param alignment The required alignment. Must be a power of two.
         * @return Alignmened pointer.
         *
         * @see @ref getAlignedOffset
         */
        static inline void* getAlignedPtr(void* ptr, size_t alignment) noexcept
        {
            return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ptr) + Memory::BasicAllocator::getAlignedOffset(ptr, alignment));
        }

        virtual ~BasicAllocator() = default;
    };
}

#endif /* SHARED_MEMORY_BASICALLOCATOR_HPP */
