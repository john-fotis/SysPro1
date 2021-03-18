#!/bin/bash

################### Defined values

duplicateRatio=50 # %
vaccinateRatio=50 # %
# We use a dictionary of first and last names
firstNamesNumber=100
lastNamesNumber=100
outputFile="citizenRecordsFile"
# Above 10000, we can't ensure unique IDs
maxID=10000


################### Get input arguments

for arg in "$@"; do
  index=$(echo $arg | cut -f1 -d=)
  val=$(echo $arg | cut -f2 -d=)
  case $index in
    virusesFile) virusList=$val;;
    countriesFile) countryList=$val;;
    $3) lines=$val;;
    $4) duplicates=$val;;
    *)
  esac
done


################### Check Input - Start

inputOk=true

if [ ! -f "$virusList" ]; then
  echo "Virus file not found."
  inputOk=false
fi

if [ ! -f "$countryList" ]; then
  echo "Country file not found."
  inputOk=false
fi

if [ $lines -lt 1 ]; then
  echo "[numLines] must be greater than 0."
  inputOk=false
fi

if [[ ($lines -gt 10000 && $duplicates -eq 0) ]]; then
  echo "Can't produce more than 10K unique IDs with the restriction of 4-digits"
  inputOk=false
fi

echo "Input was: $1, $2, $3, $4"

if [[ $inputOk == false ]]; then
  printf "Correct Usage:\n./testFile.sh [virusesFile] [countriesFile] [numLines] [duplicatesAllowed]\n"
  exit 1
fi

################### Check Input - End

start=$(date +%s.%N) # Start clock

################### Get the file records data - Start

# Viruses file
declare -a virusArray;
file="$virusList"
while read line; do virusArray+=("$line"); done < $file
printf "Virus Records:\t %d\n" $((${#virusArray[@]}))

# Countries file
declare -a countryArray;
file="$countryList"
while read line; do countryArray+=("$line"); done < $file
printf "Country Records: %d\n" $((${#countryArray[@]}))

# Generate 100 random firstnames to use as seed below
declare -a firstNamesArray;
for ((i=0; i<$firstNamesNumber; i++)) do
  length=$(($RANDOM%9+3)) # Random length of 3-12 characters
  firstNamesArray+=("$(tr -dc A-Z </dev/urandom | head -c $length)")
done
printf "First names:\t %d\n" $((${#firstNamesArray[@]}))

# Generate 100 random lastNames to use as seed below
declare -a lastNamesArray;
for ((i=0; i<$lastNamesNumber; i++)) do
  length=$(($RANDOM%9+3)) # Random length of 3-12 characters
  lastNamesArray+=("$(tr -dc A-Z </dev/urandom | head -c $length)")
done
printf "Last names:\t %d\n" $((${#lastNamesArray[@]}))

################### Get the file records data - End


################### Declare and initialize variables - Start

# Check if output file already exists and if so, delete it
[[ -f "$outputFile" ]] && rm -f $outputFile
touch $outputFile

# Array of unused IDs
declare -a unusedIdArray;
# Array that keeps |ID-firstName-lastName-country-age| of all original records
availableIdAmount=$maxID

if [[ $duplicates -eq 0 ]]; then
  # Fill up with all possible unused IDs (0-9999)
  for ((i=0; i<$maxID; i++)) do unusedIdArray+=($i); done
else
  # Generate the first original record in order to have something to duplicate
  id=$(($RANDOM%$maxID))
  length=$(($RANDOM%9+3))
  firstName=$(tr -dc A-Z </dev/urandom | head -c $length)
  length=$(($RANDOM%9+3))
  lastName=$(tr -dc A-Z </dev/urandom | head -c $length)
  country=${countryArray[(($RANDOM % ${#countryArray[@]}))]}
  age=$(($RANDOM%120+1))
fi

################### Declare and initialize variables - End


################### Main Loop

for ((counter=0; counter<$lines; counter++)) do

  if [[ $duplicates -eq 0 ]]; then
  ### Case: Avoid duplicate IDs
    # Find a random index in the remaining IDs array and claim its value
    index=$(($RANDOM%$availableIdAmount))
    id=${unusedIdArray[$index]}
    # After claiming this ID, we can swap it with the last one in the array
    # and reduce the size of the array virtually by 1
    unusedIdArray[$index]=${unusedIdArray[$availableIdAmount-1]}
    let "availableIdAmount--"
    # Generate the rest info
    firstName=${firstNamesArray[(($RANDOM % ${#firstNamesArray[@]}))]}
    lastName=${lastNamesArray[(($RANDOM % ${#lastNamesArray[@]}))]}
    country=${countryArray[(($RANDOM % ${#countryArray[@]}))]}
    age=$(($RANDOM%120+1))
  else
  ### Case: Cause duplicate IDs on purpose based on defined possibility
    if [[ "$((RANDOM%100))" -gt $duplicateRatio || $counter -lt 1 ]]; then
    ### Case: Generate an original record
      # All variables here are initialized randomly
      id=$(($RANDOM%$maxID))
      firstName=${firstNamesArray[(($RANDOM % ${#firstNamesArray[@]}))]}
      lastName=${lastNamesArray[(($RANDOM % ${#lastNamesArray[@]}))]}
      country=${countryArray[(($RANDOM % ${#countryArray[@]}))]}
      age=$(($RANDOM%120+1))
    else
    ### Case: Generate a duplicate record
      # We read a line from the output file that we have already written at least one record
      line=$(shuf -n 1 $outputFile)
      # We split this line's arguments in order to copy them
      arguments=(${line// / })
      # And initialize the appropriate variables
      id=$((${arguments}))
      firstName=${arguments[1]}
      lastName=${arguments[2]}
      country=${arguments[3]}
      age=${arguments[4]}
    fi
  fi

  # The virus, vaccination status and date are irrelevant with the above
  # computations and are initialized here, independently and randomly.
  virus=${virusArray[(($RANDOM % ${#virusArray[@]}))]}
  # Decide for a vaccination certificate based on the defined possibility
  [[ $(($RANDOM%100)) < $vaccinateRatio ]] && status="YES" || status="NO"

  # Check if we need to add a date
  if [ "$status" = "YES" ]; then
    dd=$(($RANDOM%30+1)); mm=$(($RANDOM%12+1)); yyyy=$(($RANDOM%22+2000))
    date="$dd-$mm-$yyyy"
    # Append the record to the output file
    echo $id $firstName $lastName $country $age $virus $status $date >> $outputFile
  else
    # Append the record to the output file without date
    echo $id $firstName $lastName $country $age $virus $status >> $outputFile
  fi

done

end=$(date +%s.%N) # Stop clock
runningTime=$(echo "$end - $start" | bc)
echo "Script runned for" $runningTime "seconds"
echo "Output stored in" $outputFile