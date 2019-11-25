#include "Interpolator.h"

namespace io {

    namespace point_cloud {

        Interpolator::Interpolator():
            tin_ptr_ {new TIN()}
        {
        }

        Interpolator::~Interpolator()
        {
        }

        size_t Interpolator::number_of_points() const
        {
            return tin_ptr_->number_of_points();
        }

        Interpolator::Interpolator(Interpolator &&ip)
        {
            std::swap(tin_ptr_, ip.tin_ptr_);
            std::swap(function_values_, ip.function_values_);
        }

        void Interpolator::insert_point(const geo::GeoCoordinate &p, Coord_type elev)
        {
            TIN::Point p2(p.x(), p.y());
            tin_ptr_->insert_point(p2);
            function_values_.insert(std::make_pair(p2, elev));
        }

        double Interpolator::get_value_at(const geo::GeoCoordinate &p, bool safe) const
        {
            return get_value_at(TIN::Point(p.x(), p.y()), safe);
        }

        double Interpolator::get_value_at(const TIN::Point &p, bool /*safe*/) const
        {
            typedef CGAL::Data_access< std::map<TIN::Point, Coord_type, TIN::K::Less_xy_2 > >
                Value_access;
            std::vector< std::pair< TIN::Point, Coord_type > > coords;
            Coord_type norm =
                CGAL::natural_neighbor_coordinates_2
                (tin_ptr_->T_, p, std::back_inserter(coords)).second;
            Coord_type res = CGAL::linear_interpolation(
                coords.begin(), coords.end(), norm, Value_access(function_values_));
            return res;
        }

    }

}
