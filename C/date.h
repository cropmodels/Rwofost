/*
Auhtor: Amandeep Sidhu,
partly based on code by __________
with contribution by Robert Hijmans

Date: July 2016
License: GNU General Public License (GNU GPL) v. 2
*/

#ifndef DATE_H
#define DATE_H
#include <iostream>

class date {
  protected:
    int vyear;
    int vmonth;
    int vday;

  public:
    date();
    date(const int& year, const int& month, const int& day);
    date(int doy, int year);
    date(const std::string& s);

    bool valid(void) const;

    int day() const;
    int month() const;
    int year() const;
    int dayofyear();
    bool is_leap_year() const;

    void set_day (const int& day );
    void set_month (const int& month );
    void set_year (const int& year );

    date operator ++(); // prefix
    date operator ++(int); // postfix
    date operator --(); // prefix
    date operator --(int); // postfix
    void operator =( const date&);
};

date operator +(date&, int);
date operator +(int, date&);
date operator -(date&, int);
bool operator == (const date&, const date&); // comparison operators
bool operator != (const date&, const date&);
bool operator < (const date&, const date&);
bool operator > (const date&, const date&);
bool operator <= (const date&, const date&);
bool operator >= (const date&, const date&);
std::ostream& operator << ( std::ostream& os, const date& d); // output operator
int operator-( date&, date&);

#endif
