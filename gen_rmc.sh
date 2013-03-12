#!/bin/sh

if [ -d "$1-$2-$3" ]; then
    rm -r "$1-$2-$3"
fi

mkdir "$1-$2-$3" && cd "$1-$2-$3"

../rmc -p $1 -l $2 -L $3 -IGR || exit 1;

exit 0;
