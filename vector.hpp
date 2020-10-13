#pragma once

#include <cstdlib>

template <typename T>
class FastVector {
    static const size_t DEF_ALLOC = 15;

    T* items;
    size_t itemNum;
    size_t allocated;

public:
    FastVector()
        : items(static_cast<T*>(malloc(sizeof(T)*DEF_ALLOC)))
        , itemNum(0)
        , allocated(DEF_ALLOC)
    {}

    FastVector(const FastVector& other)
        : items(static_cast<T*>(malloc(sizeof(T)*other.itemNum)))
        , itemNum(other.itemNum)
        , allocated(other.itemNum)
    {
        memcpy(this->items, other.items, itemNum*sizeof(T));
    }

    FastVector(const T array[], size_t array_size)
        : items((T*)array)
        , itemNum(array_size)
        , allocated(0)
    {}

    ~FastVector() {
        free(items);
    }

    FastVector& operator=(const FastVector& other) {
        this->items = static_cast<T*>(malloc(sizeof(T)*other.itemNum));
        this->itemNum = other.itemNum;
        this->allocated = other.itemNum;
        memcpy(this->items, other.items, itemNum*sizeof(T));
        return *this;
    }

    // --------------------------------------------------------------------------------

    size_t size() const {
        return itemNum;
    }

    T& operator[](size_t idx) const {
        return items[idx];
    }

    void push(const T& item) {
        if (allocated < itemNum + 1)
            reallocate();

        items[itemNum] = item;
        itemNum++;
    }

    void pop(T* item = nullptr) {
        if (item)
            *item = items[itemNum-1];

        itemNum--;
    }

private:
    void reallocate() {
        items = static_cast<T*>(realloc(
            items,
            sizeof(T)*itemNum*2)
        );
        allocated *= 2;
    }
};
