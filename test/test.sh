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

# WARNING: these tests will fail if you run them on a directory with
# block/character files (see note below on ls formatting), but this
# should be solely due to whitespace changes
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
-q ${DIR}
-lks ${DIR}
/ /tmp ~ ."

IFS="
"

save_cmd_output()
{
	COM1="ls ${1} | sed /total/d | sed /^$/d > ${TSYS}"
	COM2="${MY_LS} ${1} > ${TMINE}"
	sh -c "eval ${COM1}"
	sh -c "eval ${COM2}"
}

for opt in ${OPTIONS}
do
	echo "Running test: ls ${opt}"
	save_cmd_output ${opt}
	# if you want to run all tests on block/char device files, change
	# -b to -w in the diff
	diff -bq ${TSYS} ${TMINE} || echo "ls ${opt} failed"
done

# tests where other whitespace is involved (\t,\n,...), split out so
# we can run previous tests on cases with weird nonpriting WS in filenames
# and not ignore mismatches between ls and our code
save_cmd_output "-l /dev"
diff -bw ${TSYS} ${TMINE} || echo "ls ${opt} failed"

env_cmd_output()
{
	COM1="${1} ls ${2} 2>/dev/null | sed /total/d | sed /^$/d > ${TSYS}"
	COM2="${1} ${MY_LS} ${2} 2>/dev/null > ${TMINE}"
	sh -c "eval ${COM1}"
	sh -c "eval ${COM2}"
}

# these require setting BLOCKSIZE variable while they run
BLOCK_OPTIONS="BADVAL
0
2048
-25
50"

for opt in ${BLOCK_OPTIONS}
do
	echo "Running test with BLOCKSIZE=${opt}"
	env_cmd_output "BLOCKSIZE=${opt}" "-ls ."
	diff -qb ${TSYS} ${TMINE} || echo "BLOCKSIZE=${opt} failed"
done

# these require setting TZ variable while they run
TZ_OPTIONS="EST5
PST8PDT
BADVAL"

for opt in ${TZ_OPTIONS}
do
	echo "Running test with TZ=${opt}"
	env_cmd_output "TZ=${opt}" "-ls ."
	diff -qb ${TSYS} ${TMINE} || echo "BLOCKSIZE=${opt} failed"
done

# recursive timeout test?


