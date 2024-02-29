#/bin/bash

if [ $# -ne 2 ]; then
    echo "Invalid number of arguments. Please provide political parties filename and number of lines"
else
    filename=$1
    n=$2

    if [ $n -le 0 ]; then
        echo "Number of lines must be positive"
        exit 1
    fi

    if [ ! -f $1 ]; then
        echo "File $1 not found"
        exit 1
    fi 

    if [ ! -r $1 ]; then
        echo "File $1 not readable"
        exit 1
    fi 

    lines=$(wc -l $1 | cut -d' ' -f1)

    echo "Total lines  : $n"    
    echo "Filename     : $filename"
    echo "Total parties: $lines"

    if [ -e inputFile.txt ]; then
        rm inputFile.txt
    fi
    touch "inputFile.txt"

    for (( c=1; c<=$n; c++ )); do
        length=$((RANDOM%9+3))	
        firstname=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $length | head -n 1)
        lastname=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $length | head -n 1)   

        line_no=$((RANDOM%($lines)+1))	

        party=$(cat $1 | head -$line_no | tail -1)

        echo "$firstname $lastname $party $line" >> inputFile.txt
    done

fi