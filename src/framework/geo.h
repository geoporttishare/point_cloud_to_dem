#ifndef GEO_H_
#define GEO_H_

#include <iostream>
#include <cmath>


namespace geo {

    class TypeGeoCoordinate { };
    class TypePixelCenterGeoCoordinate: public TypeGeoCoordinate { };
    class TypePixelTopLeftCoordinate: public TypeGeoCoordinate { };

    class PixelCenterCoordinate;

    class GeoCoordinate
    {
        public:
            using coordinate_type = TypeGeoCoordinate;

            GeoCoordinate();
            GeoCoordinate(double, double);
            GeoCoordinate(const PixelCenterCoordinate &);

            double x() const;
            double y() const;

        private:
            double x_;
            double y_;
    };

    class PixelCenterCoordinate
    {
        public:
            using coordinate_type = TypePixelCenterGeoCoordinate;

            PixelCenterCoordinate(double, double);

            double x() const;
            double y() const;

        private:
            double x_;
            double y_;
    };

    class PixelTopLeftCoordinate
    {
        public:
            using coordinate_type = TypePixelTopLeftCoordinate;

            PixelTopLeftCoordinate(double, double);

            double x() const;
            double y() const;

        private:
            double x_;
            double y_;
    };

    class GeoDims {
        public:
            GeoDims(double, double);

            double width() const;
            double height() const;

        private:
            double x_;
            double y_;
    };

}

#endif
