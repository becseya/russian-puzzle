#include "notifier.hpp"

#ifdef MULTI_PROC
#include <omp.h>
#endif

constexpr int MAP_WIDTH = 15;
constexpr int MAP_HEIGHT = 15;
constexpr size_t SHAPE_NUM = 12;
constexpr size_t HEXA_NUM = 35;

notifer_settings_t settings;
ShapeMap map(MAP_WIDTH, MAP_HEIGHT);

const Shape hexa[HEXA_NUM] = {
    (Bitmap) {3, 3, 'U', (const char[]) {0, 0, 1, 1, 1, 1, 1, 1, 0}},
    (Bitmap) {3, 3, 'u', (const char[]) {0, 1, 0, 1, 1, 1, 1, 1, 0}},
    (Bitmap) {3, 3, 'S', (const char[]) {1, 1, 1, 0, 1, 1, 0, 1, 0}},
    (Bitmap) {3, 3, 'W', (const char[]) {1, 0, 0, 1, 1, 0, 1, 1, 1}},
    (Bitmap) {3, 3, 'Y', (const char[]) {1, 0, 1, 1, 1, 1, 0, 1, 0}},
    (Bitmap) {3, 3, 'y', (const char[]) {1, 0, 1, 1, 1, 1, 0, 0, 1}},
    (Bitmap) {3, 3, 'r', (const char[]) {1, 1, 1, 1, 0, 1, 1, 0, 0}},
    (Bitmap) {3, 4, 'w', (const char[]) {0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0}},
    (Bitmap) {3, 4, 'd', (const char[]) {0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0}},
    (Bitmap) {3, 4, '9', (const char[]) {0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0}},
    (Bitmap) {3, 4, 'H', (const char[]) {0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0}},
    (Bitmap) {3, 4, 'A', (const char[]) {0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0}},
    (Bitmap) {3, 4, 'R', (const char[]) {0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0}},
    (Bitmap) {3, 4, '6', (const char[]) {0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0}},
    (Bitmap) {3, 4, 'c', (const char[]) {0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0}},
    (Bitmap) {3, 4, '7', (const char[]) {0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0}},
    (Bitmap) {3, 4, '5', (const char[]) {0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0}},
    (Bitmap) {3, 4, '1', (const char[]) {0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0}},
    (Bitmap) {3, 4, 'f', (const char[]) {0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0}},
    (Bitmap) {3, 4, 'T', (const char[]) {1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0}},
    (Bitmap) {3, 4, 'j', (const char[]) {1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0}},
    (Bitmap) {3, 4, 'L', (const char[]) {1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0}},
    (Bitmap) {2, 4, 'q', (const char[]) {0, 1, 1, 1, 1, 0, 1, 1}},
    (Bitmap) {2, 4, 'n', (const char[]) {0, 1, 1, 1, 1, 1, 1, 0}},
    (Bitmap) {2, 4, 'K', (const char[]) {1, 0, 1, 1, 1, 1, 1, 0}},
    (Bitmap) {2, 4, 'C', (const char[]) {1, 1, 1, 0, 1, 0, 1, 1}},
    (Bitmap) {2, 4, 'F', (const char[]) {1, 1, 1, 0, 1, 1, 1, 0}},
    (Bitmap) {2, 4, 'P', (const char[]) {1, 1, 1, 1, 1, 0, 1, 0}},
    (Bitmap) {5, 2, 'z', (const char[]) {1, 1, 1, 1, 0, 0, 0, 0, 1, 1}},
    (Bitmap) {5, 2, 't', (const char[]) {1, 1, 1, 1, 1, 0, 0, 0, 1, 0}},
    (Bitmap) {5, 2, 'l', (const char[]) {1, 1, 1, 1, 1, 0, 0, 0, 0, 1}},
    (Bitmap) {5, 2, 'k', (const char[]) {1, 1, 1, 1, 1, 0, 0, 1, 0, 0}},
    (Bitmap) {5, 2, 'Z', (const char[]) {1, 1, 1, 0, 0, 0, 0, 1, 1, 1}},
    (Bitmap) {3, 2, '0', (const char[]) {1, 1, 1, 1, 1, 1}},
    (Bitmap) {6, 1, 'I', (const char[]) {1, 1, 1, 1, 1, 1}},
};

const Shape shapes_arr[SHAPE_NUM]{
    (Bitmap) {3, 3, 'X', (const char[]) {0, 1, 0, 1, 1, 1, 0, 1, 0}},
    (Bitmap) {3, 3, 'T', (const char[]) {1, 1, 1, 0, 1, 0, 0, 1, 0}},
    (Bitmap) {3, 3, 'Z', (const char[]) {1, 1, 0, 0, 1, 0, 0, 1, 1}},
    (Bitmap) {3, 3, 'V', (const char[]) {1, 1, 1, 0, 0, 1, 0, 0, 1}},
    (Bitmap) {3, 3, 'F', (const char[]) {1, 1, 0, 0, 1, 1, 0, 1, 0}},
    (Bitmap) {3, 3, 'W', (const char[]) {1, 1, 0, 0, 1, 1, 0, 0, 1}},
    (Bitmap) {3, 2, 'U', (const char[]) {1, 0, 1, 1, 1, 1}},
    (Bitmap) {3, 2, 'P', (const char[]) {1, 1, 1, 1, 1, 0}},
    (Bitmap) {4, 2, 'N', (const char[]) {0, 1, 1, 1, 1, 1, 0, 0,}},
    (Bitmap) {4, 2, 'Y', (const char[]) {1, 1, 1, 1, 0, 1, 0, 0,}},
    (Bitmap) {4, 2, 'L', (const char[]) {1, 1, 1, 1, 1, 0, 0, 0,}},
    (Bitmap) {5, 1, 'I', (const char[]) {1, 1, 1, 1, 1}},
};

const Bitmap obstacle({2, 2, '!', (const char[]) {1, 1, 1, 1}});
const Bitmap obstacle2({3, 5, '!', (const char[]) {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}});


static void init() {
    // parameters
    settings.width = MAP_WIDTH;
    settings.height = MAP_HEIGHT;
    settings.printPlacements = true;
    //settings.printBenchmark = false;

    // draw obstacle
    map.draw(obstacle2, 6, 5);
}


#ifndef MULTI_PROC

int main() {

    init();

    // objects
    const FastVector<Shape> shapes(hexa, HEXA_NUM);
    ShapeMap canvas(map);
    MyNotifier notifier(settings);
    Solver solver(shapes, canvas, notifier);

    // solve
    solver.solve(1, 0);
}

#else

class OmpMutex : public NotifyerMutex {
    omp_lock_t myLock;
public:
    OmpMutex() {omp_init_lock(&myLock);}
    ~OmpMutex() {omp_destroy_lock(&myLock);}
    virtual void lock() override {omp_set_lock(&myLock);}
    virtual void unlock() override {omp_unset_lock(&myLock);}
};

int main() {

    init();

    // objects
    const FastVector<Shape> shapes(shapes_arr, SHAPE_NUM);
    OmpMutex mutex;

    #pragma omp parallel for
    for(int i = 0; i < 13*13; i++) {
        ShapeMap canvas(map);
        MyMultiprocessNotifier notifier(settings, mutex);

        Solver solver(shapes, canvas, notifier);
        solver.solve(i%13, i/13);
    }
}

#endif
