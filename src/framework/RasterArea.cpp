#include "RasterArea.h"

#include <iostream>
#ifdef DEBUG
#include <cassert>
#endif

namespace geo {

    RasterArea::RasterArea():
        RasterArea {PixelCenterCoordinate {0, 0},
            coordinates::RasterDims {0, 0},
            0,
            ReferenceSystem()}
    {
    }

    RasterArea::RasterArea(
                     const PixelCenterCoordinate &ul,
                     const coordinates::RasterDims &dims,
                     double cellsize,
                     const ReferenceSystem &ref_sys):
            Area {{ul.x() - cellsize / 2, ul.y() + cellsize / 2},
                     {static_cast<double>(dims.cols()) * cellsize,
                      static_cast<double>(dims.rows()) * cellsize},
                      ref_sys},
        pixel_dims_ {dims},
        cell_size_ {cellsize}
    {
    }

    RasterArea::RasterArea(
                     const PixelTopLeftCoordinate &ul,
                     const coordinates::RasterDims &dims,
                     double cellsize,
                     const ReferenceSystem &ref_sys):
            Area {{ul.x(), ul.y()},
                     {static_cast<double>(dims.cols()) * cellsize,
                      static_cast<double>(dims.rows()) * cellsize},
                      ref_sys},
        pixel_dims_ {dims},
        cell_size_ {cellsize}
    {
    }

    RasterArea::RasterArea(
                     const PixelCenterCoordinate &ul,
                     const PixelCenterCoordinate &lr,
                     double cellsize,
                     const ReferenceSystem &ref_sys):
            Area {{ul.x() - cellsize / 2, ul.y() + cellsize / 2},
                     {lr.x() - ul.x() + cellsize, ul.y() - lr.y() + cellsize},
                     ref_sys},
        pixel_dims_ {0, 0},
        cell_size_ {cellsize}
    {
        auto width_ = lr.x() - ul.x();
        auto height_ = ul.y() - lr.y();
        size_t i_width = static_cast<size_t>(std::round(width_ / cell_size_));
        size_t i_height = static_cast<size_t>(std::round(height_ / cell_size_));

        double w {static_cast<double>(i_width) * cell_size_};
        double h {static_cast<double>(i_height) * cell_size_};
        if (std::abs(width_ - w) > (cell_size_ / 100) ||
            std::abs(height_ - h) > (cell_size_ / 100))
            throw std::runtime_error("The given ul and lr points "
                "do not match to the given cell size.");

        pixel_dims_ = coordinates::RasterDims::create(i_width + 1, i_height + 1);
    }

    RasterArea::RasterArea(
            const Area &ga,
            const PixelCenterCoordinate &base_coord,
            double cellsize):
            Area {ga.top_left(),
                     {ga.right() - ga.left(), ga.top() - ga.bottom()},
                     ga.CRS()},
        pixel_dims_ {0, 0},
        cell_size_ {cellsize}
    {

        adjust_to(base_coord);
        adjust_dims(ga);
    }

    RasterArea::RasterArea(
            const Area &ga,
            const PixelTopLeftCoordinate &topleft_coord,
            double cellsize):
            Area {ga.top_left(),
                     {ga.right() - ga.left(), ga.top() - ga.bottom()},
                     ga.CRS()},
        pixel_dims_ {0, 0},
        cell_size_ {cellsize}
    {
        PixelCenterCoordinate base_coord = geo::PixelCenterCoordinate {
            topleft_coord.x() + cellsize / 2, topleft_coord.y() - cellsize / 2};
        adjust_to(base_coord);
        adjust_dims(ga);
    }

    RasterArea::RasterArea(
            const Area &ga,
            double cellsize):
        Area {ga},
        pixel_dims_ {0, 0},
        cell_size_ {cellsize}
    {
        adjust_dims(ga);
    }

    /**
     * \brief Adjust the dimensions to (n * cs, m * cs), where n
     * and m are integers and n * cs and m * cs are as close to the original
     * dimensions are possible.
     */
    void RasterArea::adjust_dims(
            const Area &ga)
    {
        //using ct = typename coordinates::raster_coord_type;
        double cs {cell_size()};
        double w {ga.right() - ga.left()};
        double h {ga.top() - ga.bottom()};
        if (static_cast<size_t>(round(w / cs)) > std::numeric_limits<coordinates::raster_coord_type>::max() ||
            static_cast<size_t>(round(h / cs)) > std::numeric_limits<coordinates::raster_coord_type>::max()) {
            throw std::runtime_error("adjust_dims: raster dimensions out of range.");
        }

        coordinates::raster_coord_type cols {static_cast<coordinates::raster_coord_type>(round(w / cs))};
        coordinates::raster_coord_type rows {static_cast<coordinates::raster_coord_type>(round(h / cs))};
        pixel_dims_ = coordinates::RasterDims::create(cols, rows);
        dims_ = GeoDims {static_cast<double>(pixel_dims_.cols()) * cs,
                         static_cast<double>(pixel_dims_.rows()) * cs};
    }

    void RasterArea::add_halo(double width)
    {
        //using ct = typename coordinates::raster_coord_type;
        double cs {cell_size()};
        int sign {width > 0 ? 1 : -1};
        int iwidth {sign * static_cast<int>(std::ceil(fabs(width) / cs))};
        width = iwidth * cs;
        Area::add_halo(width);
        pixel_dims_ = coordinates::RasterDims {
            static_cast<coordinates::raster_coord_type>(static_cast<int>(pixel_dims_.cols()) + 2 * iwidth),
            static_cast<coordinates::raster_coord_type>(static_cast<int>(pixel_dims_.rows()) + 2 * iwidth )};
    }


