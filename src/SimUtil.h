/* Author: Robert Hijmans
   Date: April 2016
   License: GNU General Public License (GNU GPL) v. 2 
*/

#ifndef SIMUTIL_H_
#define SIMUTIL_H_

#include <vector>
#include <algorithm>


template <class T> T minvalue(std::vector<T> v) {
  return *std::min_element(v.begin(),v.end());  
}

template <class T> T maxvalue(std::vector<T> v) {
  return *std::max_element(v.begin(),v.end());  
}



inline double NOTNUL(double x) {
	return (x == 0 ? 1 : x);
}	


inline double INSW(double x, double y1, double y2) {
	return (x < 0 ? y1 : y2);
}	

inline int REAAND(double x1, double x2) {
	return( ((x1 > 0) & (x2 > 0)) ? 1 : 0 );
}	


inline double LIMIT(double min, double max, double v) {
  if (v < min) {
    v = min;
  } else if (v > max) {
    v = max;
  }
  return(v);
}

inline double clamp(double minv, double maxv, double v) {
  return v < minv ? minv : (v > maxv ? maxv : v);
}

inline void clampinplace(double minv, double maxv, double &v) {
  v = v < minv ? minv : (v > maxv ? maxv : v);
}

inline double AFGEN(std::vector<double> xy, double x) {
	int n = xy.size();
	double y = -99;
	if (x <= xy[0]) {
		y = xy[1];
	} else if (x >= xy[n-2]) {
		y = xy[n-1];
	} else {
		for(int i=2; i<n; i=i+2) {
			if (xy[i] > x) {
				double slope = (xy[i+1] - xy[i-1]) / (xy[i] - xy[i-2]);
				y = xy[i-1] + (x - xy[i-2]) * slope;
				break;
			}
		}
	}
	return(y);
}


inline double AFGEN2(const std::vector<double> &xy, const double &x) {
	size_t n = xy.size();
	size_t hn = n / 2;
	double y = 0;
	if (x <= xy[0]) {
		y = xy[hn];
	} else if (x >= xy[hn-1]) {
		y = xy[n-1];
	} else {
		for (size_t i=1; i<hn; i++) {
			if (x < xy[i]) {
				double slope = (xy[i+hn] - xy[i+hn-1]) / (xy[i] - xy[i-1]);
				y = xy[i+hn] + (x - xy[i]) * slope;
				break;
			}
		}
	}
	return(y);
}



inline double LINT(std::vector<std::vector<double> > xy, double x) {
  int n = xy.size();
  double y = -1;
  if (x < xy[0][0] ) {
    y = xy[0][1];
  } else if (x > xy[n-1][0]) {
    y = xy[n-1][1];
  } else {
    for(int i=1; i<n; i++) {
      if (xy[i][0] > x) {
        double slope = (xy[i][1] - xy[i-1][1]) / (xy[i][0] - xy[i-1][0]);
        y = xy[i-1][1] + (x - xy[i-1][0]) * slope;
        break;
      }
    }
  }
  return(y);
}	


inline double approx(std::vector<std::vector<double> > xy, double x) {
  int n = xy.size();
  double y = -1;
  if (x < xy[0][0] ) {
    y = xy[0][1];
  } else if (x > xy[n-1][0]) {
    y = xy[n-1][1];
  } else {
    for(int i=1; i<n; i++) {
      if (xy[i][0] > x) {
        double slope = (xy[i][1] - xy[i-1][1]) / (xy[i][0] - xy[i-1][0]);
        y = xy[i-1][1] + (x - xy[i-1][0]) * slope;
        break;
      }
    }
  }
  return(y);
}	


inline double approx(std::vector<double> xy, double x) {
	int s = xy.size() / 2;
	std::vector<double> X(xy.begin(), xy.begin() + s - 1);
	std::vector<double> Y(xy.begin()+s, xy.end());
	int n = X.size();
	double y = 0;
	if (x < X[0] ) {
		y = Y[0];
	} else if (x > X[n-1]) {
		y = Y[n-1];
	} else {
		for(int i=1; i<n; i++) {
			if (X[i] > x) {
				double slope = (Y[i] - Y[i-1]) / (X[i] - X[i-1]);
				y = Y[i-1] + (x - X[i-1]) * slope;
				break;
			}
		}
	}
	return(y);
}	



inline std::vector<std::vector<double> > matrix(int nrow, int ncol) {
	std::vector<std::vector<double> > m (nrow, std::vector<double>(ncol));
	return(m);
}



// based on civil_from_days, with changes by RH to return DOY instead
// source: http://howardhinnant.github.io/date_algorithms.html#civil_from_days
// Returns year/month/day triple in civil calendar
// Preconditions:  z is number of days since 1970-01-01 and is in the range:
//                   [numeric_limits<Int>::min(), numeric_limits<Int>::max()-719468].
inline unsigned doy_from_days(long z) {
    z += 719468;
    const long era = (z >= 0 ? z : z - 146096) / 146097;
    const unsigned doe = static_cast<unsigned>(z - era * 146097);          // [0, 146096]
    const unsigned yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365;  // [0, 399]
    const long y = static_cast<long>(yoe) + era * 400;
    const unsigned doy = doe - (365*yoe + yoe/4 - yoe/100);                // [0, 365]
    const unsigned mp = (5*doy + 2)/153;                                   // [0, 11]
   // const unsigned d = doy - (153*mp+2)/5 + 1;                             // [1, 31]
    const unsigned m = mp + (mp < 10 ? 3 : -9);                            // [1, 12]

	// return std::tuple<Int, unsigned, unsigned>(y + (m <= 2), m, d);
	// From here, changes to return DOY instead (by RH)
	long yy = y + (m <= 2);
	bool isleap = yy % 4 == 0 && (yy % 100 != 0 || yy % 400 == 0);
	return (doy + 59 + isleap) % (365 + isleap) + 1;
}




#endif
