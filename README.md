<img src="https://github.com/geoportti/Logos/blob/master/geoportti_logo_300px.png">

# Point cloud to DEM

This repository contains a small C++ program that can read LAS/LAZ point clouds
and generate a digital elevation model (DEM) by constructing a triangulated
irregular network (TIN) and interpolating it.

## Dependencies

The program uses a few open source libraries that must be installed before
compiling this program. The main dependencies are

- [Boost](https://www.boost.org)
- [LASlib](https://github.com/LAStools/LAStools/tree/master/LASlib)
- [CGAL](https://www.cgal.org)
- [GDAL](https://gdal.org)

Most libraries are available on many platforms from their package management
system. However, you may as well install all the libraries manually locally. For
example, the LASlib can be compiled locally in the following manner. Here we assume
that the installation location is `${HOME}/codes`:
```
cd ${HOME}
mkdir codes
cd codes
git clone https://github.com/LAStools/LAStools.git LAStools.git
cd LAStools.git
make
```

## Compiling

Let's first fetch the code:
```
cd ${HOME}/codes
git clone https://github.com/geoporttishare/point_cloud_to_dem.git
cd point_cloud_to_dem
```
In the Makefile it is assumed that the LASlib was cloned and compiled at
`${HOME}/codes/LAStools.git`. Depending on your system, other paths may need to
be added to the variables as well.

When the Makefile is fixed, type `make` to compile the program.

## Usage

Let's assume that you have .laz files in directory ${HOME}/lazdata. To generate
a DEM for a given area from the files, give the command
```
./point_cloud_to_raster.bin \
    -o dem.gtiff \
    --resolution 1 \
    --refsys epsg:3067 \
    --pointcloud ${HOME}/lazdata \
    --calc-win 215300,6983800,300,300 \
    --include_points_buffer 10 \
    --classes 2,9,14
```
This will check each file in the lazdata directory and fetch the points that are
inside the calculation window with the added buffer. The buffer is required so
that the TIN will cover the entire calculation window. Only the points that are
classified into the given categories are included. The resulting DEM raster with
requested resolution will be saved into dem.gtiff.

## Usage and Citing
When used, the following citing should be mentioned: "We made use of geospatial
data/instructions/computing resources provided by the Open Geospatial
Information Infrastructure for Research (oGIIR,
urn:nbn:fi:research-infras-2016072513) funded by the Academy of Finland."
