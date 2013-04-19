#!/bin/sh

if [ -d "$1-$2-$3" ]; then
    cd "$1-$2-$3"

    if [ -f "inclusion_tree_short_$1-$2-$3.gv" ]; then
        dot -Tsvg "inclusion_tree_short_$1-$2-$3.gv" > "inclusion_tree_short_$1-$2-$3.svg"
    fi

    if [ -f "inclusion_tree_$1-$2-$3.gv" ]; then
        tred "inclusion_tree_$1-$2-$3.gv" > "inclusion_tree_$1-$2-$3_reduced.gv"
        dot -Tsvg "inclusion_tree_$1-$2-$3_reduced.gv" > "inclusion_tree_$1-$2-$3.svg"
    fi

    if [ -f "rm_inclusion_tree_$1-$2-$3.gv" ]; then
        tred "rm_inclusion_tree_$1-$2-$3.gv" > "rm_inclusion_tree_$1-$2-$3_reduced.gv"
        dot -Tsvg "rm_inclusion_tree_$1-$2-$3_reduced.gv" > "rm_inclusion_tree_$1-$2-$3.svg"
    fi
else
    echo "Directory $1-$2-$3 does not exist."
    echo "You should run gen_rmc-ig.sh or gen_rmc-short.sh script to create and fill it."
    exit 1;
fi

exit 0;
