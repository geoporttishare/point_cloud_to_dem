#include "BoundingBox.h"

#include <limits>

namespace geo {

    BoundingBox::BoundingBox():
        top_left_ {std::numeric_limits<double>::max(),
                   std::numeric_limits<double>::lowest()},
        bottom_right_ {std::numeric_limits<double>::lowest(),
                       std::numeric_limits<double>::max()}
    {
    }

    void BoundingBox::add(const geo::GeoCoordinate &p)
    {
        top_left_ = geo::GeoCoordinate {
            std::min(top_left_.x(), p.x()),
            std::max(top_left_.y(), p.y()) };

        bottom_right_ = geo::GeoCoordinate {
            std::max(bottom_right_.x(), p.x()),
            std::min(bottom_right_.y(), p.y()) };
    }

}
