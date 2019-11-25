#ifndef GDAL_DATASET_PTR_H_
#define GDAL_DATASET_PTR_H_

#include <gdal_priv.h>

namespace io {

    namespace GDAL {

        class dataset_ptr {
            public:
                dataset_ptr();
                ~dataset_ptr();

                dataset_ptr(const dataset_ptr &) = delete;
                dataset_ptr(dataset_ptr &&);

                dataset_ptr & operator=(dataset_ptr &&);
                dataset_ptr & operator=(const dataset_ptr &) = delete;

                void reset(GDALDataset *);
                GDALDataset * get();
                GDALDataset * operator->();

            private:
                GDALDataset * ds_;
        };

    }

}

#endif
