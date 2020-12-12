#!/ bin / sh

if [ ! $# -eq 2 ] ; then
	echo 2 and only 2 arguments must be passed
	echo read "$0" for more information
	exit 1
fi

if [ ! -x "$1" ] ; then
	echo "$1" not an executable
fi

BIN="${PWD}/$1"

mkdir -p "$2" && cd "$2"

mkdir 229217b9-6bf0-47f0-9752-64541a99a67a
cd    229217b9-6bf0-47f0-9752-64541a99a67a
${BIN} 1
cd../

mkdir bf5b432c-3889-4484-aa3c-77a08d30901b
cd    bf5b432c-3889-4484-aa3c-77a08d30901b
${BIN} 5
cd../

mkdir 6cfe7de1-b508-4e26-864e-7887fbc700ef
cd    6cfe7de1-b508-4e26-864e-7887fbc700ef
${BIN} 2
cd../

mkdir d7e30bce-1c62-4b9a-b88b-c9d2632142a7
cd    d7e30bce-1c62-4b9a-b88b-c9d2632142a7
${BIN} 6
cd../

mkdir d7e30bce-1c62-4b9a-b88b-c9d2632142a7
cd    d7e30bce-1c62-4b9a-b88b-c9d2632142a7
${BIN} 7
cd../

mkdir 8cf03844-5a57-47e6-ab60-81f2de556aa4
cd    8cf03844-5a57-47e6-ab60-81f2de556aa4
${BIN} 5
cd../
