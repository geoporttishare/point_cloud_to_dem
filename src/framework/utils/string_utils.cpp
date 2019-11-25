#include "string_utils.h"

#include <sstream>

#include <boost/lexical_cast.hpp>

namespace utils {

    std::vector<std::string> split(const std::string &s, char sep)
    {
        std::istringstream is(s);
        std::string tmp;
        std::vector<std::string> values;
        while (std::getline(is, tmp, sep)) {
            values.push_back(tmp);
        }
        return values;
    }

    std::vector<double> string_to_doubles(const std::string &s)
    {
        std::istringstream is(s);
        std::string tmp;
        std::vector<double> values;
        while (std::getline(is, tmp, ',')) {
            values.push_back(boost::lexical_cast<double>(tmp));
        }
        return values;
    }

    std::vector<unsigned int> string_to_uints(const std::string &s)
    {
        std::istringstream is(s);
        std::string tmp;
        std::vector<unsigned int> values;
        while (std::getline(is, tmp, ',')) {
            values.push_back(boost::lexical_cast<unsigned int>(tmp));
        }
        return values;
    }

}
