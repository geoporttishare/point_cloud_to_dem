#ifndef RASTER_H_
#define RASTER_H_

#include <string>
#include <iostream>

#include "io/GDAL_help.h"
#include "framework/geo.h"
#include "framework/coordinates.h"
#include "framework/RasterArea.h"


/**
 * \brief A general class to represent a block of 2D grid data.
 */
template<typename T>
class Raster {
public:
    using value_type = T;
    /**
     * \brief A constructor to create a new Raster from scratch.
     */
    Raster(
            const geo::RasterArea & area,
            const std::string & name);

    ~Raster();

    coordinates::raster_coord_type pixel_width() const;
    coordinates::raster_coord_type pixel_height() const;

    const geo::RasterArea & area() const;

    std::string name() const;

    T* data();
    T const * data() const;
    T value_at(const geo::PixelCenterCoordinate &) const;

    /**
     * \brief Set all the data values to \a val.
     */
    void format(T);
    /**
     * \brief Format the array with no_data values.
     */
    void format();

    /**
     * \brief Free the local data array.
     */
    void freeData();
    /**
     * \brief Return the number of cells in the allocated array.
     */
    size_t array_size() const;

    bool has_no_data_value() const;
    value_type no_data_value() const;
    void no_data_value(value_type value);
    GDALDataType data_type() const;

    geo::PixelCenterCoordinate to_geocoordinate(
        const coordinates::RasterCoordinate &) const;

    size_t to_raster_index(const coordinates::RasterCoordinate &) const;

    bool is_allocated() const;

private:
    std::string name_;
    geo::RasterArea area_;
    std::vector<T> data_;
    std::map<std::string, value_type> special_values_;
};







template<typename T>
Raster<T>::Raster(
        const geo::RasterArea & area__,
        const std::string &name__):
    name_ {name__},
    area_ {area__}
{
}

template<typename T>
Raster<T>::~Raster()
{
}

template<typename T>
bool Raster<T>::is_allocated() const
{
    return data_.size() > 0;
}

template<typename T>
T* Raster<T>::data()
{
    return data_.data();
}

template<typename T>
T const * Raster<T>::data() const
{
    return data_.data();
}

template<typename T>
T Raster<T>::value_at(const geo::PixelCenterCoordinate &gc) const
{
    coordinates::RasterCoordinate c {area().to_raster_coordinate(gc)};
    return data()[c.row() * pixel_width() + c.col()];
}

template<typename T>
void Raster<T>::format(T val)
{
    data_ = std::vector<T>(pixel_width() * pixel_height(), val);
}

template<typename T>
void Raster<T>::format()
{
    if (!has_no_data_value()) {
        std::stringstream ss;
        ss << "Function format() called but the Raster '" << name_
            << "' does not have no_data_value set.";
        throw std::runtime_error(ss.str());
    }
    format(no_data_value());
}

template<typename T>
void Raster<T>::freeData()
{
    data_.clear();
}

template<typename T>
size_t Raster<T>::array_size() const {
    return data_.size();
}

template<typename T>
bool Raster<T>::has_no_data_value() const
{
    return special_values_.count("nodata");
}

template<typename T>
T Raster<T>::no_data_value() const
{
    if (!has_no_data_value()) {
        std::stringstream ss;
        ss << "Container '" << name_ << "' does not have no_data value set.";
        throw std::runtime_error(ss.str());
    }
    return special_values_.at("nodata");
}

template<typename T>
void Raster<T>::no_data_value(value_type val)
{
    special_values_["nodata"] = val;
}

template<typename T>
GDALDataType Raster<T>::data_type() const
{
    return io::GDAL::toGDALDataType<T>();
}

template<typename T>
std::string Raster<T>::name() const
{
    return name_;
}

template<typename T>
geo::PixelCenterCoordinate Raster<T>::to_geocoordinate(
    const coordinates::RasterCoordinate &c) const
{
    return area_.to_geocoordinate(c);
}

template<typename T>
size_t Raster<T>::to_raster_index(
        const coordinates::RasterCoordinate & c) const
{
    return coordinates::to_raster_index(c, pixel_width());
}

template<typename T>
coordinates::raster_coord_type Raster<T>::pixel_width() const
{
    return area_.pixel_width();
}

template<typename T>
coordinates::raster_coord_type Raster<T>::pixel_height() const
{
    return area_.pixel_height();
}

template<typename T>
const geo::RasterArea & Raster<T>::area() const
{
    return area_;
}

#endif
