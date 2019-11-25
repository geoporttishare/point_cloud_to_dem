#ifndef GEO_AREA_H_
#define GEO_AREA_H_

#include "geo.h"

#include <vector>

#include "ReferenceSystem.h"

namespace geo {

    class Area;

    //bool are_overlapping(const Area &, const Area &);

    /**
     * \brief Area determined by georeferences coordinates.
     *
     *       A---------------+      A = (left, top)
     *       |               |      B = (right, bottom)
     *       |               |
     *       |               |
     *       |               |
     *    y  |               |
     *    ^  |               |
     *    |  |               |
     *    |  +---------------B
     *    +----> x
     */
    class Area
    {
        public:
            Area();
            Area(
                const GeoCoordinate & topleft,
                const GeoDims & dims,
                const ReferenceSystem &);

            Area(const Area &) = default;

            Area & operator=(const Area &) = default;

            virtual ~Area() = default;

            double left() const;
            double right() const;
            double top() const;
            double bottom() const;
            GeoCoordinate top_left() const;
            ReferenceSystem CRS() const;
            bool contains_point(const geo::GeoCoordinate &) const;

            void CRS(const ReferenceSystem &);

            std::vector<std::string> to_vector_str() const;

            void add_halo(double width);

        protected:
            GeoCoordinate top_left_;
            GeoDims dims_;
            ReferenceSystem ref_system_;
    };

    std::ostream & operator<<(std::ostream &, const Area &);

    Area parse_rectangle_coordinates(
        const std::string &s, const std::string &ref_sys);

    Area parse_rectangle_coordinates(
        const std::vector<std::string> &v, const std::string &ref_sys);

}

#endif
