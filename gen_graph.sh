#!/bin/sh

cd "$1-$2-$3"

dot -Tsvg "inclusion_tree_$1-$2-$3.gv" > "inclusion_tree_$1-$2-$3.svg"

tred "rm_inclusion_tree_$1-$2-$3.gv" > "rm_inclusion_tree_$1-$2-$3_reduced.gv"
dot -Tsvg "rm_inclusion_tree_$1-$2-$3_reduced.gv" > "rm_inclusion_tree_$1-$2-$3.svg"

exit 0;
