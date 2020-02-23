
.make_matrices <- function(x) {
	if (length(x) > 1) { 
		x <- matrix(x, nrow=2) 
		# remove trailing zeros
		d <- diff(x[1,])
		i <- which(d < 0)
		if (length(i > 0)) {
			if (d[i+1] == 0) {
				x <- x[, 1:i]
			}
		}
	} 
	x
}

.yamltest <- function(yf) {

	ttype <- strsplit(basename(yf), "_")[[1]][2]
	y <- yaml::read_yaml(yf)
	x <- lapply(y$WeatherVariables, unlist)
	x <- data.frame(t(do.call(cbind, x)), stringsAsFactors=FALSE)
	w <- data.frame(
				date=as.Date(x$DAY), 
				srad=as.numeric(x$IRRAD) / 1000,
				prec=as.numeric(x$RAIN),
				tmin=as.numeric(x$TMIN),
				tmax=as.numeric(x$TMAX),
				vapr=as.numeric(x$VAP),
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

	if (ttype == "astro") {
		pcrop <- dcrop
		psoil <- dsoil
		tim$IDURMX <- nrow(w)
		tim$cropstart <- nrow(w)-1
	
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
		skip <- FALSE
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
		pcrop <- lapply(pcrop, .make_matrices)

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
	x <- run(m)
		
	r <- y$ModelResults
	r <- lapply(r, unlist)
	r <- data.frame(t(do.call(cbind, r)), stringsAsFactors=FALSE)
	if (colnames(r)[1] == "DAY") {
		r[,1] <- as.Date(r[,1])
		for (i in 2:ncol(r)) r[,i] <- as.numeric(r[,i])
	}
	list(R=x, P=r, prec=prec, skip=skip)
}


.complot <- function(x, p, r=p) {
	plot(x$R$date, x$P[[p]], ylim=range(c(x$P[[p]], x$R[[r]])), xlab="", ylab=p, col="blue")
	points(x$R$date, x$R[[r]], col="red", pch=2, cex=.5)
	legend("topleft", c("Py", "R"), pch=c(1, 2), pt.cex=c(1,0.5), col=c("blue", "red"))
}

.error <- function(x, p, r=p) {
	rmse <- sqrt(mean((x$P[[p]] - x$R[[r]])^2))
	min(rmse / mean(x$P[[p]]), rmse / median(x$P[[p]]))
}


.test_astro <- function(path, tests=1:44) {
	result <- matrix(nrow=length(tests), ncol=8)
	j = 1;
	for (i in tests) {
		cat(paste0(i, "-")); flush.console(); if (i ==24) cat("\n")
		fname <- paste0("test_astro_wofost71_", formatC(i, width=2, flag="0"), ".yaml")
		yf <- file.path(path, fname)
		tst <- .yamltest(yf) 
		result[j,] <- sapply(colnames(tst$P), function(v) .error(tst, v) )
		j = j + 1;
	}
	cat("\n")
	if (max(result) > 1e-04) {
		print("ASTRO error")
	} else {
		print(paste("astro OK:", round(max(result), 10)))
	}
	colnames(result) <- colnames(tst$P)
	result
}


.test_prec <- function(x, p, prec) {
	dif <- abs(x[[paste0(p, ".x")]] - x[[paste0(p, ".y")]])
	sum(dif > prec[[p]])
}

.test_phenology <- function(path, tests=1:44) {
	params <- c("DVR", "DVS")
	result <- matrix(nrow=length(tests), ncol=2)
	colnames(result) <- params

	j = 1;
	for (i in tests) {
		fname <- paste0("test_phenology_wofost71_", formatC(i, width=2, flag="0"), ".yaml")
		yf <- file.path(path, fname)
		x <- .yamltest(yf) 
		if (!x$skip) {
			cat(paste0(i, "-"))
			m <- merge(x$P[, c("DAY", params)], x$R[, c("date", params)], by=1)
			result[j,] <- sapply(params, function(v) .test_prec(m, v, x$prec) )
		} else {
			cat(paste0(i, "x"))
		}
		flush.console(); if (i ==24) cat("\n")
		j <- j + 1;
	}
	cat("\n")
	if (max(result, na.rm=TRUE) > 0) {
		print("phenology error")
	} else {
		print(paste("phenology OK"))
	}
	result
}


#library(Rwofost)
#ydir <- "C:/github/cropmodels/Rwofost/test_data/"
#x <- Rwofost:::.test_astro(ydir)
#x <- Rwofost:::.test_phenology(ydir)

#.crop_pars = Rwofost:::.crop_pars
#.soil_pars = Rwofost:::.soil_pars
#f <- file.path(ydir, "test_astro_wofost71_01.yaml")
#x <- .yamltest(f) 
#par(ask=T)
#for (v in colnames(x$P)) .complot(x, v)
