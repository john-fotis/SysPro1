#ifndef DATE_HPP
#define DATE_HPP

#include <iostream>
#include <cstdlib>
#include "string.hpp"

#define DAYS_PER_MONTH 30

class Date {
private:
  int day, month, year;
public:
  Date(int d=1, int m=1, int y=2021) { day = d; month = m; year = y; }
  // Gets a string formatted date and converts it to int
  Date(std::string date) {
    std::string d, m, y;
    int i = 0, j = 0, count = 0;
    for (i = 0; std::isdigit(date[i]); i++) count++;
    d = date.substr(0, count);
    count = 0;
    j = i + 1;
    for (i = i+1; std::isdigit(date[i]); i++) count++;
    m = date.substr(j, count);
    count = 0;
    j = i + 1;
    for (i = i+1; std::isdigit(date[i]); i++) count++;
    y = date.substr(j, count);
    day = std::atoi(d.c_str());
    month = std::atoi(m.c_str());
    year = std::atoi(y.c_str());
  }
  ~Date() {}
  Date &operator=(const Date &date) {
    if (this == &date) return *this;
    int d, m, y;
    date.get(d, m, y);
    this->set(d, m, y);
    return *this;
  }

  inline friend bool operator==(const Date &d1, const Date &d2);
  inline friend bool operator!=(const Date &d1, const Date &d2);
  inline friend bool operator<(const Date &d1, const Date &d2);
  inline friend bool operator>(const Date &d1, const Date &d2);
  inline friend bool operator<=(const Date &d1, const Date &d2);
  inline friend bool operator>=(const Date &d1, const Date &d2);
  inline friend std::ostream &operator<<(std::ostream &os, const Date &date);

  void get(int &d, int &m, int &y) const { d = day; m = month; y = year; }
  void get(std::string &date) const {
    date.clear();
    date.append(toString(day));
    date.append("-");
    date.append(toString(month));
    date.append("-");
    date.append(toString(year));
  }
  void set(int d, int m, int y) { day = d; month = m; year = y; }
  void set(std::string date) {
    std::string d, m, y;
    int i = 0, j = 0, count = 0;
    for (i = 0; std::isdigit(date[i]); i++) count++;
    d = date.substr(0, count); count = 0;
    j = i + 1;
    for (i = i + 1; std::isdigit(date[i]); i++) count++;
    m = date.substr(j, count);
    count = 0;
    j = i + 1;
    for (i = i + 1; std::isdigit(date[i]); i++) count++;
    y = date.substr(j, count);
    day = std::atoi(d.c_str());
    month = std::atoi(m.c_str());
    year = std::atoi(y.c_str());
  }
  void print()const {std::cout << day << "-" << month << "-" << year << std::endl;}
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
  return (!(d1<d2) && d1!=d2);
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