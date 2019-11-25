#ifndef GDAL_HELP_H_
#define GDAL_HELP_H_

#include <string>

#include <gdal_priv.h>

namespace geo {
    class RasterArea;
}

namespace io {

    namespace GDAL {

        enum class RW_MODE {
            READ,
            WRITE
        };

        void array_file_rw_(
            char * array,
            const geo::RasterArea & array_area,
            GDALDataType value_type,
            GDALRasterBand *,
            RW_MODE mode);

        template<typename T>
        GDALDataType toGDALDataType();

        template<typename T>
        void array_file_rw(
            T * array,
            const geo::RasterArea & array_area,
            GDALRasterBand * band,
            RW_MODE mode)
        {
            array_file_rw_(
                reinterpret_cast<char*>(array),
                array_area,
                toGDALDataType<T>(),
                band,
                mode);
        }

        template<typename T>
        GDALDataType toGDALDataType()
        {
            throw std::runtime_error("Unknown datatype");
        }

        template<> GDALDataType toGDALDataType<char>();
        template<> GDALDataType toGDALDataType<unsigned char>();
        template<> GDALDataType toGDALDataType<short>();
        template<> GDALDataType toGDALDataType<unsigned short>();
        template<> GDALDataType toGDALDataType<int>();
        template<> GDALDataType toGDALDataType<unsigned int>();
        template<> GDALDataType toGDALDataType<float>();

    }

}

#endif
