#ifndef INTERPOLATOR_H_
#define INTERPOLATOR_H_

#include <CGAL/Interpolation_traits_2.h>
#include <CGAL/natural_neighbor_coordinates_2.h>
#include <CGAL/interpolation_functions.h>

#include "TIN.h"
#include "framework/coordinates.h"
#include "framework/geo.h"

namespace io {

    namespace point_cloud {

        class Interpolator
        {
            public:
                using Coord_type = TIN::K::FT;
                using Traits = CGAL::Interpolation_traits_2<TIN::K>;

                Interpolator();
                virtual ~Interpolator();
                Interpolator(const Interpolator &) = delete;
                Interpolator(Interpolator &&);

                void insert_point(const geo::GeoCoordinate &, Coord_type elev);
                double get_value_at(const geo::GeoCoordinate &, bool = true) const;
                size_t number_of_points() const;

                template<typename C>
                void fill_array(
                    const geo::PixelCenterCoordinate & upper_left,
                    double resolution,
                    C * data_array,
                    unsigned int nx,
                    unsigned int row_start,
                    unsigned int row_stop,
                    bool use_prev_hint);
            private:
                std::unique_ptr<TIN> tin_ptr_;
                std::map<TIN::Point, Coord_type, TIN::K::Less_xy_2> function_values_;
                TIN::Delaunay_triangulation::Face_handle fh_hint_;

                double get_value_at(const TIN::Point &p, bool = true) const;

        };

        template<typename C>
        void Interpolator::fill_array(
            const geo::PixelCenterCoordinate & upper_left,
            double resolution,
            C * data_array,
            unsigned int nx,
            unsigned int row_start,
            unsigned int row_stop,
            bool use_prev_hint)
        {
            TIN::Delaunay_triangulation::Face_handle fh, fh_row_begin;
            TIN::Point ul(upper_left.x(), upper_left.y());
            if (use_prev_hint)
                fh = fh_hint_;
            else
                fh = tin_ptr_->T_.locate(ul);
            fh_row_begin = fh;
            using Value_access = CGAL::Data_access<std::map<TIN::Point, Coord_type, TIN::K::Less_xy_2>>;
            for (unsigned int j = row_start; j < row_stop; ++j) {
                for (unsigned int i = 0; i < nx; ++i) {
                    std::vector<std::pair<TIN::Point, Coord_type>> coords;
                    TIN::Point p(upper_left.x() + i * resolution,
                                 upper_left.y() - j * resolution);
                    if (i == 0) {
                        fh_row_begin = tin_ptr_->T_.locate(p, fh_row_begin);
                        fh = fh_row_begin;
                    } else {
                        fh = tin_ptr_->T_.locate(p, fh);
                    }
                    Coord_type norm = CGAL::natural_neighbor_coordinates_2(
                        tin_ptr_->T_, p, std::back_inserter(coords), fh).second;
                    if (coords.size() > 0) {
                        data_array[j * nx + i] = static_cast<C>(
                            CGAL::linear_interpolation(
                                coords.begin(), coords.end(), norm,
                                Value_access(function_values_)));
                    } else {
                        std::cout << "No data for cell (" << j << "," << i << ")" << std::endl;
                    }
                }
            }
            fh_hint_ = fh_row_begin;
        }

    }

}

#endif
