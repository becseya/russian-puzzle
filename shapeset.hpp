#include "shape.hpp"

struct frame_limit_t {
    int minX, minY, maxX, maxY;
};

struct shape_desc_t : public frame_limit_t {
    size_t var;
    int x;
    int y;
};

typedef const FastVector<Shape> shapes_t;
typedef FastVector<shape_desc_t> descriptors_t;

struct ShapeSet {
    shapes_t& shapes;
    descriptors_t descriptors;

public:
    ShapeSet(shapes_t& shapes)
        : shapes(shapes)
        , descriptors()
    {}

    ShapeSet(shapes_t& shapes, const descriptors_t& descriptors)
        : shapes(shapes)
        , descriptors(descriptors)
    {}

    void draw(Bitmap& canvas) const {
        canvas.clear();

        for (size_t i = 0; i < descriptors.size(); i++)
            canvas.draw(shapes[i].getVariant(descriptors[i].var),
                             descriptors[i].x,
                             descriptors[i].y);
    }

    shape_desc_t undrawLast(ShapeMap& canvas) {
        shape_desc_t desc;

        descriptors.pop(&desc); // descriptors.size() now returns shape correlating to popped desc
        canvas.undraw(shapes[descriptors.size()].getVariant(desc.var), desc.x, desc.y);
        return desc;
    }
};
