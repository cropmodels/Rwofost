
make_matrices <- function(x) {
	if (length(x) > 1) { 
		x <- matrix(x, nrow=2) 
		# remove trailing zeros
		d <- diff(x[1,])
		i <- which(d < 0)
		if (length(i > 0)) {
			if (x[1,i+1] == 0) {
				x <- x[, 1:i]
			}
		}
	} 
	x
}


yamltest <- function(yf) {

	ttype <- strsplit(basename(yf), "_")[[1]][2]
	y <- yaml::read_yaml(yf)
	x <- lapply(y$WeatherVariables, unlist)
	x <- data.frame(t(do.call(cbind, x)), stringsAsFactors=FALSE)
	w <- data.frame(
				date=as.Date(x$DAY), 
				srad=as.numeric(x$IRRAD) / 1000,
				prec=as.numeric(x$RAIN) * 10, #cm to mm
				tmin=as.numeric(x$TMIN),
				tmax=as.numeric(x$TMAX),
				vapr=as.numeric(x$VAP) / 10, #hPa to kPa
				wind=as.numeric(x$WIND)
			)
	
	tim <- wofost_control()
	tim$latitude <- as.numeric(x$LAT[1])
	tim$elevation=10
	tim$CO2=360
	tim$output <- "TEST"

	prec <- y$Precision
	dsoil <- wofost_soil("soil_5")
	dcrop <- wofost_crop('maize_1')
	dcrop <- wofost_crop('barley')
	#dcrop <- wofost_crop('sugarbeet_601')

	if (isTRUE(grep("dynamics", ttype))) {
		yy <- yf
		yy <- yaml::read_yaml(gsub("rootdynamics", "potentialproduction", yy))
		yy <- yaml::read_yaml(gsub("leafdynamics", "potentialproduction", yy))
		dcrop$FRTB <- Rwofost:::.make_matrices(yy$ModelParameters$FRTB)
	} 

	if (ttype == "waterlimitedproduction") {
		tim$water_limited <- TRUE
	} else {
		tim$water_limited <- FALSE
	} 
	skip <- FALSE

	if (ttype == "astro") {
		pcrop <- dcrop
		psoil <- dsoil
		tim$IDURMX <- 10
		tim$cropstart <- nrow(w) - 10
		tim$modelstart <- as.integer(w$date[1])
	
	} else  {
		
		m <- y$AgroManagement
		cal <- m[[1]][[1]]$CropCalendar
		tim$modelstart <- as.Date(names(m[[1]]))
		tim$cropstart <- as.integer(tim$modelstart - as.Date(cal$crop_start_date))
		tim$IPRODL <- 1
		tim$IDURMX <- cal$max_duration
		if (cal$crop_start_type == "sowing") tim$ISTCHO = 1
	
		p <- y$ModelParameters
		np <- names(p)
		if ("VERNBASE" %in% np) { 
			return(list(skip=TRUE))
		}
		
		pcrop <- np %in% .crop_pars
		psoil <- np %in% .soil_pars
		pother <- p[!(pcrop | psoil)]
		pcrop <- p[pcrop]
		psoil <- p[psoil]

		pcrop$CO2AMAXTB = c(40., 0.00, 360., 1.00, 720., 1.35, 1000., 1.50, 2000., 1.50)
		pcrop$CO2EFFTB = c(40., 0.00, 360., 1.00, 720., 1.11, 1000., 1.11, 2000., 1.11)
		pcrop$CO2TRATB = c(40., 0.00, 360., 1.00, 720., 0.9, 1000., 0.9, 2000., 0.9)
		pcrop <- lapply(pcrop, Rwofost:::.make_matrices)

	   # parameters missing: 
		nms <- names(dsoil)
		nms <- nms[!(nms %in% names(pcrop))]
		psoil <- c(psoil, dsoil[nms])
		#s <- s[c("SMTAB", "CONTAB", "SPADS", "SPASS", "SPODS", "SPOSS", "DEFLIM", "IZT", "ZTI", "DD", "IDRAIN")]
		#psoil <- c(psoil, s)

		nms <- names(dcrop)
		nms <- nms[!(nms %in% names(pcrop))]
		pcrop <- c(pcrop, dcrop[nms])
	}
	
	

	m <- wofost_model(pcrop, w, psoil, tim)

	frc <- y$ExternalStates
	if (!is.null(frc)) {
		f <- lapply(frc, unlist)
		f <- data.frame(t(do.call(cbind, f)), stringsAsFactors=FALSE)
		j <- which(colnames(f) == "DAY")
		f <- f[, c(j, (1:ncol(f))[-j])]	
		f$DAY <- as.Date(f$DAY)
		for (i in 2:ncol(f)) f[,i] <- as.numeric(f[,i])
		force(m) <- f
	}
	
	x <- run(m, FALSE)
		
	r <- y$ModelResults
	r <- lapply(r, unlist)
	r <- data.frame(t(do.call(cbind, r)), stringsAsFactors=FALSE)
	if (colnames(r)[1] == "DAY") {
		r[,1] <- as.Date(r[,1])
		for (i in 2:ncol(r)) r[,i] <- as.numeric(r[,i])
	} else {
		for (i in 1:ncol(r)) r[,i] <- as.numeric(r[,i])	
	}
	
	list(R=x, P=r, prec=prec, skip=skip)
}



