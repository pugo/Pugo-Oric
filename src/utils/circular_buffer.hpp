// =========================================================================
//   Copyright (C) 2009-2026 by Anders Piniesjö <pugo@pugo.org>
//
//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>
// =========================================================================

#ifndef UTILS_CIRCULAR_BUFFER_H
#define UTILS_CIRCULAR_BUFFER_H

#include <vector>

namespace utils {

/**
 * Simple circular buffer implementation.
 * @tparam T type of elements stored in the buffer
 */
template<typename T>
class CircularBuffer
{
public:
    class iterator
    {
    public:
        /**
         * @param buffer reference to the circular buffer
         * @param pos position in the buffer
         */
        iterator(CircularBuffer& buffer, size_t pos) :
            buffer(buffer),
            pos(pos)
        {
        }

        /**
         * Increment the iterator.
         * @return reference to the incremented iterator
         */
        iterator& operator++()
        {
            ++pos;
            return *this;
        }

        /**
         * Compare two iterators for equality.
         * @param other the other iterator to compare with
         * @return true if the iterators are equal, false otherwise
         */
        bool operator!=(const iterator& other) const
        {
            return pos != other.pos;
        }

        /**
         * Dereference the iterator to access the element.
         * @return reference to the element at the current position
         */
        T& operator*()
        {
            return buffer[pos];
        }

    private:
        CircularBuffer& buffer;
        size_t pos;
    };

    /**
     * Set the capacity of the circular buffer.
     * @param new_capacity the new capacity of the buffer
     */
    void set_capacity(size_t new_capacity)
    {
        storage.clear();
        storage.resize(new_capacity);
        head = 0;
        item_count = 0;
    }

    /**
     * Add an item to the end of the circular buffer.
     * @param item the item to add
     */
    void push_back(const T& item)
    {
        if (storage.empty()) {
            return;
        }

        const size_t index = (head + item_count) % storage.size();
        storage[index] = item;

        if (item_count < storage.size()) {
            ++item_count;
        }
        else {
            head = (head + 1) % storage.size();
        }
    }

    /**
     * Remove the first item from the circular buffer.
     */
    void pop_front()
    {
        if (empty()) {
            return;
        }

        head = (head + 1) % storage.size();
        --item_count;
    }

    /**
     * Clear the circular buffer, removing all items.
     */
    void clear()
    {
        head = 0;
        item_count = 0;
    }

    /**
     * Check if the circular buffer is empty.
     * @return true if the buffer is empty, false otherwise
     */
    bool empty() const
    {
        return item_count == 0;
    }

    /**
     * Get the number of items in the circular buffer.
     * @return the number of items in the buffer
     */
    size_t size() const
    {
        return item_count;
    }

    /**
     * Access an item in the circular buffer by index.
     * @param index the index of the item to access
     * @return reference to the item at the specified index
     */
    T& operator[](size_t index)
    {
        return storage[(head + index) % storage.size()];
    }

    /**
     * Access an item in the circular buffer by index (const version).
     * @param index the index of the item to access
     * @return const reference to the item at the specified index
     */
    const T& operator[](size_t index) const
    {
        return storage[(head + index) % storage.size()];
    }

    /**
     * Get an iterator to the beginning of the circular buffer.
     * @return iterator to the first item in the buffer
     */
    iterator begin()
    {
        return iterator(*this, 0);
    }

    /**
     * Get an iterator to the end of the circular buffer.
     * @return iterator to the position after the last item in the buffer
     */
    iterator end()
    {
        return iterator(*this, item_count);
    }

private:
    std::vector<T> storage;
    size_t head = 0;
    size_t item_count = 0;
};

} // namespace utils

#endif // UTILS_CIRCULAR_BUFFER_H
