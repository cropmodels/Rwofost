#path="C:/github/cropmodels/Rwofost/test_for"
#ydir <- "C:/github/cropmodels/Rwofost/test_data/"
#yf <- file.path(ydir, "test_astro_wofost71_01.yaml")

.yam_tofortran <- function(yf, path, id) {
	dir.create(file.path(path, "meteo"), FALSE, TRUE)
	dir.create(file.path(path, "soild"), FALSE, TRUE)
	dir.create(file.path(path, "cropd"), FALSE, TRUE)
	dir.create(file.path(path, "runio"), FALSE, TRUE)
	dir.create(file.path(path, "test"), FALSE, TRUE)
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
	
	Rwofost:::.writeFSEwth(w, country='AAA', station=id, lon=0, lat=x$LAT[1], elev=10, file.path(path, "meteo"))

	tim <- wofost_control()
	if (ttype == "waterlimitedproduction") {
		tim$water_limited <- TRUE
	} else {
		tim$water_limited <- FALSE
	} 

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

		nms <- names(dcrop)
		nms <- nms[!(nms %in% names(pcrop))]
		pcrop <- c(pcrop, dcrop[nms])
	}

	.writeFSE(psoil, file.path(path, "soild", paste0("soil_", id)))
	.writeFSE(pcrop, file.path(path, "cropd", paste0("crop_", id)))
	
	r <- y$ModelResults
	r <- lapply(r, unlist)
	r <- data.frame(t(do.call(cbind, r)), stringsAsFactors=FALSE)
	write.csv(r, file.path(path, "test", paste0("results", id, ".csv")), row.names=FALSE)
}

