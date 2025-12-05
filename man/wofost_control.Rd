\name{wofost_control}

\alias{wofost_control}

\title{
WOFOST control parameters
}

\description{
This functions returns a list of "control" parameters to run the WOFOST model. Either a default list, or from a file. 
See this manual \url{https://swap.wur.nl/Documents/WCC_WOFOST_717_Manual.pdf
} for the interpretation of the parameters.
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
