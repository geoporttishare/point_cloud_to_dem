#ifndef GDAL_RASTER_PRINTER_H_
#define GDAL_RASTER_PRINTER_H_

#include <iostream>

#include <gdal_priv.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "framework/io/GDAL_help.h"
#include "framework/RasterArea.h"
#include "GDAL_dataset_ptr.h"

namespace io {

    namespace GDAL {

        dataset_ptr create_data_file_(
            const boost::filesystem::path & file,
            const char * gdal_driver,
            const GDALDataType & data_type,
            const geo::RasterArea & area);

        template<typename Container>
        dataset_ptr create_data_file(
            const boost::filesystem::path & file,
            const char * gdal_driver,
            Container & container,
            const geo::RasterArea & area)
        {
            dataset_ptr ds = create_data_file_(
                file, gdal_driver, container.data_type(), area);

            if (container.has_no_data_value())
            {
                GDALRasterBand * band = ds->GetRasterBand(1);
                if (band->SetNoDataValue(static_cast<double>(
                    container.no_data_value())) == CE_Failure)
                {
                        std::cout << "Failed to set nodata_value for "
                            << "the created data set." << std::endl;
                }
            }
            return ds;
        }

        template<typename Raster>
        void write(
            Raster & raster,
            const boost::filesystem::path &file,
            const std::string & format)
        {
            dataset_ptr ds;
            ds = GDAL::create_data_file(
                file, format.c_str(), raster,
                raster.area());

            GDALRasterBand * band = ds->GetRasterBand(1);

            GDAL::array_file_rw(
                raster.data(),
                raster.area(),
                band,
                GDAL::RW_MODE::WRITE);
        }

    }

}

#endif
