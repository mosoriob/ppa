#!/bin/bash

    for j in 1 2 3 4; do
for i in 10000  1000000000000; do 
    (time ./wave $i $j 10000 1)  2>&1 > /dev/null;
    done
done

