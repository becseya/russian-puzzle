#include "notifier.hpp"

constexpr int MAP_WIDTH = 10;
constexpr int MAP_HEIGHT = 6;
constexpr size_t SHAPE_NUM = 12;

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

notifer_settings_t settings;

int main() {
    // parameters
    settings.width = MAP_WIDTH;
    settings.height = MAP_HEIGHT;

    // objects
    const FastVector<Shape> shapes(shapes_arr, SHAPE_NUM);
    ShapeMap canvas(MAP_WIDTH, MAP_HEIGHT);
    MyNotifier notifier(settings);
    Solver solver(shapes, canvas, notifier);

    // solve
    solver.solve();
}
