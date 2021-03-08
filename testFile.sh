#! /bin/bash

################### Defined values

duplicateRatio=50 # %
vaccinateRatio=50 # %
firstNameFile="firstNames"
lastNameFile="lastNames"
outputFile="citizenRecordsFile"
# We accept IDs up to 4 digits thus ID<10000
# Above that, we can't ensure unique IDs
maxID=10000

################### Defined values

################### Get input

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

################### Get Input


################### Check Input

if [ ! -f "$virusList" ]; then
  echo "Virus file not found."
  printf "Correct Usage:\n./testFile.sh [virusesFile] [countriesFile] [numLines] [duplicatesAllowed]\n"
  exit 1
fi

if [ ! -f "$countryList" ]; then
  echo "Country file not found."
  printf "Correct Usage:\n./testFile.sh [virusesFile] [countriesFile] [numLines] [duplicatesAllowed]\n"
  exit 2
fi

if [ $lines -lt 1 ]; then
  echo "[numLines] must be greater than 0."
  printf "Correct Usage:\n./testFile.sh [virusesFile] [countriesFile] [numLines] [duplicatesAllowed]\n"
  exit 3
fi

if [[ ($lines -gt 10000 && $duplicates -eq 0) ]]; then
  echo "Can't produce more than 10K unique IDs with the restriction of 4-digits"
  printf "Correct Usage:\n./testFile.sh [virusesFile] [countriesFile] [numLines] [duplicatesAllowed]\n"
  exit 4
fi

echo "Input was: $1, $2, $3, $4"

################### Check Input

start=$(date +%s.%N) # Start clock

################### Get the file records data

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

# First names file
declare -a firstNamesArray;
file="$firstNameFile"
while read line; do firstNamesArray+=("$line"); done < $file
printf "First names:\t %d\n" $((${#firstNamesArray[@]}))

# Last names file
declare -a lastNamesArray;
file="$lastNameFile"
while read line; do lastNamesArray+=("$line"); done < $file
printf "Last names:\t %d\n" $((${#lastNamesArray[@]}))

################### Get the file records data


################### Declare variables that we'll use

# Check if output file already exists and if so, delete it
[[ -f "$outputFile" ]] && rm -f $outputFile
touch $outputFile

# Array of unused IDs
declare -a unusedIdArray;
# Array that keeps |ID-firstName-lastName-country-age| of all original records
declare -A originalRecords;
# An original record is one that its info was not copied from another record
availableIdAmount=$maxID

if [[ $duplicates -eq 0 ]]; then
  # Fill up with all possible unused IDs (0-9999)
  for ((i=0; i<$maxID; i++)) do unusedIdArray+=($i); done
else
  # Generate the first original record
  id=$(($RANDOM%$maxID))
  firstName=${firstNamesArray[(($RANDOM % ${#firstNamesArray[@]}))]}
  lastName=${lastNamesArray[(($RANDOM % ${#lastNamesArray[@]}))]}
  country=${countryArray[(($RANDOM % ${#countryArray[@]}))]}
  age=$(($RANDOM%120+1))
  # Store the whole record info in the associative array for future duplication
  originalRecords+=([id0]=$id [name0]=$firstName [surname0]=$lastName [country0]=$country [age0]=$age)
  originalNo=1
fi

################### Declare variables that we'll use


################### Start Main Loop

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
    if [[ "$((RANDOM%100))" -gt $duplicateRatio ]]; then
    ### Case: Generate an original record
      id=$(($RANDOM%$maxID))
      firstName=${firstNamesArray[(($RANDOM % ${#firstNamesArray[@]}))]}
      lastName=${lastNamesArray[(($RANDOM % ${#lastNamesArray[@]}))]}
      country=${countryArray[(($RANDOM % ${#countryArray[@]}))]}
      age=$(($RANDOM%120+1))
      # Store the whole record info in the associative array for future duplication
      originalRecords+=([id$originalNo]=$id [name$originalNo]=$firstName [surname$originalNo]=$lastName [country$originalNo]=$country [age$originalNo]=$age)
      let "originalNo++"
    else
    ### Case: Generate a duplicate record
      index=$(($RANDOM%$originalNo))
      id=${originalRecords[id$index]}
      # Find the key of the record-to-copy by its ID
      for k in "${!originalRecords[@]}"; do
        [[ ${k::2} == "id" ]] && [[ ${originalRecords[$k]} == $id ]] && key=${k:2}
        # Parse the key from the original record that has the ID we want.
        # For IDs<120 we might have collision of keys when $id==$age_key
        # among the record info, so we need to parse the key apropriately
      done
      firstName=${originalRecords[name$key]}
      lastName=${originalRecords[surname$key]}
      country=${originalRecords[country$key]}
      age=$((${originalRecords[age$key]}))
    fi
  fi

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

################### End Main Loop