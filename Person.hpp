#ifndef Person_HPP
#define Person_HPP

#include <iostream>
#include "Date.hpp"
#include "Country.hpp"

class Person {
private:
  unsigned int id;
  std::string firstName;
  std::string lastName;
  Country *country;
  unsigned int age;

public:
  Person(): id(0), firstName(""), lastName(""), country(NULL), age(0) {}
  ~Person() {}
  Person(const Person &person);

  unsigned int ID() const { return id; }
  std::string getFirstName() const { return firstName; }
  std::string getLastName() const { return lastName; }
  Country &getCountry() const { return *country; }
  unsigned int getAge() const { return age; }

  Person &operator=(const Person &Person);
  friend bool operator==(const Person &r1, const Person &r2);
  friend bool operator!=(const Person &r1, const Person &r2);
  friend bool operator<(const Person &r1, const Person &r2);
  friend bool operator>(const Person &r1, const Person &r2);
  friend bool operator<=(const Person &r1, const Person &r2);
  friend bool operator>=(const Person &r1, const Person &r2);
  friend std::ostream &operator<<(std::ostream &os, const Person &Person);

  void set(unsigned int id, std::string name, std::string surname,
          Country *country, unsigned int age);
  void print() const;
};

#endif