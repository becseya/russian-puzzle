#pragma once

#include <cstdlib>

template <typename T>
class FastVector {
    static const size_t DEF_ALLOC = 15;

    T* itemsW;
    T* itemsR;
    size_t itemNum;
    size_t allocated;

public:
    FastVector()
        : itemsW(static_cast<T*>(malloc(sizeof(T)*DEF_ALLOC)))
        , itemsR(itemsW)
        , itemNum(0)
        , allocated(DEF_ALLOC)
    {}

    FastVector(const FastVector& other) {
        *this = other;
    }

    FastVector(const T array[], size_t array_size)
        : itemsW(nullptr)
        , itemsR((T*)array)
        , itemNum(array_size)
        , allocated(0)
    {}

    ~FastVector() {
        if (itemsW)
            free(itemsW);
    }

    FastVector& operator=(const FastVector& other) {
        itemsW = static_cast<T*>(malloc(sizeof(T)*other.itemNum));
        itemsR = itemsW;
        itemNum = other.itemNum;
        allocated = other.itemNum;

        memcpy(itemsW, other.itemsR, itemNum*sizeof(T));

        return *this;
    }

    // --------------------------------------------------------------------------------

    size_t size() const {
        return itemNum;
    }

    T& operator[](size_t idx) const {
        return itemsR[idx];
    }

    void push(const T& item) {
        if (allocated < itemNum + 1)
            reallocate();

        itemsW[itemNum] = item;
        itemNum++;
    }

    void pop(T* item = nullptr) {
        if (item)
            *item = itemsR[itemNum-1];

        itemNum--;
    }

private:
    void reallocate() {
        itemsW = static_cast<T*>(realloc(
            itemsW,
            sizeof(T)*itemNum*2)
        );
        itemsR = itemsW;
        allocated *= 2;
    }
};
