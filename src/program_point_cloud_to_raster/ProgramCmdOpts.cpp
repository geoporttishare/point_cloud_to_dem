#include "ProgramCmdOpts.h"

#include "framework/geo.h"
#include "framework/utils/string_utils.h"


ProgramCmdOpts::ProgramCmdOpts():
    desc_("Allowed options")
{
    namespace po = boost::program_options;

    desc_.add_options()
        ("help,h", "Produce help message")
        ("pointcloud",
            po::value<std::string>(&point_cloud_data_str_)->required(),
            "Specify a string identifying the point cloud files")
        ("output-file,o",
            po::value<std::string>(&output_file_str_)->required(),
            "Specify the output file.")
        ("calc-win",
                po::value<std::string>(&calc_window_str_)->required(),
                "The calculation areas in georeferenced coordinates\n"
                "ulx,uly,width,height\n")
        ("classes",
                po::value<std::string>(&classes_str_)->required(),
                "Which classes to include in the final point cloud data.")
        ("include_points_buffer",
                po::value<double>(&include_points_buffer_)->required(),
                "Buffer around the calculation window from where\n"
                "the points are still included into the triangulation\n"
                "and interpolation.\n")
        ("resolution",
                po::value<double>(&resolution_)->required(),
                "The resolution of the raster file.")
        ("refsys",
                po::value<std::string>(&ref_sys_string_)->required(),
                "The reference system string. Options are:\n"
                "  - full WKT string (inside quotes)\n"
                "  - EPSG code in format EPSG:<4-digit value>.")
        ;
}

bool ProgramCmdOpts::parse(int argc, char** argv)
{
    namespace po = boost::program_options;

    po::store(po::parse_command_line(argc, argv, desc_), vm_);

    if (vm_.count("help")) {
        std::cout << desc_ << std::endl;
        return false;
    }
    po::notify(vm_);

    output_file_ = boost::filesystem::path {output_file_str_};

    calc_window_ = geo::parse_rectangle_coordinates(
        calc_window_str_, ref_sys_string_);

    if (! vm_.count("output-format")) {
        output_format_ = "gtiff";
    }
    return true;
}

std::string ProgramCmdOpts::output_path() const
{
    if (output_file_str_.size() == 0)
        throw std::runtime_error("No output file given.");
    return output_file_.parent_path().string();
}

std::string ProgramCmdOpts::output_name() const
{
    if (output_file_str_.size() == 0)
        throw std::runtime_error("No output file given.");
    return output_file_.filename().string();
}

std::string ProgramCmdOpts::output_format() const
{
    return output_format_;
}

geo::Area ProgramCmdOpts::calculation_area() const
{
    return calc_window_;
}

double ProgramCmdOpts::resolution() const
{
    if (resolution_ < 0)
        throw std::runtime_error("Invalid resolution given.");
    return resolution_;
}

std::vector<unsigned int> ProgramCmdOpts::classes() const
{
    return utils::string_to_uints(classes_str_);
}

std::string ProgramCmdOpts::classes_str() const
{
    auto cls = classes();
    if (cls.size() > 0)
    {
        return classes_str_;
    } else {
        return "";
    }
}
