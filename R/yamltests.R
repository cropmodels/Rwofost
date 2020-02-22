
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

	# latitude is a "comment"
	# also in weather
	#txt <- readLines(yf)
	#lat <- txt[grep("latitude", txt)]
	#lat <- as.numeric(strsplit(lat, ":")[[1]][2])


	y <- yaml::read_yaml(yf)
	m <- y$AgroManagement
	cal <- m[[1]][[1]]$CropCalendar

	tim <- wofost_control()
	tim$modelstart <- as.Date(names(m[[1]])) + 1
	tim$cropstart <- as.integer(tim$modelstart - as.Date(cal$crop_start_date))
	if (isTRUE(grep("^test_waterlimitedproduction_wofost", basename(yf)) == 1)) tim$IPRODL <- 1
	tim$IDURMX <- cal$max_duration
	if (cal$crop_start_type == "sowing") tim$ISTCHO = 1

	p <- y$ModelParameters
	np <- names(p)
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
	s <- wofost_soil("soil_5")
	s <- s[c("SMTAB", "CONTAB", "SPADS", "SPASS", "SPODS", "SPOSS", "DEFLIM", "IZT", "ZTI", "DD", "IDRAIN")]
    psoil <- c(psoil, s)

	yw <- y$WeatherVariables 
	yw <- lapply(yw, unlist)
	yw <- data.frame(t(do.call(cbind, yw)), stringsAsFactors=FALSE)
	lat <- yw$LAT[1]
	ww <- data.frame(
				date=as.Date(yw$DAY), 
				srad=as.numeric(yw$IRRAD) / 1000,
				prec=as.numeric(yw$RAIN),
				tmin=as.numeric(yw$TMIN),
				tmax=as.numeric(yw$TMAX),
				vapr=as.numeric(yw$VAP),
				wind=as.numeric(yw$WIND)
			)

	## elevation? CO2?
	site <- list(latitude=lat, elevation=10, CO2=360)
	
	#mod <- wofost_model()
	#crop(mod) <- pcrop
	#soil(mod) <- psoil
	#weather(mod) <- ww
	#location(mod) <- loc
	#control(mod) <- tim
	#x <- run(mod)
	m <- wofost_model(pcrop, ww, psoil, tim, tim)
	x <- run(m)
		
	r <- y$ModelResults
	r <- lapply(r, unlist)
	r <- data.frame(t(do.call(cbind, r)), stringsAsFactors=FALSE)
	r[,1] <- as.Date(r[,1])
	for (i in 2:ncol(r)) r[,i] <- as.numeric(r[,i])
	
	list(R=x, P=r)
}


.complot <- function(x, p, r=p) {
	plot(x$P$DAY, x$P[[p]], ylim=range(c(x$P[[p]], x$R[[r]])), xlab="", ylab=p, col="blue")
	points(x$R$date, x$R[[r]], col="red", lwd=2, pch=3)
	legend("topleft", c("Py", "R"), pch=c(1, 3), col=c("blue", "red"))
}


#ydir <- "C:/github/cropmodels/Rwofost/yamltests/"
#fy <- file.path(ydir, "test_waterlimitedproduction_wofost71_15.yaml")
#tst <- Rwofost:::.yamltest(fy) 
#par(ask=T)
#for (v in c("DVS", "EVS", "LAI", "RD", "SM", "TAGP", "TRA", "TWRT", "TWLV", "TWST", "TWSO")) Rwofost:::.complot(tst, v)
