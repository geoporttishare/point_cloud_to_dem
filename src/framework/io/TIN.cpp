#include "TIN.h"

namespace io {

    namespace point_cloud {

        TIN::TIN()
        {
        }

        TIN::~TIN()
        {
        }

        size_t TIN::number_of_points() const
        {
            return T_.number_of_vertices();
        }

        void TIN::insert_point(const Point &p)
        {
            T_.insert(p);
        }

    }

}
