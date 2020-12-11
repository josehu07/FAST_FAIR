#!/bin/bash

ORDERINGS="no-ordering fast-fair full-ordering"
DATASETS="synth-monotonic synth-uniform synth-reverse"
NUM_KEYS=100000


sudo quartz/scripts/setupdev.sh load


for ORDERING in ${ORDERINGS}; do
    cd single-${ORDERING}/

    make clean && make

    for DATASET in $DATASETS; do
        echo "--> Running ${ORDERING} over ${DATASET}..."

        # Run with PAPI
        sudo ./btree -n ${NUM_KEYS} \
            -i ../inputs/${DATASET}-${NUM_KEYS}.txt \
            > ../results/${ORDERING}-${DATASET}-${NUM_KEYS}-pmc.txt

        # Run over quartz
        sudo ../quartz/scripts/runenv.sh ./btree -n ${NUM_KEYS} \
            -i ../inputs/${DATASET}-${NUM_KEYS}.txt -q \
            > ../results/${ORDERING}-${DATASET}-${NUM_KEYS}-lat.txt

        sleep 1
    done

    cd ..
done
