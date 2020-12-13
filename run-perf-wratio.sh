#!/bin/bash

ORDERINGS="no-ordering fast-fair full-ordering"
WRATIOS="0 25 50 75 100"
NUM_KEYS=100000


sudo quartz/scripts/setupdev.sh load


for ORDERING in ${ORDERINGS}; do
    cd single-${ORDERING}/

    make clean && make

    for WRATIO in ${WRATIOS}; do
        echo "--> Running ${ORDERING} with write ratio ${WRATIO}..."

        # Run with PAPI
        sudo ./btree -n ${NUM_KEYS} \
            -i ../inputs/ycsb-w${WRATIO}-${NUM_KEYS}.txt \
            > ../results/${ORDERING}-w${WRATIO}-${NUM_KEYS}-pmc.txt

        # Run over quartz
        sudo ../quartz/scripts/runenv.sh ./btree -n ${NUM_KEYS} \
            -i ../inputs/ycsb-w${WRATIO}-${NUM_KEYS}.txt -q \
            > ../results/${ORDERING}-w${WRATIO}-${NUM_KEYS}-lat.txt

        sleep 1
    done

    cd ..
done
