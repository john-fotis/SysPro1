#ifndef COUNTRY_HPP
#define COUNTRY_HPP

#include <iostream>

class Country {
private:
  std::string name;
  unsigned int population;
  
public:
  Country() : name(""), population(1) {}
  ~Country() {}
  Country(const Country &country);

  std::string getName() const { return name; }
  unsigned int getPopulation() const { return population; }

  void setName(const std::string &str) { name.clear(); name.assign(str); }
  void addPerson() { population++; }

  Country &operator=(const Country &country);
  friend bool operator==(const Country &c1, const Country &c2);
  friend bool operator!=(const Country &c1, const Country &c2);
  friend bool operator<(const Country &c1, const Country &c2);
  friend bool operator>(const Country &c1, const Country &c2);
  friend bool operator<=(const Country &c1, const Country &c2);
  friend bool operator>=(const Country &c1, const Country &c2);
  friend std::ostream &operator<<(std::ostream &os, const Country &country);
};

#endif