getPR <- function(path, test, i) {
    ii <- formatC(i, width=2, flag="0")
    yf <- file.path(path, paste0("test_", test, "_wofost71_", ii, ".yaml")) 
    x <- Rwofost:::.yamltest(yf)
  
    #skipping vernalization cases for now
    if (x$skip) return(NULL)
    p <- x$P
    r <- x$R
    cn <- colnames(r)[colnames(r) %in% colnames(p)]
    r <- r[, c("date", cn)]
    p <- p[, c("DAY", cn)]
    p <- p[1:nrow(r), ]
    x$P <- p
	x
}


complot <- function(x, p, r=p) {
	plot(x$R$date, x$P[[p]], ylim=range(c(x$P[[p]], x$R[[r]])), xlab="", ylab=p, col="blue")
	points(x$R$date, x$R[[r]], col="red", pch=2, cex=.5)
	legend("topleft", c("Py", "R"), pch=c(1, 2), pt.cex=c(1,0.5), col=c("blue", "red"))
}

error <- function(x, p, r=p) {
	rmse <- sqrt(mean((x$P[[p]] - x$R[[r]])^2))
	min(rmse / mean(x$P[[p]]), rmse / median(x$P[[p]]))
}


test_precision <- function(x, p) {
	dif <- abs(x$P[[p]] - x$R[[p]])
	sum( dif > x$prec[[p]]) / nrow(x$P)
}


test <- function(path, group, tests=1:42) {

	if (group == "astro"){
		aprec <- list(ANGOT=1000, ATMTR=0.004, COSLD=0.005, DAYL=0.1, DAYLP=0.1, DIFPP=1.5, DSINBE=250, SINLD=0.0005)
	}
	
	bf <- paste0("test_", group, "_wofost71_")

	first <- TRUE
	j = 1;
	for (i in tests) {
		fname <- paste0(bf, formatC(i, width=2, flag="0"), ".yaml")
		
		yf <- file.path(path, fname)
		x <- .yamltest(yf) 
		if (group == "astro"){
			x$prec <- aprec
		}
		x$prec$TWLV <- 10
		x$prec$LAI <- 0.1

		if (!x$skip) {
			stopifnot(x$P$DAY[1] == x$R$date[1])
			x$P <- x$P[1:nrow(x$R), ]
			x$P <- x$P[, colnames(x$P)[colnames(x$P) %in% colnames(x$R)], drop=FALSE]
			test <- sapply(colnames(x$P), function(v) .test_precision(x, v) )
			if (first) {
				result <- matrix(nrow=length(tests), ncol=length(test))
				first <- FALSE
			}
			result[j,] <- test
			if (max(test > 0) > 0) cat(paste0(i, "-")) else cat(paste0(i, "+"))
		} else {
			cat(paste0(i, "x"))
		}
		flush.console(); if (i ==24) cat("\n")
		j <- j + 1;
	}
	cat("\n")
	if (max(result, na.rm=TRUE) > 0) {
		cat(" errors detected\n")
	} else {
		cat(paste(" OK\n"))
	}
	cn <- colnames(x$P)[colnames(x$P) %in% colnames(x$R)]
	colnames(result) <- cn
	result
}


## location of the wofost yaml test files.
#ydir <- "C:/github/cropmodels/Rwofost_test/test_data/"

#library(Rwofost)
#xs <- Rwofost:::.test(ydir, "astro")
#xy <- Rwofost:::.test(ydir, "phenology")
#xp <- Rwofost:::.test(ydir, "partitioning")
#xa <- Rwofost:::.test(ydir, "assimilation")
#xr <- Rwofost:::.test(ydir, "rootdynamics")
#xp <- Rwofost:::.test(ydir, "respiration")
#xl <- Rwofost:::.test(ydir, "leafdynamics")
#xp <- Rwofost:::.test(ydir, "potentialproduction")
#xw <- Rwofost:::.test(ydir, "waterlimitedproduction")

