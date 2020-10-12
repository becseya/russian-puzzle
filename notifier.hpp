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

class MyNotifier : public ProgressNotifier {
    ShapeMap canvas;
    time_t last_timestamp;
    time_t started;

public:
    MyNotifier(const ShapeMap& canvas)
        : canvas(canvas)
        , last_timestamp(time(NULL))
        , started(last_timestamp)
    {}

    virtual void handleProgress(solving_event_t e, const ShapeSet& set, solving_info_t info) final {
        time_t now = time(NULL);

        if (now > last_timestamp) {
            float sol = (float)info.solutions/(now-started);
            float speed = (float)info.iterations/1000000/(now-started);

            printf("%4lu sol %5.1f sol/s - %5.1f Ma (%2.0f%%) %5.1f Mi/s %5.1f sol/Mi\n"
                , info.solutions
                , sol
                , (float)info.attempts/1000000
                , (float)info.fits/info.attempts*100
                , speed
                , sol/speed
            );
            //solution.draw(canvas);
            //print_bitmap(canvas);
            last_timestamp = now;
        }
    }

    //time_t now = time(NULL);
    //printf("Finished!\n\n");
    //printf("Found %5ld solutions in %lu seconds\n", info.solutions, now-started);
};

