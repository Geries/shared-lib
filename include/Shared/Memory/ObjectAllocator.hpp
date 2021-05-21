// Licensed under the MIT License. Copyright (c) 2021 Gergely Pintér.
#ifndef SHARED_MEMORY_OBJECTALLOCATOR_HPP
#define SHARED_MEMORY_OBJECTALLOCATOR_HPP

#include <Shared/Shared.hpp>
#include <Shared/Memory/BasicAllocator.hpp>

#include <Shared/Platform/Types.hpp>

namespace Memory
{
    /**
     * @brief Interface class extending the functionality of @ref BasicAllocator
     *
     * Interface class for allocators capable of handling memory offerings and calling per allocation assigned user functions at deallocation.
     * Implementing classes should keep the behavior consistent according to the function documentation.
     * Many of the functions may not be concurrency safe, consult the implementing class' documentation for further information.
     *
     * @see @ref BasicAllocator, @ref Heap
     */
    class ObjectAllocator : public BasicAllocator
    {
    public:

        /**
         * @brief Helper type for destructor functions.
         *
         * Destructor functions take a @a void* pointer pointing to the object to be destroyed.
         * Destructor functions may not throw exceptions.
         * 
         * @see @ref destroy()
         */
        using DestructorPtr = void(*)(void*) noexcept;

        /**
         * @brief Allocate a memory block and set a destructor function to be called on deletion.
         *
         * Allocate an aligned memory block at least @a bytes sized.
         * The returned pointer will be at least @a align bytes aligned. \n
         * The content of the memory at the returned pointer is uninitialized. \n
         * The function passed to @a destructor will get called before the allocation is invalidated by the user calling @ref free or by the system internally. \n
         * If @a align is not a power of two it may cause undefined behavior and memory corruption. \n
         * Returns @b nullptr on failure.
         *
         * Thread safety depends on actual implementation.
         *
         * Code example:
         * \code{.cpp}
            // Assume "allocator" is a pointer to a valid instance of ObjectAllocator
            void* mem_ptr = allocator->alloc(sizeof(ObjectType), allocator->Destructor<ObjectType>, alignof(ObjectType));
            // It would be very naughty to not check for nullptr before placement new, as the placement new operator may try to initialize object on the null pointer! (UB)
            if (mem_ptr == nullptr) panic();
            else
            {
                ObjectType* var = new (mem_ptr) ObjectType(initialization_value);
                // -- do stuff --
            }
            allocator->free(mem_ptr); \endcode
         *
         * @param bytes Amount of bytes requested. May be a zero value.
         * @param destructor Function to be called before the allocation gets invalidated. May be @b nullptr, in which case no function will be called.
         * @param align Requested pointer alignedness. Must be a power of two.
         * @return A valid pointer to the beginning of the requested memory block on success, @b nullptr otherwise.
         *
         * @see @ref free, @ref realloc
         */
        virtual void* alloc(size_t bytes, DestructorPtr destructor, size_t align = alignof(max_align_t)) = 0;

        /**
         * @brief Free a memory block.
         *
         * Free a previously allocated memory block.
         * Function passed as destructor at @ref alloc or at @ref realloc will be called on the pointer. \n
         * The order in which allocations must be deallocated may be limited by the implementation. \n
         * An invalid non-@b nullptr pointer value passed to @a ptr may cause undefined behavior and memory corruption. \n
         * A pointer received from a different allocator should never be passed unless implementation allows it. \n
         * The content and the accessibility of the freed memory block is undefined.
         *
         * Thread safety depends on actual implementation.
         *
         * @param ptr Pointer to a valid allocated memory block. May be nullptr.
         *
         * @see @ref alloc, @ref realloc, @ref offer, @ref reclaim
         */
        virtual void free(void* ptr) = 0;

        /**
         * @brief Reallocate memory.
         *
         * Request a different sized memory block, a different alignedness or change destructor function while retaining the data at the original allocation. \n
         * If the new size is greater than the original allocation's, the content of the newly allocated extension region is indeterminate.
         * The implementation may deem it necessary to move the allocation to statisfy the new requirements, in which case the old pointer will not match the returned pointer.
         * As a result the old pointer is considered as freed and the content and the accessibility of the old memory block is undefined. \n
         * An invalid pointer passed to @a ptr may cause undefined behavior and memory corruption. \n
         * A pointer received from a different allocator should never be passed unless implementation allows it. \n
         * This function can change the @a destructor function originally set at @ref alloc. The function passed to @a destructor will get called before the allocation is invalidated by the user calling @ref free or by the system internally. \n
         * If @a align is not a power of two it may cause undefined behavior and memory corruption. \n
         * Returns @b nullptr on failure, in which case the original allocation remains unaltered. The implementation may ignore reallocation requests and always return nullptr.
         *
         * It is generally not recommended to use realloc as it usually involves memory relocation incurring great performance penalties. Instead a suitably oversized memory block should be allocated, or an appropriate data structure used.
         *
         * Thread safety depends on actual implementation.
         *
         * @param ptr Pointer to a valid allocated memory block.
         * @param bytes Amount of bytes requested. May be a zero value.
         * @param destructor Function to be called before the allocation gets invalidated. May be nullptr, in which case no function will be called.
         * @param align Requested pointer alignedness. Must be a power of two.
         * @return A valid pointer to the beginning of the adjusted memory block on success, nullptr otherwise.
         *
         * @see @ref alloc, @ref free
         */
        virtual void* realloc(void* ptr, size_t bytes, DestructorPtr destructor, size_t align = alignof(max_align_t)) = 0;
        
