#!/bin/sh

M_WEIGHT="${4:-1000}"
R_WEIGHT="${5:-1000}"
O_WEIGHT="${6:-1000}"

rm -rf "$1-$2-$3" && \
tools/gen_rmc-short.sh build/src $1 $2 $3 ${M_WEIGHT} ${R_WEIGHT} ${O_WEIGHT} && \
tools/gen_rmc-ig.sh build/src $1 $2 $3 ${M_WEIGHT} ${R_WEIGHT} && \
tools/gen_graph.sh $1 $2 $3
