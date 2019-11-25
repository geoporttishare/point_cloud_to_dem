#include "PointCloudDataSource.h"

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <memory>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <lasreader.hpp>
#include <lasfilter.hpp>


#include "framework/ReferenceSystem.h"
#include "framework/Area.h"
#include "framework/utils/ProgressIndicator.h"
#include "BoundingBox.h"

namespace io {

    namespace point_cloud {

        void add_data_source(
            PointCloudDataSource & data_source,
            const std::string &);

        void add_data_source_from_file(
            PointCloudDataSource & data_source,
            const boost::filesystem::path & file);

        void add_data_source_filter(
            PointCloudDataSource & data_source,
            const boost::filesystem::path & parent_path,
            const boost::filesystem::path & filename,
            const boost::filesystem::path & extension);

        void add_data_source(
            PointCloudDataSource & data_source,
            const std::string & s)
        {
            boost::filesystem::path path {s};
            if (s.find("*") != std::string::npos) {
                add_data_source_filter(data_source,
                    path.parent_path(), path.filename(), path.extension());
            } else if (boost::filesystem::is_regular_file(path)) {
                add_data_source_from_file(data_source, path);
            } else if (boost::filesystem::is_directory(path)) {
                add_data_source_filter(data_source,
                    path, ".*", ".*");
            } else {
                add_data_source_filter(data_source,
                    path.parent_path(), path.filename(), path.extension());
            }
        }

        std::unique_ptr<PointCloudDataSource> create_data_source(
            const std::string & s)
        {
            boost::regex exp1 {"([^\\.])(\\*)"};
            std::ostringstream t(std::ios::out | std::ios::binary);
            std::ostream_iterator<char, char> oi(t);
            boost::regex_replace(oi, s.begin(), s.end(), exp1, "\\1.\\*",
                boost::match_default | boost::format_all);
            std::string s2 {t.str()};
            auto data_source = std::make_unique<PointCloudDataSource>();
            if (s2.find(",") != std::string::npos) {
                std::vector<boost::filesystem::path> paths;
                for (const auto & si: utils::split(s2, ',')) {
                    add_data_source(*data_source, si);
                }
            } else {
                add_data_source(*data_source, s2);
            }
            return data_source;
        }

        void add_data_source_from_file(
            PointCloudDataSource & data_source,
            const boost::filesystem::path & file)
        {
            data_source.add_file(file);
        }

        void add_data_source_filter(
            PointCloudDataSource & data_source,
            const boost::filesystem::path & parent_path,
            const boost::filesystem::path & filename,
            const boost::filesystem::path & extension)
        {
            boost::regex filename_expr {filename.string()};
            boost::regex extension_expr {extension.string()};
            if (boost::equals(filename.string(), "")) {
                filename_expr = boost::regex {".*"};
            }
            if (boost::equals(extension.string(), "")) {
                extension_expr = boost::regex {".*"};
            }
            using DI = boost::filesystem::directory_iterator;
            DI end_it;
            for (DI it(parent_path); it != end_it; ++it)
            {
                if (boost::filesystem::is_regular_file(*it)) {
                    boost::match_results<std::string::const_iterator> mfilename;
                    boost::match_results<std::string::const_iterator> mextension;
                    if (boost::regex_match(it->path().filename().string(), mfilename, filename_expr) &&
                        boost::regex_match(it->path().extension().string(), mextension, extension_expr))
                    {
                        data_source.add_file(*it);
                    }
                }
            }
        }

        double get_x(const LASpoint &p)
        {
            return p.get_x();
        }

        double get_y(const LASpoint &p)
        {
            return p.get_y();
        }

        int get_class(const LASpoint &p)
        {
            return p.classification;
        }

        size_t read_data(
            const std::string &filename,
            Interpolator &ip,
            const std::vector<FilterParams> &filter_params)
        {
            if (boost::algorithm::ends_with(filename, ".laz")) {
                return read_data_laz(filename, ip, filter_params);
            } else {
                throw std::runtime_error("Unknown point cloud format.");
            }
        }

