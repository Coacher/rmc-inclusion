#!/bin/sh

sh gen_rmc.sh ../build/src $1 $2 $3 && sh gen_graph.sh $1 $2 $3
