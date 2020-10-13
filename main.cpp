#include "notifier.hpp"

#ifdef MULTI_PROC
#include <omp.h>
#endif

constexpr int MAP_WIDTH = 8;
constexpr int MAP_HEIGHT = 8;
constexpr size_t SHAPE_NUM = 12;

notifer_settings_t settings;
ShapeMap map(MAP_WIDTH, MAP_HEIGHT);

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


static void init() {
    // parameters
    settings.width = MAP_WIDTH;
    settings.height = MAP_HEIGHT;
    //settings.printSolutions = true;

    // draw obstacle
    map.set(3, 3, '!');
    map.set(3, 4, '!');
    map.set(4, 3, '!');
    map.set(4, 4, '!');
}


#ifndef MULTI_PROC

int main() {

    init();

    // objects
    const FastVector<Shape> shapes(shapes_arr, SHAPE_NUM);
    ShapeMap canvas(map);
    MyNotifier notifier(settings);
    Solver solver(shapes, canvas, notifier);

    // solve
    solver.solve();
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
    for(int i = 0; i < 6*6; i++) {
        ShapeMap canvas(map);
        MyMultiprocessNotifier notifier(settings, mutex);

        Solver solver(shapes, canvas, notifier);
        solver.solve(i%6, i/6);
    }
}

#endif
