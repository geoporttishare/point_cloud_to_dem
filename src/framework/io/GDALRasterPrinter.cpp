#include "GDALRasterPrinter.h"

#include <iostream>

namespace io {

    namespace GDAL {

        dataset_ptr create_data_file_(
            const boost::filesystem::path & file,
            const char * gdal_driver,
            const GDALDataType & data_type,
            const geo::RasterArea & area)
        {
            GDALAllRegister();
            GDALDriver * poDriver = GetGDALDriverManager()->GetDriverByName(gdal_driver);
            if (!poDriver) {
                throw std::runtime_error("Failed to create the GDAL driver.");
            }
            dataset_ptr ds;
            ds.reset(
                poDriver->Create(file.string().c_str(),
                    static_cast<int>(area.pixel_width()),
                    static_cast<int>(area.pixel_height()),
                    1,
                    data_type,
                    nullptr));

            double geo_transform[6];
            geo_transform[0] = area.left();
            geo_transform[3] = area.top();
            geo_transform[1] = area.cell_size();
            geo_transform[5] = -area.cell_size();
            geo_transform[2] = 0;
            geo_transform[4] = 0;
            ds->SetGeoTransform(geo_transform);

            ds->SetProjection(area.CRS().string().c_str());
            return ds;
        }

    }

}
