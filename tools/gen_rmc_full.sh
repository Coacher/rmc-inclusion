#!/bin/sh

rm -rf "$1-$2-$3" && \
tools/gen_rmc-short.sh build/src $1 $2 $3 && \
tools/gen_rmc-ig.sh build/src $1 $2 $3 && \
tools/gen_graph.sh $1 $2 $3