        /**
         * @brief Offer an unnecessary but still reusable memory allocation.
         *
         * Offer an allocated memory block to be freed and reused internally when necessary. 
         * Offered allocations could be ones that contain large amounts of data loaded from disk or network, typically taking a long time to reload, however at the moment not used. \n
         * After a memory block is offered it's contents and accessibility is not guaranteed and it is not a good idea to access it before it is reclaimed.
         * The offered memory can be reclaimed with @ref reclaim using the pointer this function returns. \n
         * The allocator may decide to deallocate the offered memory block right away, in which case this function will return @b nullptr.
         * To permanently deallocate the memory after its been offered, pass the pointer returned by this function to @ref free. 
         * The returned pointer must be passed to @ref free or @ref reclaim even if the original allocation is already deallocated to ensure there are no memory leaks. \n
         * When the offered block is decided to be deallocated internally, the @a destructor function set at @ref alloc or @ref realloc will be called. \n
         * An invalid pointer passed to @a ptr may cause undefined behavior and memory corruption. \n
         * A pointer received from a different allocator should never be passed unless implementation allows it. \n
         * The value passed to @a priority is a hint for the allocator marking data importance, with 0 being the least important and the most likely to get deallocated.
         * The actual order in which the offered memory gets repurposed is entirely up to the implementation's discretion.
         *
         * Thread safety depends on actual implementation.
         *
         * Code example:
         * \code{.cpp}
            // Assume "allocator" is a pointer to a valid instance of ObjectAllocator
            void* big_data = allocator->alloc(128_MB, cleanup_function);
            // -- use the memory(if the allocation succeeded and didn't return nullptr) --
            big_data = allocator->offer(big_data);
            // -- do something else and wait till the data is needed again --

            // -- if its needed again --
            big_data = allocator->reclaim(big_data);
            if (big_data == nullptr)
            {
                // -- it got deallocated meanwhile so reallocate it --
                big_data = allocator->alloc(128_MB, cleanup_function);
                // -- reload data --
            }
            // -- use data --

            // -- on cleanup --
            allocator->free(big_data); \endcode
         *
         * @param ptr Pointer to a valid allocated memory block.
         * @param priority Indicator of data priority.
         * @return A unique pointer which the original allocation can be restored with, or @b nullptr if the offered allocator got freed immediately.
         *
         * @see @ref alloc, @ref realloc, @ref free, @ref reclaim, @ref purge
         */
        virtual void* offer(void* ptr, uint32_t priority = 0) = 0;

        /**
         * @brief Reclaim a previously offered memory block.
         *
         * Reclaim a offered allocation with the unique pointer received from @ref offer.
         * If the allocation is still valid then this function will restore it's 'in use' status with the original data intact and return the original pointer.
         * If the allocation can no longer be reclaimed then the return value will be @b nullptr.
         * In both cases the unique offer pointer will be invalidated and must not be passed to any functions again. \n
         * An invalid non-@b nullptr pointer passed to @a ptr may cause undefined behavior and memory corruption. \n
         * A pointer received from a different allocator should never be passed unless implementation allows it. \n
         *
         * Thread safety depends on actual implementation.
         *
         * @param ptr Pointer to a valid offer pointer or @b nullptr.
         * @return The pointer to the original allocation which got passed to @ref offer in case the allocation still exists, nullptr otherwise.
         *
         * @see @ref alloc, @ref realloc, @ref free, @ref offer, @ref purge
         */
        virtual void* reclaim(void* ptr) = 0;

        /**
         * @brief Deallocate offered memory in batch.
         *
         * Deallocate every memory allocation offering on and also below the given priority level.
         * Allocations on higher levels may also be deallocated.
         * Offer pointers will remain valid and still have to be deallocated! \n
         * Specific implementations may also use this function to return unused virtual memory back to the operating system.
         *
         * Thread safety depends on actual implementation.
         *
         * @param priority Memory allocations on this priority level and below will be deallocated. Memory allocations on higher levels may also get deallocated.
         *
         * @see @ref offer, @ref reclaim, @ref purge
         */
        virtual void purge(uint32_t priority = std::numeric_limits<uint32_t>::max()) = 0;

        /**
         * @brief Clear all allocations.
         *
         * Bring the allocator into a default state, but instead of invalidating all allocations it gives a chance to cleanup by calling destructor functions. /n
         * After calling this function, the content and accessibility of previous allocations are undefined and the offer pointers will be invalidated.
         *
         * @see @ref BasicAllocator::reset
         */
        virtual void clear() = 0;

        /**
         * @brief Get the amount of pending deallocation in bytes.
         *
         * Get the total amount of pending deallocation/offered memory available to the allocator to recycle.
         *
         * Thread safety depends on actual implementation.
         *
         * @return Pending memory deallocations in bytes.
         *
         * @see @ref offer, @ref reclaim, @ref purge
         */
        virtual size_t getPendingBytes() const = 0;
        
        /**
         * @brief Object destructor wrapper template function.
         * 
         * A helper template function that calls the destructor on a single object.
         *
         * @tparam T Type of the object.
         * @param memory The pointer of the object.
         *
         * @see @ref alloc, @ref realloc
         */
        template<class T>
        static void destroy(void* memory) noexcept
        {
            reinterpret_cast<T*>(memory)->~T();
        }

        virtual ~ObjectAllocator() = default;
    };
}

#endif /* SHARED_MEMORY_OBJECTALLOCATOR_HPP */
