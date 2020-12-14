#!/bin/bash

ORDERINGS="no-ordering fast-fair full-ordering"
NUM_THREADS="1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16"
NUM_KEYS=2000000


sudo quartz/scripts/setupdev.sh load


for ORDERING in ${ORDERINGS}; do
    cd concurrent-${ORDERING}/

    make clean && make

    for THREADS in ${NUM_THREADS}; do
        echo "--> Running ${ORDERING} with ${THREADS} threads..."

        # Run over quartz
        sudo ../quartz/scripts/runenv.sh ./btree_concurrent -n ${NUM_KEYS} \
            -i ../inputs/ycsb-zipfian-${NUM_KEYS}.txt -q -c 15 -t ${THREADS} \
            > ../results/${ORDERING}-t${THREADS}-${NUM_KEYS}.txt

        sleep 1
    done

    cd ..
done
