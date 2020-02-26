
wofost_model <- function(crop, weather, soil, control, location) {
	m <- WofostModel$new()
	if (!missing(crop)) { crop(m) <- crop }
	if (!missing(soil)) { soil(m) <- soil }
	if (!missing(control)) { control(m) <- control }
	if (!missing(weather)) { weather(m) <- weather }
	if (!missing(location)) { location(m) <- location }
	return(m)
}

setMethod("run", signature("Rcpp_WofostModel"), 
	function(x, ...) {
		x$run()
		out <- matrix(x$output$values, ncol=length(x$output$names), byrow=TRUE)
		colnames(out) <- x$output$names
		out <- data.frame(out)
		date <- as.Date(x$control$modelstart, origin="1970-01-01") + (out$step - 1)
		out <- data.frame(date, out)
#		out$Wtot <- out$WRT + out$WLV + out$WST + out$WSO
		out
	}
)


.crop_pars <- c("TBASEM", "TEFFMX", "TSUMEM", "IDSL", "DLO", "DLC", "TSUM1", "TSUM2", "DTSMTB", "DVSI", "DVSEND", "TDWI", "LAIEM", "RGRLAI", "SLATB", "SPA", "SSATB", "SPAN", "TBASE", "CVL", "CVO", "CVR", "CVS", "Q10", "RML", "RMO", "RMR", "RMS", "RFSETB", "FRTB", "FLTB", "FSTB", "FOTB", "PERDL", "RDRRTB", "RDRSTB", "CFET", "DEPNR", "RDI", "RRI", "RDMCR", "IAIRDU", "KDIFTB", "EFFTB", "AMAXTB", "TMPFTB", "TMNFTB", "CO2AMAXTB", "CO2EFFTB", "CO2TRATB")

setMethod("crop<-", signature("Rcpp_WofostModel", "list"), 
	function(x, value) {
		if (is.null(value$IARDU)) {
			value$IAIRDU = 0
		}
		nms <- names(value)
		if (!all(.crop_pars %in% nms)) stop(paste("parameters missing:", paste(.crop_pars[!(.crop_pars %in% nms)], collapse=", ")))
		value <- value[.crop_pars]
		nms <- names(value)
		lapply(1:length(value), function(i) eval(parse(text = paste0("x$crop$p$", nms[i], " <- ", value[i]))))
		return(x)
	}
)

.soil_pars <- c("SMTAB", "SMW", "SMFCF", "SM0", "CRAIRC", "CONTAB", "K0", "SOPE", "KSUB", "SPADS", "SPASS", "SPODS", "SPOSS", "DEFLIM", "IZT", "IFUNRN", "WAV", "ZTI", "DD", "IDRAIN", "NOTINF", "SSMAX", "SMLIM", "SSI", "RDMSOL")

setMethod("soil<-", signature("Rcpp_WofostModel", "list"), 
	function(x, value) {
		nms <- names(value)
		if (!all(.soil_pars %in% nms)) stop(paste("parameters missing:", paste(.soil_pars[!(.soil_pars %in% nms)], collapse=", ")))
		value <- value[.soil_pars]
		nms <- names(value)
		lapply(1:length(value), function(i) eval(parse(text = paste0("x$soil$p$", nms[i], " <- ", value[i]))))
		return(x)
	}
)



setMethod("weather<-", signature("Rcpp_WofostModel", "data.frame"), 
	function(x, value) {
		parameters <- c("date", "srad", "tmin", "tmax", "prec", "wind", "vapr")
		nms <- colnames(value)
		if (!all(parameters %in% nms)) stop(paste("weather variables missing:", paste(parameters[!(parameters %in% nms)], collapse=", ")))

		# x$setWeather(value$date, value$tmin, value$tmax, value$srad, value$prec, value$wind, value$vapr)

		w <- WofostWeather$new()
		w$date <- as.integer(value$date)
		w$srad <- value$srad
		w$tmin <- value$tmin
		w$tmax <- value$tmax
		w$prec <- value$prec
		w$wind <- value$wind
		w$vapr <- value$vapr
		x$weather <- w


		return(x)
	}
)



.req_ctr_pars <- c("modelstart", "cropstart", "water_limited", "nutrient_limited", "IOXWL", "ISTCHO", "IDESOW", "IDLSOW", "IENCHO", "IDAYEN", "IDURMX", "latitude", "CO2", "elevation")
.opt_ctr_pars <- c("output", "ANGSTA", "AMAXTB")

setMethod("control<-", signature("Rcpp_WofostModel", "list"), 
	function(x, value) {
		nms <- names(value)
		if (!all(.req_ctr_pars %in% nms)) stop(paste("parameters missing:", paste(.req_ctr_pars[!(.req_ctr_pars %in% nms)], collapse=", ")))
		value <- value[nms %in% c(.req_ctr_pars, .opt_ctr_pars)]
		nms <- names(value)

		# value <- sapply(value, function(v) ifelse(is.character(v), paste0("\'", v, "\'"), v))
		if (!is.null(value$output)) value$output <- paste0("\'", value$output, "\'")

		lapply(1:length(value), function(i) eval(parse(text = paste0("x$control$", nms[i], " <- ", value[i]))))
		return(x)
	}
)


if (!isGeneric("force<-")) { setGeneric("force<-", function(x, value) standardGeneric("force<-")) }	

setMethod("force<-", signature("Rcpp_WofostModel", "data.frame"), 
	function(x, value) {
		count <- 0
		fs <- ""
		for (field in c("DVS", "LAI", "SM", "WLV", "WRT", "WSO", "WST")) {
			if (!is.null(value[[field]])) {
				eval(parse(text = paste0("x$forcer$force_", field, " <- TRUE")))				
				eval(parse(text = paste0("x$forcer$", field, " <- value$", field)))
				count <- count + 1
			}
		}
		if (count > 0) {
			x$control$useForce <- TRUE
		}
		return(x)
	}
)

