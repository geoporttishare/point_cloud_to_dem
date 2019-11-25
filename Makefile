.PHONY: all clean

LASTOOLS_DIR := ${HOME}/codes/LAStools.git
INCL := -Isrc -I. -isystem${LASTOOLS_DIR}/LASlib/inc -isystem${LASTOOLS_DIR}/LASzip/src -isystem/usr/include/gdal
LDFLAGS := -L${LASTOOLS_DIR}/LASlib/lib
CPPFLAGS := -O2 -DNDEBUG -std=c++14 -Wall -Wextra
LIBS := -lCGAL -lgmp -lmpfr -lgdal -lboost_filesystem -lboost_regex -lboost_program_options -lboost_system -llas

sources := $(shell find src -type f -name "*.cpp")
objects := $(patsubst %.cpp,%.o,$(sources))

all: $(objects) point_cloud_to_raster.bin

%.o: %.cpp
	g++ $(CPPFLAGS) $(INCL) -o $@ -c $<

point_cloud_to_raster.bin: $(objects)
	g++ -o $@ $(objects) $(LDFLAGS) $(LIBS)

clean:
	$(shell find src -type f -name "*.o" -delete)
	rm -f point_cloud_to_raster.bin
