#ifndef TIN_H_
#define TIN_H_

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

namespace io {

    namespace point_cloud {

        class Interpolator;

        class TIN
        {
            friend class io::point_cloud::Interpolator;

            public:
                using K = CGAL::Exact_predicates_inexact_constructions_kernel;
                using Delaunay_triangulation = CGAL::Delaunay_triangulation_2<K>;
                using Point = K::Point_2;

                TIN();
                TIN(const TIN &) = delete;
                virtual ~TIN();

                void insert_point(const Point &);

                size_t number_of_points() const;

            private:
                Delaunay_triangulation T_;
        };

    }

}

#endif
