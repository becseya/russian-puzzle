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

#ifdef MULTI_PROC

struct NotifyerMutex {
    virtual void lock() = 0;
    virtual void unlock() = 0;
};

class MyMultiprocessNotifier : public ProgressNotifier {
    const notifer_settings_t& settings;
    NotifyerMutex& mtx;
    solving_info_t myInfo;
    time_t myLastTimestamp;


    static FastVector<MyMultiprocessNotifier*> children;
    static time_t lastTimestamp;
    static time_t started;
    static solving_info_t totalInfo;


public:
    MyMultiprocessNotifier(const notifer_settings_t& settings, NotifyerMutex& mtx)
        : settings(settings)
        , mtx(mtx)
        , myInfo({0})
        , myLastTimestamp(time(NULL))
    {
        lock();
        born(this);
        unlock();
    }

    ~MyMultiprocessNotifier() {
        lock();
        die(this);
        unlock();
    }

    void lock() {
        mtx.lock();
    }

    void unlock() {
        mtx.unlock();
    }

    virtual void handleProgress(solving_event_t e, const ShapeSet& set, solving_info_t info) override {
        if (e == E_FINISHED) {
            lock();
            myInfo = info;
            unlock();
        }
        else if(settings.printBenchmark) {
            time_t now = time(NULL);

            if (now > myLastTimestamp) {

                lock();
                myInfo = info;
                collectStat(now);
                unlock();

                myLastTimestamp = now;
            }
        }
    }

    static void collectStat(time_t now) {
        if (lastTimestamp == now)
            return; // has been updated by someone else

        solving_info_t sum = totalInfo;
        for(size_t i = 0; i<children.size(); i++)
            addInfo(sum, children[i]->myInfo);

        printf("%lu children:", children.size());
        MyNotifier::printBenchmark(sum, now-started);
        lastTimestamp = now;
    }

    static void born(MyMultiprocessNotifier* child) {
        if (started == 0) {
            started = time(NULL);
            lastTimestamp = started;
        }

        children.push(child);
    }

    static void die(MyMultiprocessNotifier* child) {
        addInfo(totalInfo, child->myInfo);

        children.remove(child);
        if (children.size() == 0)
            finished();
    }

    static void finished() {
        time_t now = time(NULL);
        printf("Finished!\n\n");
        printf("Found %5ld solutions in %lu seconds\n", totalInfo.solutions, now-started);
    }

    static void addInfo(solving_info_t& sum, const solving_info_t& addent) {
        sum.attempts += addent.attempts;
        sum.fits += addent.fits;
        sum.iterations += addent.iterations;
        sum.solutions += addent.solutions;
    }
};

FastVector<MyMultiprocessNotifier*> MyMultiprocessNotifier::children;
solving_info_t MyMultiprocessNotifier::totalInfo = {0};
time_t MyMultiprocessNotifier::lastTimestamp = 0;
time_t MyMultiprocessNotifier::started = 0;

#endif
