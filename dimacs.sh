#!/bin/bash

# Generates directed graphs!
# DIMACS filer: dimacs.sh -n 100 -m 1000 -w 0 1000
# TODO: Let all nodes be connected
# TODO: Find out how to fordel antal kanter lige til alle knuder (kan gøres ved at anvende kvotienten på alle knuder?)

n=0
m=0
WMIN=-1
WMAX=-1

while getopts n:m:w: option
do
 case "${option}"
 in
     n) n=${OPTARG};;
     m) m=${OPTARG};;
     w) WMIN=${OPTARG};;
 esac
done
WMAX=$7
if [[ $n -le 0 || $m -le 0 || $WMIN -lt 0 || $WMAX -lt 0 ]]; then
    echo "How to run: ./dimacs.sh -n 100 -m 1000 -w 0 200"
    exit
fi
# directed graph formula (https://en.wikipedia.org/wiki/Dense_graph)
tmp=`expr $n - 1`
tmp2=`expr $n \* $tmp`
# scale indicates number of floating points
density=$(echo "scale=5; $m/$tmp2" | bc)
FILENAME="n=$n"-"m=$m"-"d=$density"
# Find out which num. of occurence it is
i=0
while [[ -e $i-$FILENAME.txt ]]; do
    let i++
done
name=$i-$FILENAME.txt
echo "p sp $n $m" > $name
echo "c " >> $name

declare -a assigned
declare -a adjmatrix
for (( i=0; i<$n; i++ )); do
    for (( j=0; j<$n; j++ )); do
        adjmatrix[$i,$j]=0
    done
done
mleft=$m
beg=1
RANDOM=$$
for (( i=0; i<$n; i++ )); do
    # picks a random node to connect to
    node=$(shuf -i 0-$n -n 1)
    if [[ assigned[$i] -ne 1 && $node -ne $i && ${adjmatrix[$i, $node]} -ne 1
        && ${adjmatrix[$node, $i]} -ne 1 ]]; then
        adjmatrix[$i,$node]=1
        adjmatrix[$node,$i]=1
        assigned[$i]=1
        assigned[$node]=1
        let mleft--
    fi
    if [[ $mleft -le 0 ]]; then
        exit
    fi
    # gen. beg-n, gen. how many edges for the node i
    DIFF=$(($n-$beg+1))
    edges=$(($(($RANDOM%$DIFF))+$beg))
    for (( j=0; j<$edges; j++ )); do
        # echo "$mleft $i $edges $j"
        # picks a random node to connect to
        node=$(shuf -i 0-$n -n 1)
        while [[ ${adjmatrix[$i, $node]} -eq 1 ]]; do
            node=$(shuf -i 0-$n -n 1)
        done
        if [[ $node -ne $i && ${adjmatrix[$i,$node]} -ne 1 && ${adjmatrix[$node,$i]} -ne 1 ]]; then
            adjmatrix[$i,$node]=1
            adjmatrix[$node,$i]=1
            assigned[$i]=1
            assigned[$node]=1
            let mleft--
        fi
    done
done

# Printing the matrix out with edges
for (( i=0; i<$n; i++ )); do
    for (( j=0; j<$n; j++ )); do
        if [[ ${adjmatrix[$i, $j]} -eq 1 ]]; then
            weight=$(shuf -i $WMIN-$WMAX -n 1)
            tmp=`expr $i + 1`
            tmp2=`expr $j + 1`
            echo "a $tmp $tmp2 $weight" >> $name
            echo "a $tmp2 $tmp $weight" >> $name
        fi
    done
done
        
