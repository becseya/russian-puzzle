#include <stdio.h>
#include <time.h>

#include "solver.hpp"

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


#define COLOR_AFTER "\033[0m"

const char* get_color_before(unsigned n)
{
    n = n % 14;
    static const char* labels[14] = {
        "\x1B[31m",
        "\x1B[32m",
        "\x1B[33m",
        "\x1B[34m",
        "\x1B[35m",
        "\x1B[36m",
        "\x1B[37m",
        "\x1B[91m",
        "\x1B[92m",
        "\x1B[93m",
        "\x1B[94m",
        "\x1B[35m",
        "\x1B[96m",
        "\x1B[97m"
    };

    return labels[n];
}

const char* get_marker(char c) {
    if (c == ' ')
        return " ";

    return (c-'A')/7 > 0 ? "X" : "*";
}

void print_bitmap(const Bitmap& bitmap) {
    static char line_buff[1024];

    printf("--------------------------------\n");
    for (int y = 0; y < bitmap.getHeight(); y++) {

        strcpy(line_buff, "|");
        for (int x = 0; x < bitmap.getWidth(); x++){
            const char c = bitmap.getC(x,y);
            strcat(line_buff, get_color_before(c%7));
            strcat(line_buff, get_marker(c));
            strcat(line_buff, get_marker(c));
            strcat(line_buff, get_marker(c));
            strcat(line_buff, COLOR_AFTER);
        }
        strcat(line_buff, "|");

        printf("%s\n", line_buff);
        printf("%s\n", line_buff);
    }
    printf("--------------------------------\n");
}

class MyNotifier : public ProgressNotifier {
    ShapeMap myCanvas;
    time_t last_timestamp;
    time_t started;

public:
    MyNotifier(const ShapeMap& canvas)
        : myCanvas(canvas)
        , last_timestamp(time(NULL))
        , started(last_timestamp)
    {}

    virtual void handleProgress(solving_event_t e, const ShapeSet& set, solving_info_t info) final {
        time_t now = time(NULL);

        if (now > last_timestamp) {
            float sol = (float)info.solutions/(now-started);
            float speed = (float)info.iterations/1000000/(now-started);
            printf("%4lu sol %5.1f sol/s - %5.1f Ma (%2.0f%%) %3.1f Mi/s %.2f sol/Mi\n"
                , info.solutions
                , sol
                , (float)info.attempts/1000000
                , (float)info.fits/info.attempts*100
                , speed
                , sol/speed
            );
            //solution.draw(myCanvas);
            //print_bitmap(myCanvas);
            last_timestamp = now;
        }
    }

    //time_t now = time(NULL);
    //printf("Finished!\n\n");
    //printf("Found %5ld solutions in %lu seconds\n", info.solutions, now-started);
};


int main() {
    ShapeMap canvas(10, 6);
    MyNotifier notifier(canvas);
    Solver solver(shapes, canvas, notifier);

    solver.solve();
}
