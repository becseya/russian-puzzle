#include "notifier.hpp"

constexpr size_t SHAPE_NUM = 12;
const Shape shapes_arr[SHAPE_NUM]{
    (Bitmap) {3, 3, 'E', (const char[]) {1, 1, 1, 0, 1, 0, 0, 1, 0}},
    (Bitmap) {3, 3, 'F', (const char[]) {0, 1, 0, 1, 1, 1, 0, 1, 0}},
    (Bitmap) {3, 3, 'G', (const char[]) {1, 1, 0, 0, 1, 0, 0, 1, 1}},
    (Bitmap) {3, 3, 'H', (const char[]) {1, 1, 1, 0, 0, 1, 0, 0, 1}},
    (Bitmap) {3, 3, 'I', (const char[]) {1, 1, 0, 0, 1, 1, 0, 1, 0}},
    (Bitmap) {3, 3, 'J', (const char[]) {1, 1, 0, 0, 1, 1, 0, 0, 1}},
    (Bitmap) {3, 2, 'K', (const char[]) {1, 0, 1, 1, 1, 1}},
    (Bitmap) {3, 2, 'L', (const char[]) {1, 1, 1, 1, 1, 0}},
    (Bitmap) {4, 2, 'B', (const char[]) {0, 1, 1, 1, 1, 1, 0, 0,}},
    (Bitmap) {4, 2, 'C', (const char[]) {1, 1, 1, 1, 1, 0, 0, 0,}},
    (Bitmap) {4, 2, 'D', (const char[]) {1, 1, 1, 1, 0, 1, 0, 0,}},
    (Bitmap) {5, 1, 'A', (const char[]) {1, 1, 1, 1, 1}},
};
ConstFastVector<Shape> shapes(shapes_arr, SHAPE_NUM);

int main() {
    ShapeMap canvas(10, 6);
    MyNotifier notifier(canvas);
    Solver solver(shapes, canvas, notifier);

    solver.solve();
}
