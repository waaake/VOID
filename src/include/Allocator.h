// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _IMAGE_ALLOCATOR_H
#define _IMAGE_ALLOCATOR_H

/* STD */
#include <cmath>
#include <list>
#include <mutex>
#include <vector>
#include <unordered_map>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

template <typename _Ty>
class ImageBufferAllocator
{
public:
    using value_type = _Ty;

    ImageBufferAllocator() = default;
    ~ImageBufferAllocator()
    {
        for (auto& [_, freelist] : s_buckets)
        {
            for (void* _pointer : freelist)
                ::operator delete(_pointer);
        }
    }

    _Ty* allocate(std::size_t _size)
    {
        // Rounded requested size
        std::size_t bucketSize = Round(_size);

        std::lock_guard<std::mutex> lock(s_allocMutex);
        std::list<void*>& freelist = s_buckets[bucketSize];
        if (freelist.empty())
            return static_cast<_Ty*>(::operator new(bucketSize * sizeof(_Ty)));

        _Ty* _pointer = static_cast<_Ty*>(freelist.front());
        freelist.pop_front();
        return _pointer;
    }

    void deallocate(_Ty* _pointer, std::size_t _size)
    {
        std::size_t bucketSize = Round(_size);
        std::lock_guard<std::mutex> lock(s_allocMutex);
        s_buckets[bucketSize].push_back(_pointer);
    }

    // static void FreeMem()
    // {
    //     for (auto& [_, freelist] : s_buckets)
    //     {
    //         for (void* _pointer : freelist)
    //             ::operator delete(_pointer);
    //     }
    // }

    ImageBufferAllocator(const ImageBufferAllocator& other)
    {
        s_buckets = other.s_buckets;
    }
    ImageBufferAllocator(ImageBufferAllocator&& other)
    {
        std::swap(s_buckets, other.s_buckets);
    }

    ImageBufferAllocator& operator=(const ImageBufferAllocator& other)
    {
        if (this == &other)
            return *this;
        
        s_buckets = other.s_buckets;
        return *this;
    }

    ImageBufferAllocator& operator=(ImageBufferAllocator&& other)
    {
        if (this == &other)
            return *this;
        
        std::swap(s_buckets, other.s_buckets);
        return *this;
    }

    bool operator==(const ImageBufferAllocator& other) { return true; }
    bool operator!=(const ImageBufferAllocator& other) { return false; }

private: 
    std::unordered_map<std::size_t, std::list<void*>> s_buckets;
    std::mutex s_allocMutex;

private:
    static std::size_t Round(std::size_t _size)
    {
        std::size_t bucket = 1;
        // Next greater 2^n than the requested size
        while (bucket < _size) bucket <<= 1;
        return bucket;
    }
};

VOID_NAMESPACE_CLOSE

#endif // _IMAGE_ALLOCATOR_H
