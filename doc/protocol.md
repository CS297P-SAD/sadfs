| Action | From | To  | Param1 | Param2 | Param3 | Param4 | 
| :----: | :--: | :---: | :----: | :----: | :----: | :----: |
| Write Request | sadfsd | sadmd | filename | offset | |
| Grant Write Request | sadmd | sadfsd | chunkid | chunkserverIPs |
| Reject Write Request | sadmd | sadfsd | filename | offset |
| Write To Chunk | sadfsd,sadcd | sadcd | data | offset | chunkid | chunkserverIPs |
| Notify Master Of Chunk | sadcd | sadmd | chunkid |
| Read Request | sadfsd | sadmd | filename |
| Grant Read Request | sadmd | sadfsd | chunkserverIP | chunkid |
| Read Chunk | sadfsd | sadcd | chunkid |
| Chunk Contents | sadcd | sadfsd | data |
| Join Request | sadcd | sadmd | mem_available |
| Reject Join Request | sadmd | sadcd |
| Accept Join Request | sadmd | sadcd |
