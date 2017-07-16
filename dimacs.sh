#!/bin/bash

# Generates undirected graphs in DIMACS ssp file format!
# DIMACS filer: dimacs.sh -n 100 -d 0.1 -w 100 1000

n=0
d=0.0
WMIN=-1
WMAX=-1

while getopts n:d:w: option
do
 case "${option}"
 in
     n) n=${OPTARG};;
     d) d=${OPTARG};;
     w) WMIN=${OPTARG};;
 esac
done
WMAX=$7
# scale indicates number of floating points
comp=$(echo "scale=2; 0.0" | bc)
null=0.0
comp=$(echo "scale=2; $d <= $null" | bc)

if [[ $n -le 0 || $comp -eq 1 || $WMIN -lt 0 || $WMAX -lt 0 || $WMAX -le $WMIN ]]; then
    echo "Error! Try again! Remember:"
    echo "How to run: ./dimacs.sh -n 100 -d 0.1 -w 100 1000"
    echo "n > 0"
    echo "d > 0.0"
    echo "maximum weight needs to be >= than minimum weight"
    exit
fi
tmp=`expr $n - 1`
tmp2=`expr $n \* $tmp`
m=$(echo "scale=2; $tmp2 * $d" | bc)
m=$(echo "$m/1" | bc)

FILENAME="n=$n-m=$m-d=$d".txt
# Find out which num. of occurence it is
i=0
while [[ -e graphs/"$i-$FILENAME" ]]; do
    let i++
done
name="$i-$FILENAME"
make rebuild
# run prog
./dimacs $n $d $WMIN $WMAX $name
DIRECTORY=graphs
if [ ! -d "$DIRECTORY" ]; then
  mkdir $DIRECTORY
fi
mv $name $DIRECTORY
