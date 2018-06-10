\name{readWofostOutput}

\alias{readWofostOutput}

\title{
Read WOFOST output
}

\description{
Read output from the stand-alone (FORTAN) WOFOST crop growth model
Implemented here to allow easy comparison of model output with output of the R implementation of the model.
}

\usage{
readWofostOutput(f, wlim=FALSE)
}

\arguments{
\item{f}{filename}
\item{wlim}{logical. If \code{TRUE}, the results for water limited production are returned}
}

\value{
matrix
}


\examples{
f <- system.file("test/1/wofost.out", package="Rwofost")
d <- readWofostOutput(f)
}
