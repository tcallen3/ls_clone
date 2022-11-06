#!/bin/sh

# Tests are based on the suite from: 
# https://stevens.netmeister.org/631/ls-test

# Edge-case examples can be generated from a related script:
# https://stevens.netmeister.org/631/makelstest

# get full path to binary
MY_LS=`readlink -f ../bin/ls`

DIR=$1
if [ -z "${1}" ]; then
	echo "No argument supplied, setting test dir to ."
	DIR="."
fi

TSYS="/tmp/sys_ls"
TMINE="/tmp/my_ls"

OPTIONS="${DIR}
-la ${DIR}
-lai ${DIR}
-d ${DIR}
-d .
-d . .. /
-n /home
-lsh ${DIR}
-lF ${DIR}
-A ${DIR}
-w ${DIR}
-q ${DIR}"

IFS="
"

for opt in ${OPTIONS}
do
	echo "testing options: ${opt}"
	COM1="ls ${opt} | sed /total/d > ${TSYS}"
	COM2="${MY_LS} ${opt} > ${TMINE}"
	sh -c "eval ${COM1}"
	sh -c "eval ${COM2}"
	diff -bq ${TSYS} ${TMINE} || echo "ls ${opt} failed"
done

# system ls uses weird whitespace for char/block files
WS_OPTIONS="-l /dev"

BLOCK_OPTIONS=""

TZ_OPTIONS=""

# recursive timeout test?


