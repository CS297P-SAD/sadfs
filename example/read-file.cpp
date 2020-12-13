#include <iostream>
#include <fstream>

/*
* Steps to test getattr()
* 1. Run dummy-sadmd
* 2. In a different window, run sadcd-bootstrap with PATH set, following conf:
*    port=6969 ipaddress=127.0.0.1 master_port=6666 master_ipaddress=127.0.0.1
     serverid=dbbf47d9-afe5-4149-97ef-91f9873e27ff
* 3. Create an empty directory where the filesystem should be mounted
*    for e.g. /home/user/Documents/gladfs/
* 4. In a different window, run sadfsd-bootstrap with following conf:
*    port=6666 ipaddress=127.0.0.1 mountpoint=/home/user/Documents/gladfs/
* 5. In a different window, run read-file
* 6. When done, on Linux "fusermount -u /home/user/Documents/gladfs"
* 		on macOS "umount /home/user/Documents/gladfs"
* 7. Stop dummy-sadmd and sadcd
*/

int main()
{
	std::fstream my_file;
	my_file.open("/home/shu-g/Documents/gladfs/hello", std::ios::in);
	if (!my_file) {
		std::cout << "File not open";
	}
	else
	{
		std::cout << "File opened\n";
		char* buf = new char[100];
		my_file.read(buf, 11);
		printf("%s\n", buf);	
		std::cout << "Read success!\n";
	}

	return 0;
}
