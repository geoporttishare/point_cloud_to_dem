#include "program.h"
#include "ProgramCmdOpts.h"
#include "framework/Raster.h"
#include "framework/io/GDALRasterPrinter.h"
#include "framework/io/PointCloudDataSource.h"

int program(
    const ProgramCmdOpts & opts)
{
    using DemDataType = float;
    using DemClass = Raster<DemDataType>;

    // Create a data source from the given files.
    auto data_src = io::point_cloud::create_data_source(
        opts.point_cloud_data_str());

    {
        // Create the filter window.
        std::stringstream ss;
        auto w = opts.calculation_area();
        w.add_halo(opts.include_points_buffer());
        ss << w.left() << "," << w.top() << "," << (w.right() - w.left())
            << "," << (w.top() - w.bottom());
        data_src->add_filter("keep_window", ss.str());
    }
    {
        // Create the class filter.
        std::string keep_classes {opts.classes_str()};
        if (keep_classes.size() > 0) {
            data_src->add_filter("keep_classes", keep_classes);
        }
    }

    geo::RasterArea calc_area {
        opts.calculation_area(), opts.resolution() };

    // Create the raster for the DEM, set the NODATA value, and format the
    // array with that value.
    DemClass new_dem { calc_area, "DEM" };
    new_dem.no_data_value(9999);
    new_dem.format();

    // Read points from the point cloud files, generate TIN from the points, and
    // interpolate the TIN on the raster cells.
    io::point_cloud::fill_array(new_dem, *data_src);

    // Write the resulting raster to a file.
    io::GDAL::write(
        new_dem,
        boost::filesystem::path(opts.output_path()) / boost::filesystem::path(opts.output_name()),
        opts.output_format());

    return 0;
}
