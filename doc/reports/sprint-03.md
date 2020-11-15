## Sprint: 3
11/02/2020 - 11/13/2020

### Team: GladFS
| Team Member | Tickets | Points |
|    :---:    |  :---:  | :----: |
| Adam        | 4 | 12 |
| Daman       | 5 | 13 |
| Shubham     | 1 | 8 |

### Sprint Overview:
#### Planned
| Items | Points |
| :---: | :----: |
| 10 | 33 |
#### Completed
| Items | Points |
| :---: | :----: |
| 7 | 19 |

### Sprint Retrospective:
This section should answer the following questions:
* What did you do this sprint?  
  We implemented an abstraction to set up outbound connections, and
  partially implemented control messages built on top of Protocol Buffers.
  We implemented functionality in the master and chunk servers to accept
  incoming connections. We also refactored some code to better suit our
  design. We implemented another feature allowing the master server to
  build an in-memory store of metadata upon start. We have implemented
  mounting of a file system on the client side. We moved from using a
  manually written Makefile to using CMake. We also implemented GitHub
  automation to run build tests on Ubuntu and MacOS on every push, and
  pull request.

* What went well?
  We started implementing FUSE support, deviating from our original
  plan of having client-side applications relink against our read/write
  API. This will make our file system easily accessible and testable.
  Having automated build tests on GitHub will help us avoid running into
  the recurring issue where code written on one platform failed to compile
  on other platforms. Using CMake has made it very easy and flexible to
  test our code on different platforms, since all three team members work
  on different Operating Systems.

* What didn't go well?
  The transition to CMake, and setting up automation in GitHub were
  unplanned tasks, and set us back quite a bit, because of which we weren't
  able to implemented as many user stories as we needed.

* What should go better?
  We should have made the switch to CMake, and set up automated build tests
  the first time we faced those issues -- we should be more proactive in
  solving problems in a permanent fashion. We picked up user stories on
  an ad-hoc basis this sprint. We should plan sprints better, and come up
  with a road map so that we have a better idea of where we stand.

### Sprint Backlog
List all tasks you were working on.
|  ID  | Type | Owner | Title | Status | Estimate |
| :--: | :--: | :---: | :---: | :----: | :------: |
| 1 | User Story | Adam | Chunk server daemon should be able to establish new connections | Completed | 3 |
| 2 | User Story | Daman | Introduce class to listen for new connections | Completed | 3 |
| 3 | User Story | Adam | Master server should listen for, and accept connections | Completed | 3 |
| 4 | User Story | Adam | Implement Chunk server class | Completed | 3 |
| 5 | User Story | Adam | Master server should load file system info from the disk upon startup | In progress | 3 |
| 6 | Task       | Daman | Fix bugs in example/test code | Completed | 2 |
| 7 | Task       | Daman | Set up CMake | Completed | 3 |
| 8 | Task       | Daman | Set up GitHub Action to run build tests | Completed | 2 |
| 9 | User Story | Daman | Implement control messages for read/write operations | In progress | 3 |
| 10 | User Story | Shubham | On startup, the file system daemon should mount sadfs at the configured mountpoint | In progress | 8 |
