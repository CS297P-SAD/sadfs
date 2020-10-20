| Action | From | To  | Param1 | Param2 | Param3 | 
| :----: | :--: | :-: | :----: | :----: | :---:  |
| Write Request | sadfsd | sadmd | filename | | |
| Grant Write Request | sadmd | sadfsd | chunkid | chunkserverIP | metadata |
| Reject Write Request | sadmd | sadfsd | metadata | | |
| Write To Chunk | sadfsd,sadcd | sadcd | data | chunkid | metadata |
| Notify Master Of Chunk | sadcd | sadmd | chunkid | | |
| Read Request | sadfsd | sadmd | filename | | |
| Grant Read Request | sadmd | sadfsd | chunkserverIP | chunkid | |
| Read Chunk | sadfsd | sadcd | chunkid | | |
| Chunk Contents | sadcd | sadfsd | data | | | 
| Join Request | sadcd | sadmd | mem_available | | |
| Reject Join Request | sadmd | sadcd | | | |
| Accept Join Request | sadmd | sadcd | | | |
