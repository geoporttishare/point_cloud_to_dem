#ifndef CMD_OPTS_H_
#define CMD_OPTS_H_

#include <string>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "framework/Area.h"


class ProgramCmdOpts
{
    public:
        ProgramCmdOpts();

        std::string point_cloud_data_str() const {
            return point_cloud_data_str_;
        }
        geo::Area calculation_area() const;

        std::string output_path() const;
        std::string output_name() const;
        std::string output_format() const;

        double resolution() const;
        double include_points_buffer() const {
            return include_points_buffer_;
        }
        const boost::filesystem::path & output_file() const {
            return output_file_;
        }

        std::string classes_str() const;
        std::vector<unsigned int> classes() const;

        bool parse(int argc, char** argv);

    private:
        boost::program_options::variables_map vm_;
        boost::program_options::options_description desc_;
        std::string point_cloud_data_str_;
        std::string output_file_str_;
        std::string calc_window_str_;
        std::string output_format_;
        std::string ref_sys_string_;
        std::string classes_str_;
        boost::filesystem::path output_file_;
        geo::Area calc_window_;
        double resolution_;
        double include_points_buffer_;
};

#endif
