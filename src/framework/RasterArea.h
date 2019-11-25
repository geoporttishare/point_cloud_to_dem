#ifndef GEO_RASTER_AREA_H_
#define GEO_RASTER_AREA_H_

#include "Area.h"
#include "coordinates.h"

namespace geo {

    /**
     * \brief Class to represent rasterized rectangular areas with
     * georeferences coordinates.
     *
     *       B-------+-------+      A = (ulx, uly)
     *       |       |       |      B = (ulx - cellsize/2, uly + cellsize/2)
     *       |   A   |       |        = (left, top)
     *       |       |       |
     *       +-------+-------+
     *    y  |       |       |      C = (lrx, lry)
     *    ^  |       |   C   |      D = (lrx + cellsize/2, lry - cellsize/2)
     *    |  |       |       |        = (right, bottom)
     *    |  +-------+-------D
     *    +----> x
     *
     * The (ulx, uly) is the center of the pixel.
     *
     * \a Note the pixel_width and pixel_height is the number of pixels,
     * whereas width and length are the distances of the center of pixels,
     * so e.g. pixel_width * cell_size = width + cell_size.
     */
    class RasterArea: public Area
    {
        public:

            RasterArea();
            RasterArea(const RasterArea &) = default;
            RasterArea(
                    const PixelCenterCoordinate &ul,
                    const coordinates::RasterDims &dims,
                    double cell_size,
                    const ReferenceSystem &);
            RasterArea(
                    const PixelTopLeftCoordinate &ul,
                    const coordinates::RasterDims &dims,
                    double cell_size,
                    const ReferenceSystem &);
            RasterArea(
                    const PixelCenterCoordinate &ul,
                    const PixelCenterCoordinate &lr,
                    double cs,
                    const ReferenceSystem &);
            RasterArea(
                    const Area &,
                    const PixelCenterCoordinate &,
                    double cellsize);
            RasterArea(
                    const Area &,
                    const PixelTopLeftCoordinate &,
                    double cellsize);

            RasterArea(
                const Area &,
                double cellsize);

            RasterArea & operator=(const RasterArea &) = default;

            void adjust_to(const PixelCenterCoordinate &);
            void add_halo(double width);

            coordinates::raster_coord_type pixel_width() const;
            coordinates::raster_coord_type pixel_height() const;
            coordinates::RasterDims raster_dims() const;

            double ulx() const;
            double uly() const;
            double lrx() const;
            double lry() const;
            double cell_size() const;

            /**
             * \brief Return the (ulx, uly) coordinates.
             */
            PixelCenterCoordinate ul_corner() const;

            coordinates::RasterCoordinate to_raster_coordinate(
                const PixelCenterCoordinate &c) const;

            PixelCenterCoordinate to_geocoordinate(
                const coordinates::RasterCoordinate &) const;

            /**
             * \brief Set the (ulx, uly) to the given coordinates.
             */
            void ul_corner(const PixelCenterCoordinate &);

            using Area::contains_point;
            bool contains_point(const geo::PixelCenterCoordinate &) const;

            RasterArea sub_area(
                const coordinates::RasterCoordinate &,
                const coordinates::RasterDims &) const;

            PixelCenterCoordinate base_point() const;

        private:
            coordinates::RasterDims pixel_dims_;
            double cell_size_;

            void adjust_dims(
                const Area &);

            void closest_to(
                const Area &,
                const GeoCoordinate &);

            double _width() const;
            double _height() const;

            coordinates::raster_coord_type to_pixel_x(double) const;
            coordinates::raster_coord_type to_pixel_y(double) const;
    };

    std::ostream & operator<<(std::ostream &, const RasterArea &);

}

#endif
