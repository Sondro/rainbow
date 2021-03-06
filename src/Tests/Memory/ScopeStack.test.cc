// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Memory/ScopeStack.h"

#include <cstddef>

#include <gtest/gtest.h>

using rainbow::LinearAllocator;
using rainbow::ScopeStack;

namespace
{
    class DeleteMe
    {
    public:
        DeleteMe(bool& deleted, DeleteMe**& order)
            : deleted_(deleted), order_(order)
        {
        }

        ~DeleteMe()
        {
            deleted_ = true;
            *order_ = this;
            ++order_;
        }

    private:
        bool& deleted_;
        DeleteMe**& order_;
    };
}  // namespace

TEST(LinearAllocatorTest, AlignedSizeFitsAnyType)
{
    for (size_t size : {sizeof(int8_t),
                        sizeof(int16_t),
                        sizeof(int32_t),
                        sizeof(double),
                        sizeof(LinearAllocator),
                        sizeof(ScopeStack),
                        sizeof(char[1024])})
    {
        const size_t aligned = LinearAllocator::aligned_size(size);
        ASSERT_GE(aligned, size);
        ASSERT_EQ(aligned % alignof(std::max_align_t), 0u);
    }
}

TEST(LinearAllocatorTest, AllocatesEnoughSpace)
{
    constexpr ptrdiff_t data_size = sizeof(sizeof(char[100]));

    LinearAllocator allocator(1024);
    const uint8_t* begin = static_cast<const uint8_t*>(allocator.end());

    ASSERT_EQ(allocator.allocate(data_size), begin);

    const uint8_t* end = static_cast<const uint8_t*>(allocator.end());

    ASSERT_GE(end - begin, data_size);

    const size_t aligned_double = LinearAllocator::aligned_size(data_size);

    ASSERT_EQ(end, begin + aligned_double);
}

TEST(ScopeStackTest, ResetsStack)
{
    const uint32_t kFirst = 0xabad1dea;
    const uint32_t kSecond = 0xdeadbeef;

    LinearAllocator allocator(1024);
    const uint8_t* allocator_begin =
        static_cast<const uint8_t*>(allocator.end());

    ScopeStack stack(allocator);
    const auto i = stack.allocate<uint32_t>(kFirst);

    ASSERT_EQ(kFirst, *i);
    ASSERT_GT(allocator.end(), allocator_begin);

    const auto j = stack.allocate<uint32_t>(kSecond);

    ASSERT_EQ(kFirst, *i);
    ASSERT_EQ(kSecond, *j);

    stack.reset();

    ASSERT_EQ(allocator.end(), allocator_begin);

    const auto k = stack.allocate<uint32_t>(kFirst);

    ASSERT_EQ(kFirst, *k);
    ASSERT_GT(allocator.end(), allocator_begin);

    const auto l = stack.allocate<uint32_t>(kSecond);

    ASSERT_EQ(kFirst, *k);
    ASSERT_EQ(kSecond, *l);
}

TEST(ScopeStackTest, RewindsAllocator)
{
    const uint32_t kFirst = 0xabad1dea;
    const uint32_t kSecond = 0xdeadbeef;

    LinearAllocator allocator(1024);
    const uint8_t* allocator_begin =
        static_cast<const uint8_t*>(allocator.end());
    {
        ScopeStack stack(allocator);
        const auto i = stack.allocate<uint32_t>(kFirst);

        ASSERT_EQ(kFirst, *i);
        ASSERT_GT(allocator.end(), allocator_begin);

        const auto j = stack.allocate<uint32_t>(kSecond);

        ASSERT_EQ(kFirst, *i);
        ASSERT_EQ(kSecond, *j);
    }
    ASSERT_EQ(allocator.end(), allocator_begin);
}

TEST(ScopeStackTest, CallsDestructorsInReverseOrder)
{
    DeleteMe* order[3];
    DeleteMe* subjects[3];
    bool deleted1 = false;
    bool deleted2 = false;
    bool deleted3 = false;
    LinearAllocator allocator(1024);
    const uint8_t* allocator_begin =
        static_cast<const uint8_t*>(allocator.end());
    {
        ScopeStack stack(allocator);
        DeleteMe** order_p = order;
        subjects[0] = stack.allocate<DeleteMe>(deleted1, order_p);
        subjects[1] = stack.allocate<DeleteMe>(deleted2, order_p);
        subjects[2] = stack.allocate<DeleteMe>(deleted3, order_p);

        ASSERT_FALSE(deleted1);
        ASSERT_FALSE(deleted2);
        ASSERT_FALSE(deleted3);
    }
    ASSERT_TRUE(deleted1);
    ASSERT_TRUE(deleted2);
    ASSERT_TRUE(deleted3);
    ASSERT_EQ(order[0], subjects[2]);
    ASSERT_EQ(order[1], subjects[1]);
    ASSERT_EQ(order[2], subjects[0]);
    ASSERT_EQ(allocator.end(), allocator_begin);
}
