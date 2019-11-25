#ifndef STRING_UTILS_H_
#define STRING_UTILS_H_

#include <vector>
#include <string>
#include <sstream>

namespace utils {

    std::vector<std::string> split(const std::string &, char);

    std::vector<double> string_to_doubles(const std::string &s);
    std::vector<unsigned int> string_to_uints(const std::string &s);

    template<typename C>
    std::string separate_by_commas(const C &c);










    template<typename C>
    std::string separate_by_commas(const C &c)
    {
        if (c.size() == 0) return "";
        std::stringstream ss;
        auto it = c.begin();
        ss << *it;
        //if (c.size() == 1) return ss.str();
        while (++it != c.end())
        {
            ss << "," << *it;
        }
        return ss.str();
    }

}

#endif
