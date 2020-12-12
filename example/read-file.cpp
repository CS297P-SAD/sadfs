#include <iostream>
#include <fstream>

using namespace std;

/*
* Steps to test getattr()
* 1. Run dummy-sadmd
* 2. In a different window, run sadcd-bootstrap with PATH set, following conf:
*    port=6969 ipaddress=127.0.0.1 master_port=6666 master_ipaddress=127.0.0.1
     serverid=dbbf47d9-afe5-4149-97ef-91f9873e27ff
* 3. Create an empty directory where the filesystem should be mounted
*    for e.g. /mnt/gladfs
* 4. In a different window, run sadfsd-bootstrap with following conf:
*    port=6666 ipaddress=127.0.0.1 mountpoint=/mnt/gladfs/
* 5. In a different window, run read-file
* 6. When done, on Linux "fusermount -u /mnt/gladfs"
* 		on macOS "umount /mnt/gladfs"
* 7. Stop dummy-sadmd and sadcd
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
		cout << "File opened\n";
		/*string tp;
		const uint off = 0;*/
		char* buf = new char[100];
		my_file.read(buf, 11);
		printf("%s\n", buf);	
		cout << "Read success!\n";
	}

	return 0;
}
