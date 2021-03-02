#! /bin/bash

################### Defined values

duplicateRatio=50 # 50%
vaccinateRatio=50 # 50%
outputFile="citizenRecordsFile"

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

if [ -f "$virusList" ]; then
  :
else
  echo "Virus file not found."
  printf "Correct Usage:\n./testFile.sh [virusesFile] [countriesFile] [numLines] [duplicatesAllowed]\n"
  exit 1
fi

if [ -f "$countryList" ]; then
  :
else
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

START=$(date +%s.%N) # Start clock

################### Get length of file records

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
file="firstNames"
while read line; do
firstNamesArray+=("$line");
done < $file
printf "First names:\t %4d\n" $((${#firstNamesArray[@]}))

# Last names file
declare -a lastNamesArray;
file="lastNames"
while read line; do
lastNamesArray+=("$line");
done < $file
printf "Last names:\t %4d\n" $((${#lastNamesArray[@]}))

################### Get length of file records


################### Prepare for main loop

# Check if file already exists
if [ -f "$outputFile" ]; then
rm -f $outputFile
fi
touch $outputFile

# We accept IDs up to 4 digits thus ID<10000
# Above that, we can't ensure unique IDs
maxID="$(($lines < 10000 ? $lines : 10000))"
# Array of existing IDs in record file
declare -a usedIdArray;
# Array of unused IDs.
declare -a unusedIdArray;
# Fill up with all unused IDs
for ((it=0; it<lines; it++)) do
  unusedIdArray+=($it)
done

################### Prepare for main loop


################### Main Loop

for ((counter=lines; counter>0 ;counter--)) do

  ### Case: Avoid duplicate IDs
  if [[ $duplicates -eq 0 ]]; then
    # Find a random index in the remaining IDs array and claim its value
    id=${unusedIdArray[ $(($RANDOM % ${#unusedIdArray[@]})) ]}
  else
  ### Case: Cause duplicate IDs on purpose
    # Decide to generate a duplicate based on the defined possibility
    # ONLY IF there's at least one record already
    if [[ "$((RANDOM%100 + 1))" -lt $duplicateRatio && ${#usedIdArray[@]} -gt 0 ]]; then
      id=${usedIdArray[ $RANDOM % ${#usedIdArray[@]} ]}
    else
      id=$(($RANDOM%$maxID)) # Only used for first record
    fi
  fi

  # Store in the usedIdArray so we know this ID is used
  usedIdArray+=($id)

  firstName=${firstNamesArray[(($RANDOM % ${#firstNamesArray[@]}))]}
  lastName=${lastNamesArray[(($RANDOM % ${#lastNamesArray[@]}))]}
  country=${countryArray[(($RANDOM % ${#countryArray[@]}))]}
  age=$(($RANDOM%120 + 1))
  virus=${virusArray[(($RANDOM % ${#virusArray[@]}))]}

  # Decide for a vaccination certificate based on the defined possibility
  [[ $(($RANDOM%100 + 1)) < $vaccinateRatio ]] && status="YES" || status="NO"

  # Check if we need to add a date
  if [ "$status" = "YES" ]; then
    let "dd=$RANDOM%30 + 1"
    let "mm=$RANDOM%12 + 1"
    let "yyyy=$RANDOM%22 +2000"
    date="$dd-$mm-$yyyy"
    # Append the record to the output file
    printf "$id"" ""$firstName"" ""$lastName"" ""$country"" ""$age"" ""$virus"" ""$status"" ""$date\n" >> $outputFile
  else
    # Append the record to the output file without date
    printf "$id"" ""$firstName"" ""$lastName"" ""$country"" ""$age"" ""$virus"" ""$status\n" >> $outputFile
  fi

done

END=$(date +%s.%N) # Stop clock
DIFF=$(echo "$END - $START" | bc)
echo "Script runned for" $DIFF "seconds"

################### End Main Loop