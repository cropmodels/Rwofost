\name{wofost_control}

\alias{wofost_control}

\title{
WOFOST control parameters
}

\description{
This functions returns a list of "control" parameters to run the WOFOST model. Either a default list, or from a file.
}

\usage{
wofost_control(filename='')
}

\arguments{
\item{filename}{character. Filename}
}

\value{
list
}

\seealso{ \code{\link{wofost}} }

\examples{
contr <- wofost_control()
str(contr)
}
