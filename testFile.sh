#!/bin/bash

################### Defined values

duplicateRatio=50 # %
vaccinateRatio=50 # %
outputFile=citizenRecordsFile
# Dictionary files for viruses, countries, first and last names
virusNameFile=virusesFile
countryNameFile=countriesFile
firstNameFile=firstNames
lastNameFile=lastNames
# Counters for dictionary of first and last names in case there's no file
firstNamesNumber=100
lastNamesNumber=100
# Above 10000, we can't ensure unique IDs
maxUniqueID=10000


################### Function to write records to the output file

insertRecord () {  # ID firstName lastName country age virusName status
  # Check if we need to add a date
  if [[ "$7" == "YES" ]]; then
    dd=$(($RANDOM%30+1)); mm=$(($RANDOM%12+1)); yyyy=$(($RANDOM%21+2000+1))
    date="$dd-$mm-$yyyy"
    # Append the record to the output file
    echo $1 $2 $3 $4 $5 $6 $7 $date >> $outputFile
  else
    # Append the record to the output file without date
    echo $1 $2 $3 $4 $5 $6 $7 >> $outputFile
  fi
}


################### Get input arguments

for arg in "$@"; do
  index=$(echo $arg | cut -f1 -d=)
  val=$(echo $arg | cut -f2 -d=)
  case $index in
    $virusNameFile) virusList=$val;;
    $countryNameFile) countryList=$val;;
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

echo "Input was: $1, $2, $3, $4"

if [[ $inputOk == false ]]; then
  printf "Correct Usage:\n./testFile.sh [$virusNameFile] [$countryNameFile] [numLines] [duplicatesAllowed]\n"
  exit 1
fi

################### Check Input - End

start=$(date +%s.%N) # Start clock

################### Records' data seed arrays - Start

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

declare -a firstNamesArray;
if [ -f "$firstNameFile" ]; then
  # Found the first names file
  file="$firstNameFile"
  while read line; do firstNamesArray+=("$line"); done < $file
else
  echo "First names file not found, generating random values"
  for ((i=0; i<$firstNamesNumber; i++)) do
    length=$(($RANDOM%10+3)) # Random length of 3-12 characters
    firstNamesArray+=("$(tr -dc A-Z </dev/urandom | head -c $length)")
  done
fi
printf "First names:\t %d\n" $((${#firstNamesArray[@]}))

declare -a lastNamesArray;
if [ -f "$lastNameFile" ]; then
  # Found the last names file
  file="$lastNameFile"
  while read line; do lastNamesArray+=("$line"); done < $file
else
  echo "Last names file not found, generating random values"
  for ((i=0; i<$lastNamesNumber; i++)) do
    length=$(($RANDOM%10+3)) # Random length of 3-12 characters
    lastNamesArray+=("$(tr -dc A-Z </dev/urandom | head -c $length)")
  done
fi
printf "Last names:\t %d\n" $((${#lastNamesArray[@]}))

################### Records' data seed arrays - End


################### Main Program

# Check if output file already exists and if so, delete it
[[ -f "$outputFile" ]] && rm -f $outputFile
touch $outputFile

# Array of unused IDs
declare -a unusedIdsArray;
availableIdAmount=$maxUniqueID

if [[ $duplicates -eq 0 ]]; then ### Case: Avoid duplicate IDs

  # Fill up the unused ID array with all possible unused IDs (0-9999)
  for ((i=0; i<$maxUniqueID; i++)) do unusedIdsArray+=($i); done

  # Figure out if requested lines are more than the unique IDs
  [[ $lines -gt $maxUniqueID ]] && loops=$maxUniqueID || loops=$lines

  for ((counter=0; counter<$loops; counter++)) do
    # Find a random index in the unused IDs array and claim its value
    index=$(($RANDOM%$availableIdAmount))
    id=${unusedIdsArray[$index]}
    # After claiming this ID, we can swap it with the last one in the array
    # and reduce the size of the array virtually by 1
    unusedIdsArray[$index]=${unusedIdsArray[$availableIdAmount-1]}
    let "availableIdAmount--"
    # Generate the rest info
    firstName=${firstNamesArray[(($RANDOM % ${#firstNamesArray[@]}))]}
    lastName=${lastNamesArray[(($RANDOM % ${#lastNamesArray[@]}))]}
    country=${countryArray[(($RANDOM % ${#countryArray[@]}))]}
    age=$(($RANDOM%120+1))    
    # The virus, vaccination status and date are irrelevant with the above
    # computations and are initialized here, independently and randomly.
    virusName=${virusArray[(($RANDOM % ${#virusArray[@]}))]}
    # Decide for a vaccination certificate based on the defined possibility
    [[ $(($RANDOM%100)) < $vaccinateRatio ]] && status="YES" || status="NO"

    insertRecord $id $firstName $lastName $country $age $virusName $status
  done

  # If we have to produce more records than the unique IDs
  # we will fill up the rest with duplicates of the first 10K records
  [[ $loops -eq $lines ]] && remainingLoops=0 || remainingLoops=$(($lines-$maxUniqueID))
  for ((counter=0; counter<$remainingLoops; counter++)) do
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
    # The virus, vaccination status and date are irrelevant with the above
    # computations and are initialized here, independently and randomly.
    virusName=${virusArray[(($RANDOM % ${#virusArray[@]}))]}
    # Decide for a vaccination certificate based on the defined possibility
    [[ $(($RANDOM%100)) < $vaccinateRatio ]] && status="YES" || status="NO"

    insertRecord $id $firstName $lastName $country $age $virusName $status
  done

else ### Case: Cause duplicate IDs on purpose based on defined possibility

  for ((counter=0; counter<$lines; counter++)) do
    if [[ "$((RANDOM%100))" -gt $duplicateRatio || $counter -lt 1 ]]; then
    ### Case: Generate an original record
      # All variables here are initialized randomly
      id=$(($RANDOM%$maxUniqueID))
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
    # The virus, vaccination status and date are irrelevant with the above
    # computations and are initialized here, independently and randomly.
    virusName=${virusArray[(($RANDOM % ${#virusArray[@]}))]}
    # Decide for a vaccination certificate based on the defined possibility
    [[ $(($RANDOM%100)) < $vaccinateRatio ]] && status="YES" || status="NO"

    insertRecord $id $firstName $lastName $country $age $virusName $status
  done

fi

end=$(date +%s.%N) # Stop clock
runningTime=$(echo "$end - $start" | bc)
echo "Script runned for" $runningTime "seconds"
echo "Output stored in" $outputFile