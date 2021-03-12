#include "Person.hpp"

Person::Person(const Person &person) {
  if (this == &person) return;
  id = person.ID();
  firstName.clear();
  firstName = person.getFirstName();
  lastName.clear();
  lastName = person.getLastName();
  country = &person.getCountry();
  age = person.getAge();
}

Person &Person::operator=(const Person &person) {
  if (this == &person) return *this;
  id = person.ID();
  firstName.clear();
  firstName = person.getFirstName();
  lastName.clear();
  lastName = person.getLastName();
  country = &person.getCountry();
  age = person.getAge();
  return *this;
}

bool operator==(const Person &r1, const Person &r2) {
  return (
    r1.ID() == r2.ID() &&
    r1.getFirstName() == r2.getFirstName() &&
    r1.getLastName() == r2.getLastName() &&
    r1.getCountry() == r2.getCountry() &&
    r1.getAge() == r2.getAge()
  );
}

bool operator!=(const Person &r1, const Person &r2) {
  return !(r1 == r2);
}

bool operator<(const Person &r1, const Person &r2) {
  return r1.ID() < r2.ID();
}

bool operator>(const Person &r1, const Person &r2) {
  return r1.ID() > r2.ID();
}

bool operator<=(const Person &r1, const Person &r2) {
  return r1.ID() <= r2.ID();
}

bool operator>=(const Person &r1, const Person &r2) {
  return r1.ID() >= r2.ID();
}

std::ostream &operator<<(std::ostream &os, const Person &person) {
  os << person.ID() << " "
     << person.getFirstName() << " "
     << person.getLastName() << " "
     << person.getCountry() << " "
     << person.getAge();
  return os;
}

void Person::set( unsigned int id, std::string name, std::string surname,
                 Country *country, unsigned int age) {
  this->id = id;
  firstName.clear();
  firstName = name;
  lastName.clear();
  lastName = surname;
  this->country = country;
  this->age = age;
}

void Person::print() const {
  std::cout << id << " " << " " << firstName << " " 
  << lastName << " "  << *country << " " << age;
}