#ifndef COORDINATES_H_
#define COORDINATES_H_

#include <iostream>
#include <stdexcept>
#include <limits>
#include <cmath>

namespace coordinates {

    using raster_coord_type = unsigned int;

    class RasterCoordinate;
    class RasterDims;

    class RasterCoordinate {
        public:
            RasterCoordinate();
            RasterCoordinate(raster_coord_type, raster_coord_type);

            raster_coord_type col() const;
            raster_coord_type row() const;

        private:
            raster_coord_type x_;
            raster_coord_type y_;
    };

    inline size_t to_raster_index(size_t col, size_t row, size_t width)
    {
        return row * width + col;
    }

    inline size_t to_raster_index(const RasterCoordinate &c, size_t nx)
    {
        return to_raster_index(c.col(), c.row(), nx);
    }

    class RasterDims {
        public:
            RasterDims(raster_coord_type, raster_coord_type);

            raster_coord_type cols() const;
            raster_coord_type rows() const;

            static RasterDims create(size_t, size_t);

        private:
            raster_coord_type x_;
            raster_coord_type y_;
    };

}

#endif
