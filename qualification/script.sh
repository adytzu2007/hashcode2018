#!/bin/bash

make
for test in *.in; do
    in=${test}
    out=${test%.in}.out
    echo "Running test ${in} and saving output into ${out}"
    ./code < ${in} > ${out}
done

wait
