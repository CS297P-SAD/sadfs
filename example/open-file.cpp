#include <iostream>
#include <fstream>

using namespace std;

/*
* Steps to test getattr()
* 1. Run dummy-sadmd
* 2. Create an empty directory where the filesystem should be mounted
*    for e.g. /mnt/gladfs
* 3. In a different window, run sadfsd-bootstrap with following conf:
*    "port=6666 ipaddress=127.0.0.1 mountpoint=/mnt/gladfs/"
* 4. In a different window, run open-file
* 5. When done, on Linux "fusermount -u /mnt/gladfs"
* 		on macOS "umount /mnt/gladfs"
* 6. Stop dummy-sadmd process
*/

int main()
{
	fstream my_file;
	my_file.open("/mnt/gladfs/hello", ios::in);
	if (!my_file) {
		cout << "File not open";
	}
	else
	{
		cout << "File opened";
		
	}

	return 0;
}
