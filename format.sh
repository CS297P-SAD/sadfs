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
INLINE_FLAG=${INLINE:+"-i"}

echo "running command: clang-format ${INLINE_FLAG} ${STYLE_FLAG} $@"
clang-format ${INLINE_FLAG} ${STYLE_FLAG} "$@"
