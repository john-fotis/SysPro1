#! /bin/bash

################### Defined values

duplicateRatio=10 # 10%
vaccinateRatio=50 # 50%
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

# Virus file
declare -a virusArray;
file="$virusList"
while read line; do
virusArray+=("$line");
done < $file
printf "Virus Records:\t %2d\n" $((${#virusArray[@]}))

# Country file
declare -a countryArray;
file="$countryList"
while read line; do
countryArray+=("$line");
done < $file
printf "Country Records: %3d\n" $((${#countryArray[@]}))

# First names file
declare -a firstNamesArray;
file="$firstNameFile"
while read line; do
firstNamesArray+=("$line");
done < $file
printf "First names:\t %4d\n" $((${#firstNamesArray[@]}))

# Last names file
declare -a lastNamesArray;
file="$lastNameFile"
while read line; do
lastNamesArray+=("$line");
done < $file
printf "Last names:\t %4d\n" $((${#lastNamesArray[@]}))

################### Get the file records data


################### Prepare for main loop

# Check if file already exists
if [ -f "$outputFile" ]; then
  rm -f $outputFile
fi
touch $outputFile

# Array of existing IDs in record file
declare -a usedIdArray;
# Array of unused IDs
declare -a unusedIdArray;
if [[ $duplicates -eq 0 ]]; then
  # Fill up with all possible unused IDs (0-9999)
  for ((i=0; i<maxID; i++)) do
    unusedIdArray+=($i)
  done
fi
# Array that keeps ID firstName lastName country age of all records
declare -A associativeRecords;

################### Prepare for main loop


################### Main Loop

for ((counter=0; counter<$lines ;counter++)) do

  if [[ $duplicates -eq 0 ]]; then
  ### Case: Avoid duplicate IDs
    # Find a random index in the remaining IDs array and claim its value
    index=$(($RANDOM % ${#unusedIdArray[@]}))
    id=${unusedIdArray[ $index ]}
    # Remove the indexed ID from the array so it can't be reused
    unusedIdArray=( "${unusedIdArray[@]::$index}" "${unusedIdArray[@]:$((index+1))}" )
    # Generate the rest info
    firstName=${firstNamesArray[(($RANDOM % ${#firstNamesArray[@]} + 1))]}
    lastName=${lastNamesArray[(($RANDOM % ${#lastNamesArray[@]} + 1))]}
    country=${countryArray[(($RANDOM % ${#countryArray[@]} + 1))]}
    age=$(($RANDOM%120 + 1))
  else
  ### Case: Cause duplicate IDs on purpose
    if [[ "$((RANDOM%100 + 1))" -lt $duplicateRatio && ${#usedIdArray[@]} -gt 0 ]]; then
    ### Decide to generate a duplicate based on the defined possibility
    ### ONLY IF there's at least one record already
      id=${usedIdArray[$(($RANDOM%${#usedIdArray[@]}))]}
      # Find the key of the record-to-copy by its ID
      for k in "${!associativeRecords[@]}"; do
        [[ ${associativeRecords[$k]} == $id ]] && key=$k
      done
      # Keep the key of the record in associative array
      # in order to retrieve the rest info with it
      [[ "${key::2}" == "id" ]] && key=${key:2} || key=${key:3}
      # for IDs<120 we might have collision of keys when $id==$age_key
      # in associative array so we need to parse the key apropriately
      firstName=${associativeRecords[name$key]}
      lastName=${associativeRecords[surname$key]}
      country=${associativeRecords[country$key]}
      age=$((${associativeRecords[age$key]}))
    else
    ### Only used for first record - Generate an original record
      id=$(($RANDOM%$maxID))
      firstName=${firstNamesArray[(($RANDOM % ${#firstNamesArray[@]} + 1))]}
      lastName=${lastNamesArray[(($RANDOM % ${#lastNamesArray[@]} + 1))]}
      country=${countryArray[(($RANDOM % ${#countryArray[@]} + 1))]}
      age=$(($RANDOM%120 + 1))
    fi
    # Store in the usedIdArray so we know this ID is used
    usedIdArray+=($id)
    # Store the whole record info in the associative array for future duplication
    associativeRecords+=([id$counter]=$id [name$counter]=$firstName [surname$counter]=$lastName [country$counter]=$country [age$counter]=$age)
  fi

  virus=${virusArray[(($RANDOM % ${#virusArray[@]}))]}

  # Decide for a vaccination certificate based on the defined possibility
  [[ $(($RANDOM%100 + 1)) < $vaccinateRatio ]] && status="YES" || status="NO"

  # Check if we need to add a date
  if [ "$status" = "YES" ]; then
    dd=$(($RANDOM%30 + 1))
    mm=$(($RANDOM%12 + 1))
    yyyy=$(($RANDOM%22 +2000))
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