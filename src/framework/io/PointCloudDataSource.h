#ifndef POINT_CLOUD_H_
#define POINT_CLOUD_H_

#include <map>
#include <memory>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#include "framework/ReferenceSystem.h"
#include "framework/RasterArea.h"
#include "framework/coordinates.h"
#include "framework/io/Interpolator.h"
#include "framework/utils/string_utils.h"

class LASpoint;

namespace io {

    namespace point_cloud {

        class PointCloudDataSource;

        std::unique_ptr<PointCloudDataSource> create_data_source(
            const std::string & s);

        enum class PointFilterType {KEEP_WINDOW, KEEP_CLASSES};

        using FilterParams = std::pair<PointFilterType, std::vector<std::string>>;

        size_t read_data(
            const std::string &filename,
            Interpolator &ip,
            const std::vector<FilterParams> &filter_params);

        size_t read_data_laz(
            const std::string &filename,
            Interpolator &ip,
            const std::vector<FilterParams> &filter_params);


        class PointCloudDataSource
        {
            public:
                PointCloudDataSource();
                ~PointCloudDataSource();

                void add_file(const boost::filesystem::path &);

                void add_filter(
                    const std::string &filter_name,
                    const std::string &filter_str);

                geo::ReferenceSystem CRS() const;
                bool has_no_data_value() const;
                double no_data_value() const;
                std::string no_data_value_str() const;
                bool has_data_inside_area(const geo::Area &) const;
                geo::Area area() const;

                std::vector<FilterParams> filter_params() const;
                std::vector<boost::filesystem::path> filenames() const;

            private:
                std::vector<boost::filesystem::path> filenames_;
                std::vector<FilterParams> filter_params_;
        };

        template<typename R>
        bool fill_array(
            R & raster,
            const PointCloudDataSource & src);
        template<typename R>
        bool fill_array(
            R & raster,
            const PointCloudDataSource & src)
        {
            std::vector<FilterParams> local_filter_params;
            for (const auto &f: src.filter_params())
            {
                if (f.first == PointFilterType::KEEP_WINDOW) {
                    local_filter_params.push_back(
                        {f.first, f.second});
                } else {
                    local_filter_params.push_back(f);
                }
            }
            Interpolator ip;
            for (const auto &f: src.filenames()) {
                std::cout << "Importing points from the file '"
                    << f.string() << "'" << std::endl;
                size_t n {read_data(f.string(), ip, local_filter_params)};
                if (n == static_cast<size_t>(0)) {
                    std::cout <<"  No matching points." << std::endl;
                }
            }
            std::cout << "Created a TIN interpolator from "
                << ip.number_of_points() << " points." << std::endl;
            std::cout << "Starting to interpolate to "
                << raster.pixel_width() << " x " << raster.pixel_height()
                << " grid." << std::endl;
            unsigned int ny {raster.pixel_height()};
            unsigned int prog {0};
            for (unsigned int row = 0; row < ny; ++row) {
                ip.fill_array(raster.to_geocoordinate(coordinates::RasterCoordinate {0, 0}),
                    raster.area().cell_size(),
                    raster.data(),
                    raster.pixel_width(),
                    row, row + 1,
                    row > 0);
                if ((row * 10)/ ny > prog)
                    std::cout << (++prog * 10) << " %" << std::endl;
            }
            std::cout << "100 %" << std::endl;
            return true;
        }
        double get_x(const LASpoint &p);
        double get_y(const LASpoint &p);
        int get_class(const LASpoint &p);

        template<typename T>
        class PointFilter
        {
            public:
                virtual ~PointFilter() {}
                virtual bool operator()(const T &point) = 0;

                virtual std::string str() const = 0;
        };

        template<typename T>
        class PointFilterKeepWindow: public PointFilter<T>
        {
            public:
                PointFilterKeepWindow(
                    double xmin, double xmax, double ymin, double ymax):
                        PointFilter<T> {},
                        xmin_ {xmin}, xmax_ {xmax}, ymin_ {ymin}, ymax_ {ymax}
                {
                }

                PointFilterKeepWindow(const std::vector<std::string> &params):
                    PointFilter<T> {}
                {
                    if (params.size() != 4) {
                        throw std::runtime_error("Needed 4 params for the PointFilterKeepWindow constructor.");
                    }
                    try {
                        xmin_ = boost::lexical_cast<double>(params[0]);
                        ymax_ = boost::lexical_cast<double>(params[1]);
                        xmax_ = xmin_ + boost::lexical_cast<double>(params[2]);
                        ymin_ = ymax_ - boost::lexical_cast<double>(params[3]);
                    } catch (boost::bad_lexical_cast & /*e*/) {
                        throw std::runtime_error(
                            "Invalid values passed to PointFilterKeepWindow constructor.");
                    }
                }

                bool operator()(const T &point) override
                {
                    double x {get_x(point)};
                    double y {get_y(point)};
                    return !(x < xmin_ || x > xmax_ || y < ymin_ || y > ymax_);
                }

                std::string str() const override
                {
                    std::stringstream ss;
                    ss << "PointFilterKeepWindow ["
                        << std::fixed << std::setprecision(6)
                        << xmin_ << ":" << xmax_
                        << "] x [" << ymin_ << ":" << ymax_ << "]";
                    return ss.str();
                }

                double left() const { return xmin_; }
                double right() const { return xmax_; }
                double top() const { return ymax_; }
                double bottom() const { return ymin_; }

            private:
                double xmin_, xmax_, ymin_, ymax_;
        };

        template<typename T>
        class PointFilterKeepClasses: public PointFilter<T>
        {
            public:
                PointFilterKeepClasses(const std::vector<int> &cls):
                    PointFilter<T> {},
                    classes_ {cls.begin(), cls.end()}
                {
                }

                PointFilterKeepClasses(const std::vector<std::string> &params):
                    PointFilter<T> {}
                {
                    for (const auto &s: params)
                    {
                        classes_.insert(boost::lexical_cast<int>(s));
                    }
                }

                bool operator()(const T &point) override
                {
                    int c {get_class(point)};
                    return classes_.find(c) != classes_.end();
                }

                std::string str() const override
                {
                    std::stringstream ss;
                    ss << "PointFilterKeepClasses(" <<
                        utils::separate_by_commas(classes_) << ")";
                    return ss.str();
                }

            private:
                std::set<int> classes_;
        };

        template<typename T>
        std::unique_ptr<PointFilter<T>> promote_filter(const FilterParams &p)
        {
            switch (p.first) {
                case PointFilterType::KEEP_WINDOW:
                    return std::move(std::unique_ptr<PointFilter<T>>(
                        new PointFilterKeepWindow<T>(p.second)));
                case PointFilterType::KEEP_CLASSES:
                    return std::move(std::unique_ptr<PointFilter<T>>(
                        new PointFilterKeepClasses<T>(p.second)));
                default:
                    // FIXME better message
                    throw std::runtime_error("promote_filter not implemented for the given PointFilterType.");
            }
        }

    }

}

#endif
