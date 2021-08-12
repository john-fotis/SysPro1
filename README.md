# This is the first of the total three assignments for the [Systems Programming Course](http://cgi.di.uoa.gr/~antoulas/k24/) during my B.Sc in Informatics and Telecommunications.

## Description:
This project's main goal is to practice on multiple Data Structures in C or C++ programming language without the use of STL and organise all the database information in such way, as to minimize data-overlap and repetition.

## Requirements:
- Ubuntu-based linux distribution (Developed in Ubuntu 20.04 LTS)
- GCC 7.5.0 or newer
- GNU Make 4.2.1
- Bash shell

## How to run:
You can run everything, in the root path of the project, using the [Makefile](https://github.com/john-fotis/SysPro1/blob/main/Makefile). Type `make help` for all the available options. You can also run both the script and application with the appropriate make rules. All the run parameters can be changed through the [Makefile](https://github.com/john-fotis/SysPro1/blob/main/Makefile).

## Bash-Script: <br/>
  You first need to produce a [citizenRecordsFile](https://github.com/john-fotis/SysPro1/blob/main/citizenRecordsFile) if not already there. For this purpose, you need [virusesFile](https://github.com/john-fotis/SysPro1/blob/main/virusesFile), [countriesFile](https://github.com/john-fotis/SysPro1/blob/main/countriesFile), [firstNames](https://github.com/john-fotis/SysPro1/blob/main/firstNames) and [lastNames](https://github.com/john-fotis/SysPro1/blob/main/lastNames). The names of these files are case-sensitive, though, can be changed in the [testFile.sh](https://github.com/john-fotis/SysPro1/blob/main/testFile.sh). First and Last names dictionaries are non-mandatory, however, if abscent, the script will produce records with random citizen names using only characters A-Z. <br/>

  The above can be executed with:
  1) `make scriptRun` or
  2) `./testFile.sh [virusesFile] [countriesFile] [numLines] [duplicatesAllowed]`

  Notes: **duplicatesAllowed** flag works as boolean for zero and non-zero values.
  - Non-zero means the script will produce duplicate and conflicting records.
  - Zero means the script will produce unique-ID records up to 10K records. After that, it behaves like the non-zero mode. For more unique records, you can increase the **maxID** value in the [script](https://github.com/john-fotis/SysPro1/blob/main/testFile.sh) (currently 9999).

## Application: <br/>
  Firstly, simply compile with `make` or `make all`.
  The application initiates its record database with the [input file](https://github.com/john-fotis/SysPro1/blob/main/citizenRecordsFile). It filters out all duplicate records or records that are incompatible with the assignment's format. After that, its ready to answer a series of requests from the user. You can type `/menu` for all the available options. <br/>
 
  The above can be executed with:
  1) `make run` or
  2) `./vaccineMonitor -c [citizenRecordsFile] –b [bloomSize]`
  
  Notes: All arguments are mandatory, case and order sensitive. Minimum bloomfilter size is 1, optimal 1000 (for the current input file) and maximum tested is 100K bytes. You can also run with `make valgrind` rule. It has been tested for multiple leak types.
 
## Copyright and License: <br/>
&copy; 2021 John Fotis <br/>
This project is licensed under the [MIT License](https://github.com/john-fotis/SysPro2/blob/main/LICENSE.md)
