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

echo "running clang-format on files: $@"
clang-format -i --style=file "$@"
