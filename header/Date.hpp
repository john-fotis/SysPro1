#ifndef DATE_HPP
#define DATE_HPP

#include <iostream>
#include <cstdlib>
#include <ctime>

#include "StringLibrary.hpp"

#define DAYS_PER_MONTH 30
#define MONTHS_IN_YEAR 12
#define OLDEST_YEAR 1900

static std::time_t t = std::time(0); // Current time!
static std::tm *tm = std::localtime(&t);

class Date {
private:
  int day, month, year;

public:
  Date(int d=tm->tm_mday, int m=tm->tm_mon+1, int y=tm->tm_year+1900)
  : day(d), month(m), year(y) {}
  // Gets a string formatted date and converts it to int
  Date(std::string date) { set(date); }

  void get(int &d, int &m, int &y) const { d = day; m = month; y = year; }
  void get(std::string &date) const {
    date.clear();
    date.append(toString(day));
    date.append("-");
    date.append(toString(month));
    date.append("-");
    date.append(toString(year));
  }

  void set(int d=tm->tm_mday, int m=tm->tm_mon+1, int y=tm->tm_year+1900) {
    day = d;
    month = m;
    year = y;
  }
  void set(std::string date) {
    if (!date.compare("")) { day = month = year = 0; return; }
    bool validDate = true;
    std::string d(""), m(""), y("");
    List<std::string> args;
    splitLine(date, args, '-');
    if (args.getSize() != 3) validDate = false;
    else {
      d = *args.getNode(0);
      m = *args.getNode(1);
      y = *args.getNode(2);
    }

    for (unsigned int pos = 0; pos < d.length(); pos++)
      if (!std::isdigit(d[pos])) validDate = false;
    for (unsigned int pos = 0; pos < m.length(); pos++)
      if (!std::isdigit(m[pos])) validDate = false;
    for (unsigned int pos = 0; pos < y.length(); pos++)
      if (!std::isdigit(y[pos])) validDate = false;

    if (validDate) {
      day = myStoi(d.c_str());
      month = myStoi(m.c_str());
      year = myStoi(y.c_str());
    } else day = month = year = 0;
  }

  inline friend bool operator==(const Date &d1, const Date &d2);
  inline friend bool operator!=(const Date &d1, const Date &d2);
  inline friend bool operator<(const Date &d1, const Date &d2);
  inline friend bool operator>(const Date &d1, const Date &d2);
  inline friend bool operator<=(const Date &d1, const Date &d2);
  inline friend bool operator>=(const Date &d1, const Date &d2);
  inline friend std::ostream &operator<<(std::ostream &os, const Date &date);

  bool valid() const {
    // Attention needed. Day 31 of a month is considered invalid for simplicity
    if (day < 1 || day > DAYS_PER_MONTH) return false;
    if (month < 1 || month > MONTHS_IN_YEAR) return false;
    if (year < OLDEST_YEAR) return false;
    return true;
  }
  void print() const {std::cout << day << "-" << month << "-" << year << std::endl;}
};

inline bool operator==(const Date &d1, const Date &d2) {
  return (d1.day == d2.day && d1.month == d2.month && d1.year == d2.year);
}

inline bool operator!=(const Date &d1, const Date &d2) {
  return !(d1 == d2);
}

inline bool operator<(const Date &d1, const Date &d2) {
  if (d1.year < d2.year) return true;
  if (d1.month < d2.month) return true;
  if (d1.day < d2.day) return true;
  return false;
}

inline bool operator>(const Date &d1, const Date &d2) {
  return (!(d1<d2) && (d1!=d2));
}

inline bool operator<=(const Date &d1, const Date &d2) {
  return !(d1>d2);
}

inline bool operator>=(const Date &d1, const Date &d2) {
  return !(d1<d2);
}

inline std::ostream &operator<<(std::ostream &os, const Date &date) {
  std::string str;
  date.get(str);
  os << str;
  return os;
}

#endif