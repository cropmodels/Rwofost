\name{Rwofost-package}

\alias{Rwofost-package}

\alias{Rwofost}

\docType{package}

\title{
\packageTitle{Rwofost}
}

\description{
This package provides a R interface to the WOFOST crop growth simulation model.

This is the first release. Please consider this version unstable. It needs more work to simplify its use.  

More detailed documentation is also forthcoming, but there is ample general documentation available on-line. The documentation for the FORTRAN version 7.1 is most applicable. For example, this manual \url{https://www.wur.nl/en/show/WOFOST-7.1-User-Manual.htm}

The WOFOST model that this R package uses is written in C++ and it can also be compiled and run as a stand-alone program (see the github repository at \url{https://github.com/cropmodels/Rwofost}. It was derived from the original FORTRAN implementation. It passes the tests cases developed for the PCSE/python version. This suggests that you safely can use the model for "standard" computation of potential and water-limited production. 
}

