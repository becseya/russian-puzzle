#include "shapeset.hpp"
#include "flood-filler.hpp"

typedef size_t attempt_counter_t;

struct solving_info_t {
    attempt_counter_t attempts;
    attempt_counter_t fits;
    size_t solutions;
    size_t iterations;
};

enum solving_event_t {E_PLACED, E_SOLUTION, E_FINISHED};

struct ProgressNotifier {
    virtual void handleProgress(solving_event_t e, const ShapeSet& set, solving_info_t info) = 0;
};


class Solver : public ShapeSet
{
    enum fit_result_t {FIT, FAIL, SOLUTION, CONTINUE};


    ShapeMap& canvas;
    ProgressNotifier& notifier;
    solving_info_t info;
    FloodFiller flooder;
    frame_limit_t frameLimit;
    FastVector<frame_limit_t> frameLimits;
    bool firstIsFixed;
    int firstX, firstY;

public:
    Solver(shapes_t& shapes, ShapeMap& canvas, ProgressNotifier& notifier)
        : ShapeSet(shapes)
        , canvas(canvas)
        , notifier(notifier)
        , info({.attempts = 0, .fits = 0, .solutions = 0, .iterations = 0})
        , flooder(canvas)
        , frameLimit({.minX = 0, .minY = 0, .maxX = (canvas.getWidth()-1), .maxY = (canvas.getHeight()-1)})
        , firstIsFixed(false)
    {}

    void solve(bool fixed = false) {
        firstIsFixed = fixed;

        fitNextRecursive();
        notify(E_FINISHED);
    }

    void solve(int first_x, int first_y) {
        firstX = first_x;
        firstY = first_y;

        solve(true);
    }

private:
    bool tryToFitShape(const Shape& shape_to_fit, shape_desc_t& desc, bool skip_first, bool first_fit) {
        const size_t max_var = shape_to_fit.getNumOfVariants();

        if (skip_first)
            desc.x++;

        for (size_t var = desc.var; var < max_var; var++) {
            const Bitmap& b = shape_to_fit.getVariant(var);
            const int max_y = (frameLimit.maxY + 1) - (b.getHeight() - 1);
            const int max_x = (frameLimit.maxX + 1) - (b.getWidth() - 1);

            for (int y = desc.y; y < max_y; y++) {
                for (int x = desc.x; x < max_x; x++) {
                    info.iterations++;

                    if (canvas.placeIfNoOverlap(b, x, y)) {
                        flooder.draw(b, x, y);
                        desc.var = var;
                        desc.x = x;
                        desc.y = y;
                        descriptors.push(desc);
                        return true;
                    }
                    else if(first_fit)
                        return false;
                }
                desc.x = frameLimit.minX;
            }
            desc.y = frameLimit.minY;
        }

        return false;
    }

    fit_result_t tryToFit(bool refit = false, bool first_fit = false) {
        bool fitted;
        shape_desc_t desc;

        if (refit)
            desc = undrawLast();
        else {
            desc.var = 0;

            if (first_fit) {
                desc.x = firstX;
                desc.y = firstY;
            }
            else {
                desc.x = frameLimit.minX;
                desc.y = frameLimit.minY;
            }

            static_cast<frame_limit_t&>(desc) = frameLimit;
        }

        do {
            info.attempts++;
            fitted = tryToFitShape(shapes[descriptors.size()], desc, refit, first_fit);

            if (fitted) {
                refit = !fieldsAreOk();

                if (refit && first_fit)
                    return FAIL;

                notify(E_PLACED);

                if (refit)
                    undrawLast(false);
            }
        } while(fitted && refit);

        info.fits++;

        if (!fitted)
            return FAIL;
        else {
            updateFrameLimit();
            return ((descriptors.size() == shapes.size()) ? SOLUTION : FIT);
        }
    }

    fit_result_t fitNextRecursive(int lvl = 0, bool refit = false) {
        fit_result_t res;

        do {
            res = tryToFit(refit, (firstIsFixed && (lvl == 0)));

            if (res == FIT)
                res = fitNextRecursive(lvl+1);
            else if (res == SOLUTION) {
                info.solutions++;
                notify(E_SOLUTION);
                undrawLast();
                return CONTINUE;
            }
            else
                return res;

            if (firstIsFixed && (lvl == 0))
                return res;

            refit = true;
        } while (descriptors.size() > 0);

        return res;
    }

    shape_desc_t undrawLast(bool pop_frame = true) {
        shape_desc_t desc = ShapeSet::undrawLast(canvas);
        const Bitmap& b = shapes[descriptors.size()].getVariant(desc.var);
        flooder.undraw(b, desc.x, desc.y);

        if (pop_frame) {
            frameLimit = (frame_limit_t)desc;
        }

        return desc;
    }

    bool fieldsAreOk() {
        size_t filled;

        while ((filled = flooder.findNextField())) {
            if ((filled % 6) != 0)
                return false;
        }

        return true;
    }

    bool isColFilled (int y) {
        for (int x = 0; x < canvas.getWidth(); x++)
            if (!canvas.get(x, y))
                return false;

        return true;
    }

    bool isRowFilled (int x) {
        for (int y = 0; y < canvas.getHeight(); y++)
            if (!canvas.get(x, y))
                return false;

        return true;
    }

    void updateFrameLimit() {
        // cols
        while (isColFilled(frameLimit.minY))
            frameLimit.minY++;

        while (isColFilled(frameLimit.maxY))
            frameLimit.maxY--;

        // rows
        while (isRowFilled(frameLimit.minX))
            frameLimit.minX++;

        while (isRowFilled(frameLimit.maxX))
            frameLimit.maxX--;
    }

    void notify(solving_event_t e) {
        notifier.handleProgress(e, *this, info);
    }
};