        size_t read_data_laz(
            const std::string &filename,
            Interpolator &ip,
            const std::vector<FilterParams> &filter_params)
        {
            size_t n_added {0};
            LASreadOpener lro;
            lro.set_file_name(filename.c_str());
            std::unique_ptr<LASreader> reader {lro.open()};
            geo::BoundingBox bb;
            bb.add({reader->get_min_x(), reader->get_min_y()});
            bb.add({reader->get_max_x(), reader->get_max_y()});

            std::vector<std::unique_ptr<PointFilter<LASpoint>>> filters;
            for (const auto &par: filter_params)
            {
                std::unique_ptr<PointFilter<LASpoint>> filter =
                    promote_filter<LASpoint>(par);
                if (filter) {
                    if (par.first == PointFilterType::KEEP_WINDOW)
                    {
                        PointFilterKeepWindow<LASpoint> &f =
                            dynamic_cast<PointFilterKeepWindow<LASpoint>&>(*filter);
                        if (!bb.overlaps_with(f)) return 0;
                    }
                    filters.push_back(std::move(filter));
                }
            }

            utils::ProgressIndicator indic {reader->npoints, 10};
            while (reader->read_point())
            {
                LASpoint &point = reader->point;
                geo::GeoCoordinate gp {point.get_x(), point.get_y()};
                bool print_progress {indic.inc()};
                bool add {true};
                for (const auto &f: filters)
                {
                    if (!(*f)(point)) {
                        add = false;
                        break;
                    }
                }
                if (add)
                {
                    ip.insert_point(gp, point.get_z());
                    ++n_added;
                    indic.mark();
                }
                if (print_progress) {
                    std::stringstream ss;
                    ss << indic.progress() << " %";
                    std::cout << ss.str() << std::endl;
                }
                // For animated progress bar
                //if (print_progress) {
                //    std::cout << '\r' << indic.bar_str();
                //    std::cout.flush();
                //}
            }
            std::cout << "Added " << n_added << " points to the TIN."
                << std::endl;
            return n_added;
        }

        PointCloudDataSource::PointCloudDataSource()
        {
        }

        PointCloudDataSource::~PointCloudDataSource()
        {
        }

        void PointCloudDataSource::add_filter(
            const std::string &filter_name,
            const std::string &filter_str)
        {
            std::vector<std::string> sep_values {utils::split(filter_str, ',')};
            if (boost::algorithm::equals(filter_name, "keep_window"))
            {
                filter_params_.push_back(
                    {PointFilterType::KEEP_WINDOW, sep_values});
            } else if (boost::algorithm::equals(filter_name, "keep_classes"))
            {
                filter_params_.push_back(
                    {PointFilterType::KEEP_CLASSES, sep_values});
            } else {
                // FIXME better message
                throw std::runtime_error("Unknown PointFilterType.");
            }
        }

        void PointCloudDataSource::add_file(
            const boost::filesystem::path &f)
        {
            std::cout << "add_point_cloud_file - " << f.string() << std::endl;
            if (boost::filesystem::exists(f))
            {
                if (boost::filesystem::is_regular_file(f) ||
                    boost::filesystem::is_symlink(f)) {
                    if (boost::ends_with(f.string(), ".laz")) {
                        filenames_.push_back(f);
                    } else {
                        std::stringstream ss;
                        ss << "Unknown point cloud file type: " << f;
                        throw std::runtime_error(ss.str());
                    }
                } else if (boost::filesystem::is_directory(f))
                {
                    using DIT = boost::filesystem::directory_iterator;
                    for (DIT it(f); it != DIT(); ++it)
                    {
                        add_file(it->path());
                    }
                }
            }
        }

        std::vector<FilterParams> PointCloudDataSource::filter_params() const
        {
            return filter_params_;
        }

        std::vector<boost::filesystem::path> PointCloudDataSource::filenames() const
        {
            return filenames_;
        }

        geo::ReferenceSystem PointCloudDataSource::CRS() const {
            std::cout << "PointCloudDataSource::CRS() not impl." << std::endl;
            return {};
        }

        bool PointCloudDataSource::has_no_data_value() const
        {
            std::cout << "PointCloudDataSource::has_no_data_value() not impl."
                << std::endl;
            return false;
        }

        double PointCloudDataSource::no_data_value() const
        {
            std::cout << "PointCloudDataSource::no_data_value() not impl."
                << std::endl;
            return 0.0;
        }

        std::string PointCloudDataSource::no_data_value_str() const
        {
            std::cout << "PointCloudDataSource::no_data_value_str() not impl."
                << std::endl;
            return "";
        }

        bool PointCloudDataSource::has_data_inside_area(const geo::Area &) const
        {
            std::cout << "PointCloudDataSource::has_data_inside_area() not impl."
                << std::endl;
            return true;
        }

        geo::Area PointCloudDataSource::area() const
        {
            std::cout << "PointCloudDataSource::area() not impl." << std::endl;
            return {};
        }

    }

}
