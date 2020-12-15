#! /bin/sh

echo "Starting logging script..."

while
do
	echo "`date`: something important"
	sleep 2
	echo "`date`: something not so important"
	sleep 2
	echo "`date`: something went wrong..."
	sleep 1
	echo "`date`: and we recovered!"
	sleep 2
done
