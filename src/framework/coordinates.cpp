#include "coordinates.h"

#include <stdexcept>

namespace coordinates {

    RasterCoordinate::RasterCoordinate():
        x_ {0}, y_ {0}
    {
    }

    RasterCoordinate::RasterCoordinate(
            raster_coord_type x,
            raster_coord_type y):
        x_ {x}, y_ {y}
    {
    }

    raster_coord_type RasterCoordinate::col() const
    {
        return x_;
    }

    raster_coord_type RasterCoordinate::row() const
    {
        return y_;
    }

    RasterDims::RasterDims(raster_coord_type x, raster_coord_type y):
        x_ {x}, y_ {y}
    {
    }

    raster_coord_type RasterDims::cols() const
    {
        return x_;
    }

    raster_coord_type RasterDims::rows() const
    {
        return y_;
    }

    RasterDims RasterDims::create(size_t x, size_t y)
    {
        if (x > std::numeric_limits<raster_coord_type>::max()) {
            throw std::runtime_error("RasterDims cols too large.");
        }
        if (y > std::numeric_limits<raster_coord_type>::max()) {
            throw std::runtime_error("RasterDims rows too large.");
        }
        return {static_cast<raster_coord_type>(x),
                static_cast<raster_coord_type>(y)};
    }

}