    /**
     * \brief The (ulx, uly) is moved to the closest grid point defined by
     * the \a coords and the cell size of the RasterArea. The dimensions are
     * not altered.
     */
    void RasterArea::adjust_to(const PixelCenterCoordinate &coords)
    {
        double cs = cell_size();

        double new_ulx = coords.x() + round((ulx() - coords.x()) / cs) * cs;
        double new_uly = coords.y() + round((uly() - coords.y()) / cs) * cs;
        {
            double dx = std::abs(new_ulx - ulx());
            double dy = std::abs(new_uly - uly());
            if (dx / cs > 0.1 || dy / cs > 0.1)
                std::cout << "NOTE: adjusting RasterArea ' by ("
                    << (new_ulx - ulx()) << ", " << (new_uly - uly())
                    << ")" << std::endl;

        }
        top_left_ = GeoCoordinate {new_ulx - cs / 2, new_uly + cs / 2};
    }

    double RasterArea::cell_size() const
    {
        return cell_size_;
    }

    coordinates::raster_coord_type RasterArea::pixel_width() const {
        return pixel_dims_.cols();
    }

    coordinates::raster_coord_type RasterArea::pixel_height() const {
        return pixel_dims_.rows();
    }

    coordinates::RasterDims RasterArea::raster_dims() const
    {
        return pixel_dims_;
    }

    double RasterArea::ulx() const {
        return top_left_.x() + cell_size_ / 2;
    }

    double RasterArea::uly() const {
        return top_left_.y() - cell_size_ / 2;
    }

    double RasterArea::lrx() const {
        return ulx() + _width();
    }

    double RasterArea::lry() const {
        return uly() - _height();
    }

    double RasterArea::_width() const {
        return static_cast<double>(pixel_dims_.cols() - 1) * cell_size_;
    }

    double RasterArea::_height() const {
        return static_cast<double>(pixel_dims_.rows() - 1) * cell_size_;
    }

    coordinates::raster_coord_type RasterArea::to_pixel_x(double xval) const
    {
        int ret {static_cast<int>(std::round((xval - ulx()) / cell_size_))};
        if (ret < 0)
            throw std::runtime_error("to_pixel_x returned a negative raster coordinate.");
        return static_cast<unsigned int>(ret);
    }

    coordinates::raster_coord_type RasterArea::to_pixel_y(double yval) const
    {
        int ret {static_cast<int>(std::round((uly() - yval) / cell_size_))};
        if (ret < 0)
            throw std::runtime_error("to_pixel_y returned a negative raster coordinate.");
        return static_cast<unsigned int>(ret);
    }

    coordinates::RasterCoordinate RasterArea::to_raster_coordinate(
        const PixelCenterCoordinate &c) const
    {
        return coordinates::RasterCoordinate {to_pixel_x(c.x()), to_pixel_y(c.y())};
    }

    PixelCenterCoordinate RasterArea::to_geocoordinate(
        const coordinates::RasterCoordinate &c) const
    {
        return {left() + (c.col() + 0.5) * cell_size(),
                top() - (0.5 + c.row()) * cell_size()};
    }

    PixelCenterCoordinate RasterArea::ul_corner() const
    {
        return {top_left_.x() + cell_size_ / 2,
                top_left_.y() - cell_size_ / 2};
    }

    void RasterArea::ul_corner(const PixelCenterCoordinate &gc)
    {
        top_left_ = {gc.x() - cell_size_ / 2,
                     gc.y() + cell_size_ / 2};
    }

    PixelCenterCoordinate RasterArea::base_point() const
    {
        double cs = cell_size_;
        double base_x = ulx();
        int n = static_cast<int>(std::floor(base_x / cs));
        base_x -= n * cs;
        double base_y = uly();
        n = static_cast<int>(std::floor(base_y / cs));
        base_y -= n * cs;
        return {base_x, base_y};
    }

    bool RasterArea::contains_point(const PixelCenterCoordinate &c) const
    {
        if (c.x() < left() || c.x() > right() ||
            c.y() < bottom() || c.y() > top()) return false;
        return true;
    }

    RasterArea RasterArea::sub_area(
        const coordinates::RasterCoordinate &ul_pix,
        const coordinates::RasterDims &pix_dims) const
    {
        #ifdef DEBUG
        size_t col {ul_pix.col()};
        size_t row {ul_pix.row()};
        assert(col + pix_dims.cols() <= pixel_dims_.cols());
        assert(row + pix_dims.rows() <= pixel_dims_.rows());
        #endif

        PixelCenterCoordinate ul {
            ulx() + ul_pix.col() * cell_size_,
            uly() - ul_pix.row() * cell_size_};
        return RasterArea { ul, pix_dims, cell_size_, CRS() };
    }

    std::ostream & operator<<(std::ostream &os, const RasterArea &g)
    {
        os << "RasterArea:" << std::endl;
        os << "  (pwidth, pheight) = (" << g.pixel_width() << "px x " <<
            g.pixel_height() << "px)" << std::endl;
        os << "   pixel size: " << g.cell_size() << "m" << std::endl;
        os << "  (width, height) = (" << g.right() - g.left() << "m x " <<
            g.top() - g.bottom() << "m)" << std::endl;
        os << "  (xmin, xmax, ymin, ymax) = (" << std::fixed << g.left() <<
            ", " << g.right() << ", " << g.bottom() << ", " << g.top() << ")\n";
        os << "  (ulx,uly) = (" << std::fixed << g.ulx() << ", "
           << std::fixed << g.uly() << ")" <<
            std::endl;
        return os;
    }

}
