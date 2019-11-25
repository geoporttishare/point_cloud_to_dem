#ifndef REFERENCESYSTEM_H_
#define REFERENCESYSTEM_H_

#include <string>


namespace geo {

    class ReferenceSystem;

    bool operator==(const ReferenceSystem &a, const ReferenceSystem &b);
    bool operator!=(const ReferenceSystem &a, const ReferenceSystem &b);

    class ReferenceSystem
    {
        public:
            ReferenceSystem();
            ReferenceSystem(const ReferenceSystem &) = default;
            ReferenceSystem(const std::string &);

            ReferenceSystem & operator=(ReferenceSystem);

            friend bool operator==(
                const ReferenceSystem &,
                const ReferenceSystem &);

            std::string string() const;

        private:
            std::string wkt_str_;
    };

}

#endif
