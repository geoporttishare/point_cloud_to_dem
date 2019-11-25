#ifndef PROGRESSINDICATOR_H_
#define PROGRESSINDICATOR_H_

#include <vector>
#include <string>

namespace utils {

    class ProgressIndicator {
        public:
            long long npoints;
            long long counter;
            long long block_length;
            unsigned short length;
            unsigned short bar_pos;
            std::vector<bool> bar_hits;

            ProgressIndicator(long long npoints_, unsigned short length_);
            ~ProgressIndicator();

            std::string bar_str() const;
            unsigned short progress() const;

            bool inc();
            void mark();
    };

}

#endif
