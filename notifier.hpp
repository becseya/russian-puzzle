#pragma once

#include <stdio.h>
#include <time.h>

#include "solver.hpp"


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

struct notifer_settings_t {
    int width, height;
    bool printSolutions = false;
    bool printPlacements = false;
    bool printBenchmark = true;
    bool printFinish = true;
};

class MyNotifier : public ProgressNotifier {
    ShapeMap canvas;

    time_t last_timestamp;
    time_t started;
    const notifer_settings_t& settings;

public:
    MyNotifier(const notifer_settings_t& settings)
        : canvas(settings.width, settings.height)
        , last_timestamp(time(NULL))
        , started(last_timestamp)
        , settings(settings)
    {}

    virtual void handleProgress(solving_event_t e, const ShapeSet& set, solving_info_t info) override {
        time_t now = time(NULL);

        if (settings.printBenchmark && (now > last_timestamp)) {
            printBenchmark(info, (now-started));
            last_timestamp = now;
        }

        if (settings.printPlacements || (settings.printSolutions && (e == E_SOLUTION))) {
            set.draw(canvas);
            print_bitmap(canvas);
        }

        if (settings.printFinish && (e == E_FINISHED)) {
            time_t now = time(NULL);
            printf("Finished!\n\n");
            printf("Found %5ld solutions in %lu seconds\n", info.solutions, now-started);
        }
    }

    static void printBenchmark(const solving_info_t& info, size_t dt) {
        float sol = (float)info.solutions/dt;
        float speed = (float)info.iterations/1000000/dt;

        printf("%5lu sol %4.0f sol/s - %4lu Ma (%2.0f%%) %3.0f Mi/s %5.1f sol/Mi\n"
            , info.solutions
            , sol
            , info.attempts/1000000
            , (float)info.fits/info.attempts*100
            , speed
            , sol/speed
        );
    }
};
