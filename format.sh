#! /bin/sh

if ! command -v clang-format 1>/dev/null ; then
	echo "clang-format unavailable!"
	exit 1
fi

if [ ! -f .clang-format ] ; then
	echo ".clang-format unavailable!"
	exit 1
fi

if [ ! "$#" -gt 0 ] ; then
	echo "input files missing"
	exit 1
fi

STYLE_FLAG="--style=file"
INPLACE_FLAG=${INPLACE:+"-i"}

echo "running command:\nclang-format ${INPLACE_FLAG} ${STYLE_FLAG} $@"
clang-format ${INPLACE_FLAG} ${STYLE_FLAG} "$@"
