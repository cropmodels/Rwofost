\name{wofost_soil}

\alias{wofost_soil}

\title{
WOFOST soil parameters
}

\description{
This functions returns a list with parameters describing a soil for the WOFOST model
}

\usage{
wofost_soil(name='')
}

\arguments{
\item{name}{character. Either the name of a soil that comes with the package (see examples) or a filename of a similarly formatted file}
}

\value{ list }


\seealso{ \code{\link{wofost}} }

\examples{
wofost_soil('')
soil <- wofost_soil('ec1')
str(soil)
}
