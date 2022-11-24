#!/bin/bash

# Trigger all your test cases with this script
# USYD CODE CITATION ACKNOWLEDGEMENT
# I declare that the following lines of code have been copied from the
# tutorial solutions of Week 8 with only minor changes and it is not my own work. 

# Tutorial solutions for week 8 from INFO1110 course
# https://edstem.org/courses/4000

echo "##########################"
echo "### Running e2e tests! ###"
echo "##########################"
echo
count=0 # number of test cases run so far

for folder in `ls -d tests/*/ | sort -V`; do
    name=$(basename "$folder")
    
    echo Running test $name.

    args_file=tests/$name/$name.x2017
    expected_file=tests/$name/$name.out
    
    ./vm_x2017 $args_file | diff - $expected_file && echo "Test $name: passed!"|| echo "Test $name: failed!"
    echo

    count=$((count+1))
done

echo "Finished running $count tests!"