#!/bin/sh

if [ -d "$1-$2-$3" ]; then
    cd "$1-$2-$3"

    tred "inclusion_tree_$1-$2-$3.gv" > "inclusion_tree_$1-$2-$3_reduced.gv"
    dot -Tsvg "inclusion_tree_$1-$2-$3_reduced.gv" > "inclusion_tree_$1-$2-$3.svg"

    tred "rm_inclusion_tree_$1-$2-$3.gv" > "rm_inclusion_tree_$1-$2-$3_reduced.gv"
    dot -Tsvg "rm_inclusion_tree_$1-$2-$3_reduced.gv" > "rm_inclusion_tree_$1-$2-$3.svg"
else
    echo "Directory $1-$2-$3 does not exist. You should run gen_rmc.sh script to create and fill it."
    exit 1;
fi

exit 0;
