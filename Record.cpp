#include "Record.hpp"

Record &Record::operator=(const Record &record) {
  if (this == &record) return *this;
  citizenID = record.citizenID;
  firstName = record.firstName;
  lastName = record.lastName;
  country = record.country;
  age = record.age;
  virusName = record.virusName;
  status = record.status;
  date = record.date;
  return *this;
}

bool operator==(const Record &r1, const Record &r2) {
  return (
      r1.citizenID == r2.citizenID &&
      r1.firstName == r2.firstName &&
      r1.lastName == r2.lastName &&
      r1.country == r2.country &&
      r1.age == r2.age &&
      r1.virusName == r2.virusName &&
      r1.status == r2.status &&
      r1.citizenID == r2.citizenID &&
      r1.date == r2.date
  );
}

bool operator!=(const Record &r1, const Record &r2) {
  return !(r1==r2);
}

bool operator<(const Record &r1, const Record &r2) {
  return r1.citizenID < r2.citizenID;
}
bool operator>(const Record &r1, const Record &r2) {
  return r1.citizenID > r2.citizenID;
}
bool operator<=(const Record &r1, const Record &r2) {
  return r1.citizenID <= r2.citizenID;
}
bool operator>=(const Record &r1, const Record &r2) {
  return r1.citizenID >= r2.citizenID;
}

std::ostream &operator<<(std::ostream &os, const Record &record) {
  os << record.citizenID << " " << record.firstName << " "
     << record.lastName << " " << record.country << " "
     << record.age << " " << record.virusName << " " << record.status;
  std::string str(record.status);
  if (!str.compare("YES")) os << " " << record.date;
  return os;
}

void Record::set(unsigned int id, std::string name,
                std::string surname, std::string country,
                unsigned int age, std::string virus,
                std::string status, Date date) {
  this->citizenID = id;
  this->firstName = name;
  this->lastName = surname;
  this->country = country;
  this->age = age;
  this->virusName = virus;
  this->status = status;
  this->date = date;
}

void Record::print() const {
  std::cout << citizenID << " " << firstName << " " << lastName << " "
  << country << " " << age << " " << virusName << " " << status << " ";
  date.print();
}