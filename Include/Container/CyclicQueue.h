#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <stdexcept>

template <typename T, size_t Size> class CyclicQueue
{
    static_assert(Size > 0, "CyclicQueue size cannot be zero");

  private:
    std::array<T, Size> data;
    size_t cursor; // index of the next element
    size_t elementCount;

  public:
    CyclicQueue() : cursor(0), elementCount(0)
    {
    }

    void pushFront(T value)
    {
        if (elementCount == Size)
        {
            throw std::runtime_error("CyclicQueue elementCount overflowed Size");
        }

        data[cursor] = value;
        cursor = (cursor + 1) % Size;
        elementCount = std::min(elementCount + 1, Size);
    }

    void popBack()
    {
        if (elementCount > 0)
        {
            elementCount -= 1;
        }
    }

    size_t size()
    {
        return elementCount;
    }

    [[nodiscard]] T &front()
    {
        if (elementCount == 0)
        {
            throw std::runtime_error("Cannot get the front of an empty queue");
        }

        return data[(cursor + Size - 1) % Size];
    }

    [[nodiscard]] T &back()
    {
        if (elementCount == 0)
        {
            throw std::runtime_error("Cannot get the back of an empty queue");
        }

        return data[(cursor + Size - elementCount) % Size];
    }

    class Iterator
    {
      private:
        CyclicQueue *queue;
        size_t index;
        size_t remaining;

      public:
        Iterator(CyclicQueue *q, size_t start, size_t remaining) : queue(q), index(start), remaining(remaining)
        {
        }

        T &operator*()
        {
            return queue->data[index];
        }

        Iterator &operator++()
        {
            if (remaining == 0)
            {
                throw std::runtime_error("Iterator out of bounds");
            }

            index = (index + 1) % Size;
            remaining -= 1;
            return *this;
        }

        bool operator!=(Iterator &other)
        {
            return remaining != other.remaining;
        }
    };

    class ConstIterator
    {
      private:
        const CyclicQueue *queue;
        size_t index;
        size_t remaining;

      public:
        ConstIterator(const CyclicQueue *q, size_t start, size_t remaining)
            : queue(q), index(start), remaining(remaining)
        {
        }

        const T &operator*()
        {
            return queue->data[index];
        }

        ConstIterator &operator++()
        {
            if (remaining == 0)
            {
                throw std::runtime_error("Iterator out of bounds");
            }

            index = (index + 1) % Size;
            remaining -= 1;
            return *this;
        }

        bool operator!=(ConstIterator &other)
        {
            return remaining != other.remaining;
        }
    };

    Iterator begin()
    {
        if (elementCount == 0)
        {
            return end();
        }

        size_t start = (cursor + Size - elementCount) % Size;
        return Iterator(this, start, elementCount);
    }

    Iterator end()
    {
        size_t start = (cursor + Size - elementCount) % Size;
        return Iterator(this, start, 0);
    }

    const ConstIterator begin() const
    {
        if (elementCount == 0)
        {
            return end();
        }

        size_t start = (cursor + Size - elementCount) % Size;
        return ConstIterator(this, start, elementCount);
    }

    const ConstIterator end() const
    {
        size_t start = (cursor + Size - elementCount) % Size;
        return ConstIterator(this, start, 0);
    }
};
