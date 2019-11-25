#include "Area.h"

#include <iomanip>
#include <boost/lexical_cast.hpp>

#include "utils/string_utils.h"


namespace geo {

    Area::Area(
            const GeoCoordinate &topleft,
            const GeoDims &dims,
            const ReferenceSystem &ref_sys):
        top_left_ {topleft},
        dims_ {dims},
        ref_system_ {ref_sys}
    {
    }

    Area::Area():
        Area {GeoCoordinate {0, 0},
                 GeoDims {0, 0},
                 ReferenceSystem()}
    {
    }

    double Area::left() const
    {
        return top_left_.x();
    }

    double Area::right() const
    {
        return left() + dims_.width();
    }

    double Area::top() const
    {
        return top_left_.y();
    }

    double Area::bottom() const
    {
        return top() - dims_.height();
    }

    GeoCoordinate Area::top_left() const
    {
        return top_left_;
    }

    ReferenceSystem Area::CRS() const
    {
        return ref_system_;
    }

    void Area::CRS(const ReferenceSystem &rs)
    {
        ref_system_ = rs;
    }

    void Area::add_halo(double width)
    {
        if (width < 0)
        {
            if ((right() - left()) < fabs(2 * width) ||
                (top() - bottom()) < fabs(2 * width))
            {
                std::stringstream ss;
                ss << "Cannot subtract a halo region of width " << width <<
                    " from an area of dimensions " << (right() - left()) <<
                    " x " << (top() - bottom()) << ".";
                throw std::runtime_error(ss.str());
            }
        }
        top_left_ = geo::GeoCoordinate {
            top_left_.x() - width,
            top_left_.y() + width };
        dims_ = GeoDims {dims_.width() + 2 * width, dims_.height() + 2 * width};
    }

    bool Area::contains_point(const geo::GeoCoordinate &c) const
    {
        if (c.x() < left() || c.x() > right() ||
            c.y() < bottom() || c.y() > top()) return false;
        return true;
    }

    std::vector<std::string> Area::to_vector_str() const
    {
        std::vector<std::string> ret;
        {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(10);
            ss << left();
            ret.push_back(ss.str());
        }
        {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(10);
            ss << top();
            ret.push_back(ss.str());
        }
        {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(10);
            ss << (right() - left());
            ret.push_back(ss.str());
        }
        {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(10);
            ss << (top() - bottom());
            ret.push_back(ss.str());
        }
        return ret;
    }

    Area parse_rectangle_coordinates(
        const std::string &s, const std::string &ref_sys)
    {
        try {
            std::vector<double> values = utils::string_to_doubles(s);
            if (values.size() != 4) {
                throw std::runtime_error("Wrong number of values for "
                    "determining calculation window");
            }
            geo::GeoCoordinate topleft {values[0], values[1]};
            geo::GeoDims dims {values[2], values[3]};
            return geo::Area {topleft, dims, geo::ReferenceSystem(ref_sys)};
        } catch (boost::bad_lexical_cast & /*e*/) {
            std::stringstream ss;
            ss << "Cannot convert calc-win string \"" << s << "\" to doubles.";
            throw std::runtime_error(ss.str());
        } catch (std::exception &e) {
            std::stringstream ss;
            ss << "Cannot convert calc-win string \"" << s << "\" to Area: "
                << e.what();
            throw std::runtime_error(ss.str());
        } catch (...) {
            throw std::runtime_error("Could not parse coordinates.");
        }
    }

    Area parse_rectangle_coordinates(
        const std::vector<std::string> &v, const std::string &ref_sys)
    {
        try {
            if (v.size() != 4) {
                throw std::runtime_error("Wrong number of values for "
                    "determining calculation window");
            }
            std::vector<double> values;
            for (const auto &vi: v)
                values.push_back(boost::lexical_cast<double>(vi));
            geo::GeoCoordinate topleft {values[0], values[1]};
            geo::GeoDims dims {values[2], values[3]};
            return geo::Area {topleft, dims, geo::ReferenceSystem(ref_sys)};
        } catch (boost::bad_lexical_cast & /*e*/) {
            std::stringstream ss;
            ss << "Cannot convert calc-win vector of strings to doubles.";
            throw std::runtime_error(ss.str());
        } catch (std::exception &e) {
            std::stringstream ss;
            ss << "Cannot convert calc-win vector of strings to Area: "
                << e.what();
            throw std::runtime_error(ss.str());
        } catch (...) {
            throw std::runtime_error("Could not parse coordinates.");
        }
    }

    std::ostream & operator<<(std::ostream &os, const Area &g)
    {
        os << "Area:" << std::endl;
        os << "  (width, height) = (" << g.right() - g.left() << "m x " <<
            g.top() - g.bottom() << "m)" << std::endl;
        os << "  (xmin, xmax, ymin, ymax) = (" << std::fixed << g.left() << ", " <<
            g.right() << ", " << g.bottom() << ", " << g.top() << ")\n";
        return os;
    }

}
