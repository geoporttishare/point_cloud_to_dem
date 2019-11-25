#include "ProgressIndicator.h"

#include <sstream>

namespace utils {

    ProgressIndicator::ProgressIndicator(
            long long npoints_,
            unsigned short length_):
        npoints {npoints_}, counter {0},
        block_length {npoints_ / length_},
        length {length_},
        bar_pos {0}
    {
        bar_hits = std::vector<bool>(length, false);
        if (block_length * length < npoints) ++block_length;
    }

    ProgressIndicator::~ProgressIndicator()
    {
    }

    std::string ProgressIndicator::bar_str() const
    {
        std::stringstream ss;
        for (unsigned short i = 0; i < bar_pos; ++i)
        {
            if (bar_hits[i])
                ss << "x";
            else
                ss << ".";
        }
        for (unsigned short i = bar_pos; i < length; ++i)
            ss << "_";
        return ss.str();
    }

    bool ProgressIndicator::inc() {
        ++counter;
        if (counter == npoints) {
            bar_pos = length;
            return true;
        }
        if ((counter / block_length) != bar_pos) {
            ++bar_pos;
            return true;
        } else {
            return false;
        }
    }

    void ProgressIndicator::mark()
    {
        bar_hits[bar_pos] = true;
    }

    unsigned short ProgressIndicator::progress() const
    {
        return static_cast<unsigned short>(counter * 100 / npoints);
    }

}
