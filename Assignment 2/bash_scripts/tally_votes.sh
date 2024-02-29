#/bin/bash

if [ $# -ne 1 ]; then
    echo "Invalid number of arguments. Please provide tally results file"
else
    inputFile=inputFile.txt
    outputFile=$1

    echo "Input: $inputFile"    
    echo "Output: $outputFile"    
    
    if [ ! -f $inputFile ]; then
        echo "File $inputFile not found"
        exit 1
    fi 

    if [ ! -r $inputFile ]; then
        echo "File $inputFile not readable"
        exit 1
    fi 

    cat $inputFile | cut -d' ' -f3 | sort | uniq -c | awk '{ print $2 " " $1 }' > $1
fi