#include "geo.h"

namespace geo {

    GeoCoordinate::GeoCoordinate():
        x_ {0}, y_ {0}
    {
    }

    GeoCoordinate::GeoCoordinate(double x__, double y__):
        x_ {x__}, y_ {y__}
    {
    }

    GeoCoordinate::GeoCoordinate(const PixelCenterCoordinate &c):
        x_ {c.x()}, y_ {c.y()}
    {
    }

    double GeoCoordinate::x() const
    {
        return x_;
    }

    double GeoCoordinate::y() const
    {
        return y_;
    }

    PixelCenterCoordinate::PixelCenterCoordinate(double x__, double y__):
        x_ {x__}, y_ {y__}
    {
    }

    double PixelCenterCoordinate::x() const
    {
        return x_;
    }

    double PixelCenterCoordinate::y() const
    {
        return y_;
    }

    PixelTopLeftCoordinate::PixelTopLeftCoordinate(double x__, double y__):
        x_ {x__}, y_ {y__}
    {
    }

    double PixelTopLeftCoordinate::x() const
    {
        return x_;
    }

    double PixelTopLeftCoordinate::y() const
    {
        return y_;
    }

    GeoDims::GeoDims(double x, double y):
        x_ {x}, y_{y}
    {
    }

    double GeoDims::width() const
    {
        return x_;
    }

    double GeoDims::height() const
    {
        return y_;
    }

}
