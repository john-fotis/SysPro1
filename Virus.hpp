#ifndef VIRUS_HPP
#define VIRUS_HPP

#include <iostream>

#include "BloomFilter.hpp"
#include "SkipList.hpp"
#include "Record.hpp"

class Virus {
private:
  std::string name;
  BloomFilter filter;
  SkipList<Record> vaccinatedList;
  SkipList<int> nonVaccinatedList;
public:
  Virus() : name("") {}
  ~Virus() {}
  Virus (const Virus &virus);

  std::string getName() const { return name; }
  void setName(const std::string &str) { name = str; }

  Virus &operator=(const Virus &virus);
  friend bool operator==(const Virus &v1, const Virus &v2);
  friend bool operator!=(const Virus &v1, const Virus &v2);
  friend bool operator<(const Virus &v1, const Virus &v2);
  friend bool operator>(const Virus &v1, const Virus &v2);
  friend bool operator<=(const Virus &v1, const Virus &v2);
  friend bool operator>=(const Virus &v1, const Virus &v2);
  friend std::ostream &operator<<(std::ostream &os, const Virus &virus);

  void insertBloom(const std::string &str) { filter.insert(str); }
  bool checkBloom(const std::string &str) { return filter.check(str); }
  void bloomStatus() const { filter.arrayStatus(); }
};

#endif