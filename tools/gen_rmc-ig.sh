#!/bin/sh

if [ -d "$1" ]; then
    if [ -x "$1/rmc-ig" ]; then
        if [ ! -d "$2-$3-$4" ]; then
            mkdir "$2-$3-$4"
        fi

        cd "$2-$3-$4" && "../$1/rmc-ig" -p $2 -l $3 -L $4 -IGR || exit 1;
    else
        echo "$1/rmc-ig is not found. You should build it."
        exit 1;
    fi
else
    echo "Directory $1 does not exist."
    exit 1;
fi

exit 0;
