
source("C:/github/cropmodels/Rwofost/tests/pcse_tests.R")
## location of the wofost yaml test files.
ydir <- "C:/github/cropmodels/Rwofost_test/test_data/"

library(Rwofost)
x <- getPR(ydir, "rootdynamics", 1)
par(ask=TRUE) 
for (v in colnames(x$P)[-1]) complot(x, v)

xr <- wtest(ydir, "rootdynamics")
xs <- wtest(ydir, "astro")
xy <- wtest(ydir, "phenology")
xt <- wtest(ydir, "partitioning")
xa <- wtest(ydir, "assimilation")
xe <- wtest(ydir, "respiration")
xl <- wtest(ydir, "leafdynamics")
xp <- wtest(ydir, "potentialproduction")
xw <- wtest(ydir, "waterlimitedproduction")

