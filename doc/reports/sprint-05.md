## Sprint: 5
11/29/2020 - 12/12/2020

### Team: GladFS
| Team Member | Tickets | Points |
|    :---:    |  :---:  | :----: |
| Adam        | 8 | 28 |
| Daman       | 7 | 36 |
| Shubham     | 5 | 29 |

### Sprint Overview:
#### Planned
| Items | Points |
| :---: | :----: |
| 20 | 93 |
#### Completed
| Items | Points |
| :---: | :----: |
| 18 | 83 |

### Sprint Retrospective:
This section should answer the following questions:
* **What did you do this sprint?**  
  We implemented the following:
  - a uuid class to uniquely identify servers and chunks of files
  - support for libfuse to integrate our project with the user's native filesystem
  - finished implementng a messaging class for component communication
  - add the following messages to our messaging class and implemented their handlers on 
    the servers: heartbeat, request to join the network, notification of write, request 
    for chunk location, release a write lock, ack/nack
  - a separate thread on the chunk servers for sending heartbeats
  - our filesystem's getattr() and read() implementations

* **What went well?**  
  We definitely have increased our speed and made more progress this sprint than
  during previous sprints. We also tweaked our code review process to merge changes
  in faster.

* **What didn't go well?**  
  We had a couple instances of two team members doing the same work, which is inefficient.
  There were a few times that some unintuitive git/GitHub features made our workflow slow
  down to fix mistakes or resolve conflicts between versions.

* **What should go better?**  
  We should try to communicate what we are working on more clearly to avoid duplicate 
  efforts. But overall we are very close to a working version and feel we are in good shape 
  for the demos coming up this week.

### Sprint Backlog
List all tasks you were working on.
|  ID  | Type | Owner | Title | Status | Estimate |
| :--: | :--: | :---: | :---: | :----: | :------: |
| 1 | User Story | Adam | Chunk server daemon should be able to establish new connections | Completed | 3 |
| 2 | User Story | Adam | Master server should listen for, and accept connections | Completed | 3 |
| 3 | User Story | Adam | Implement Chunk server class | Completed | 3 |
| 4 | User Story | Adam | Master server should load file system info from the disk upon startup | Completed | 3 |
| 5 | Task       | Daman | Fix bugs in example/test code | Completed | 2 |
| 6 | Task | Daman | Implement control messages for read/write operations and identification | Completed | 8 |
| 7 | User Story | Shubham | On startup, the file system daemon should mount sadfs at the configured mountpoint | Completed | 8 |
| 8 | Task       | Adam | Implement internal protobuf for faster database serialization | Completed | 2 |
| 9 | User Story | Adam | Create functions for master server to maintain list of chunk servers | Completed | 3 |
| 10 | User Story | Adam | Create functions for master to maintain list of chunks (including generating uuids) | Completed | 3 |
| 11 | Task | Daman | Re-implement control message framework to be more extensible | Completed | 8 |
| 12 | Task | Shubham | Set up gRPC | Abandoned | 8 |
| 13 | User Story | Adam | Implement message handlers for master server | Completed | 8 |
| 14 | User Story | Daman | Implement heartbeat messages | Completed | 3 |
| 15 | User Story | Daman | Create separate threads for the chunk server's responsibilities | Completed | 5 |
| 16 | User Story | Shubham | Implement our filesystem's readdir() | Completed | 3 |
| 17 | User Story | Shubham | Implement our filesystem's read() | Completed | 5 |
| 18 | User Story | Shubham | Implement our filesystem's write() | In Progress | 5 |
| 19 | User Story | Daman | Implement data streaming for a read | Completed | 5 |
| 20 | User Story | Daman | Implement data streaming for a write | In Progress | 5 |
