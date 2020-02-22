

readFortranOutput <- function(f, wlim=FALSE) {

	.trim2 <- function(x) return(gsub("^ *|(?<= ) | *$", "", x, perl=TRUE))

	r <- .trim2(readLines(f));
	hdr <- grep("YEAR DAY", r)
	i <- substr(r,1,4) %in% 1970:2016
	x <- r[i]
	x <- strsplit(x, ' ')
	n <- length(x) / 2

	if (!wlim) {
		pot <- t(sapply(x[1:n], rbind))
		pot <- matrix(as.numeric(pot), ncol=ncol(pot))
		colnames(pot) <- unlist(strsplit(r[hdr[1]], ' '))
		return(pot)
	} else {
		wlm <- t(sapply(x[(n+1):length(x)], rbind))
		wlm <- matrix(as.numeric(wlm), ncol=ncol(wlm))
		colnames(wlm) <- unlist(strsplit(r[hdr[2]], ' '))
		return(wlm)
	}
}


## compare with FORTRAN wofost
## 1
library(Rwofost)

crop <- wofost_crop("rapeseed_1001")
f <- system.file("extdata/Netherlands_Swifterbant.csv", package="meteor")
wth_n <- read.csv(f)
wth_n$date <- as.Date(wth_n$date)

cont <- wofost_control()
cont$IPRODL <- 0
soil <- wofost_soil("soil_5")
#wth_n <- wth_n[wth_n$date >= '1977-01-01', ]
cont$modelstart <- as.Date('1977-01-01')
cont$latitude <- 52.57
cont$elevation=50
cont$CO2=360

rp <- wofost(crop, wth_n, soil, cont)

f <- system.file("test/1/wofost.out", package="Rwofost")
d <- readFortranOutput(f)

par(mfrow=c(1,2))
plot(d[,'DAY'], d[,'LAI'], type='l')
points(rp[,'step'], rp[, 'LAI'])

# yield
plot(d[,'DAY'], d[,'WSO'], type='l')
points(rp[, 1], rp[,'WSO'])


## 2
# no photoperiod effect
crop$IDSL <- 0
rp <- wofost(crop, wth_n, soil, cont)
f <- system.file("test/2/wofost.out", package="Rwofost")
d <- readFortranOutput(f)
plot(d[,'DAY'], d[,'LAI'], type='l')
points(rp[,'step'], rp[, 'LAI'])



## 3 water limited
crop <- wofost_crop("maize_1")
f <- system.file("extdata/Philippines_IRRI.csv", package="meteor")
wth_n <- read.csv(f)
wth_n$date <- as.Date(wth_n$date)
cont <- wofost_control()
soil <- wofost_soil("soil_5")
cont$modelstart <- as.Date('1985-01-01')
cont$IPRODL <- 1
cont$elevation <- 21
cont$latitude <- 14.18
cont$ANGSTA <- -0.25
cont$ANGSTB <- -0.45
cont$usePENMAN <- TRUE

rp <- wofost(crop, wth_n, soil, cont)

f <- system.file("test/3/wofost.out", package="Rwofost")
d <- readFortranOutput(f, TRUE)
plot(d[,'DAY'], d[,'LAI'], type='l')
points(rp[,'step'], rp[, 'LAI'])


## 4 water limited with groundwater
soil <- wofost_soil("ec1")
cont$IZT <- 1
rp <- wofost(crop, wth_n, soil, cont)

f <- system.file("test/4/wofost.out", package="Rwofost")
d <- readFortranOutput(f, TRUE)
plot(d[,'DAY'], d[,'LAI'], type='l')
points(rp[,'step'], rp[, 'LAI'])



## 5 npk mode
f <- system.file("test/5/wofost_npk_reference_results.csv", package="Rwofost")
d <- read.csv(f)
soil <- wofost_soil("ec4")
crop <- wofost_crop("winterwheat_102")
cont <- wofost_control()
cont$IPRODL <- 0
f <- system.file("extdata/Netherlands_Wageningen.csv", package="meteor")
wth <- read.csv(f)
wth$date <- as.Date(wth$date)
wth <- wth[wth$date >= "1976-01-01", ]
cont$modelstart <- as.Date("1976-01-01")

cont$npk_model <- 1
cont$latitude=51.58
cont$elevation=50
cont$CO2=360

rp <- wofost(crop, wth, soil, cont)

plot(c(1:nrow(d)), d[, 'LAI'], type = "l")
points(rp[, 'step'], rp[, 'LAI'])

