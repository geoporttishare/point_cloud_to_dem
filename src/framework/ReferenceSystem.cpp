#include "ReferenceSystem.h"

#include <sstream>
#include <algorithm>

#include <boost/regex.hpp>
#include <gdal_priv.h>
#include <ogrsf_frmts.h>

namespace {

    class OGR_char_pointer {
        public:
        OGR_char_pointer(): ptr {nullptr} {
        }
        ~OGR_char_pointer() {
            if (ptr) {
                CPLFree(ptr);
                ptr = nullptr;
            }
        }
        char * ptr;
    };
}

namespace geo {

    ReferenceSystem::ReferenceSystem():
        wkt_str_ {"not_defined"}
    {
    }

    ReferenceSystem::ReferenceSystem(const std::string &s):
        wkt_str_ {s}
    {
        boost::regex expr {"(epsg|EPSG):([0-9]{4})"};
        boost::match_results<std::string::const_iterator> match;
        if (boost::regex_match(s, match, expr)) {
            OGRSpatialReference ref {};
            int epsg_value;
            std::stringstream ss;
            ss << match[2];
            ss >> epsg_value;
            ref.importFromEPSG(epsg_value);
            OGR_char_pointer ocp;
            ref.exportToWkt(&(ocp.ptr));
            wkt_str_ = std::string(ocp.ptr);
        } else {
            OGRSpatialReference ref {s.c_str()};
            if (ref.Validate() != OGRERR_NONE)
                wkt_str_ = "not_defined";
        }
    }

    ReferenceSystem & ReferenceSystem::operator=(ReferenceSystem r)
    {
        std::swap(wkt_str_, r.wkt_str_);
        return *this;
    }

    bool operator==(const ReferenceSystem &a, const ReferenceSystem &b)
    {
        return a.wkt_str_ == b.wkt_str_;
    }

    bool operator!=(const ReferenceSystem &a, const ReferenceSystem &b)
    {
        return !(a == b);
    }

    std::string ReferenceSystem::string() const
    {
        return wkt_str_;
    }

}
