# Scientific Visualization 2018-2019

This repository contains the code for Scientific Visualization 2018-2019 as implemented by Kevin Gevers and Martijn Luinstra. 

The folder `Code` contains the project code in two subfolders. `Code\Smoke` contains the application code and `Code\FFTW` contains the FFTW (version 3) library compiled for windows.

The folder `Images` contains the original images that have been included in the report.

The file `report.pdf` is a digital version of the report as submitted on 11 April 2019.

## How to run
Open the file `Code\Smoke\Smoke.pro` in Qt Creator and build from there. On Windows systems, a compiled version of the FFTW (version 3) library is expected in the folder `Code\FFTW`. On other systems, the development version of the FFTW (version 3) library is expected to be installed system wide. On Debian based systems, this can be achieved by installing the `libfftw3-dev` package.

