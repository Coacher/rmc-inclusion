#!/bin/sh

if [ -d "$1-$2-$3" ]; then
    rm -r "$1-$2-$3"
fi

mkdir "$1-$2-$3" && cd "$1-$2-$3"

../rmc -p $1 -l $2 -L $3 -IGR || exit 1;

tred "inclusion_tree_$1-$2-$3.gv" > "inclusion_tree_$1-$2-$3_reduced.gv"
dot -Tsvg "inclusion_tree_$1-$2-$3_reduced.gv" > "inclusion_tree_$1-$2-$3.svg"

tred "rm_inclusion_tree_$1-$2-$3.gv" > "rm_inclusion_tree_$1-$2-$3_reduced.gv"
dot -Tsvg "rm_inclusion_tree_$1-$2-$3_reduced.gv" > "rm_inclusion_tree_$1-$2-$3.svg"

exit 0;
