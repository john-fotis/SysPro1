#ifndef RECORD_HPP
#define RECORD_HPP

#include <iostream>
#include "Date.hpp"

class Record {
private:
  unsigned int citizenID;
  std::string firstName;
  std::string lastName;
  std::string country;
  unsigned int age;
  std::string virusName;
  std::string status; // [YES/NO]
  Date date;
public:
  Record() { citizenID = 0; age = 0; }
  ~Record() {}

  unsigned int ID() const { return citizenID; }
  std::string getFirstName() const { return firstName; }
  std::string getLastName() const { return lastName; }
  std::string getCountry() const { return country; }
  unsigned int getAge() const { return age; }
  std::string getVirusName() const { return virusName; }
  std::string getStatus() const { return status; }
  std::string getDate() const { std::string str(""); date.get(str); return str; }

  Record &operator=(const Record &record);
  friend bool operator==(const Record &r1, const Record &r2);
  friend bool operator!=(const Record &r1, const Record &r2);
  friend bool operator<(const Record &r1, const Record &r2);
  friend bool operator>(const Record &r1, const Record &r2);
  friend bool operator<=(const Record &r1, const Record &r2);
  friend bool operator>=(const Record &r1, const Record &r2);
  friend std::ostream &operator<<(std::ostream &os, const Record &record);

  void set(unsigned int id, std::string name,
          std::string surname, std::string country,
          unsigned int age, std::string virus,
          std::string status, Date date);
  void print() const;
};

#endif