#ifndef BOUNDING_BOX_H_
#define BOUNDING_BOX_H_

#include <string>

#include "framework/geo.h"

namespace geo {

    class BoundingBox
    {
        public:
            BoundingBox();

            void add(const geo::GeoCoordinate &p);

            template<typename T>
            bool overlaps_with(const T &) const;

        private:
            geo::GeoCoordinate top_left_;
            geo::GeoCoordinate bottom_right_;
    };










    template<typename T>
    bool BoundingBox::overlaps_with(const T &t) const
    {
        if (top_left_.x() > t.right() ||
            bottom_right_.x() < t.left() ||
            top_left_.y() < t.bottom() ||
            bottom_right_.y() > t.top()) return false;
        else
            return true;
    }

}

#endif
