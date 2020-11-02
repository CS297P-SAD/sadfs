## Sprint: 2
10/19/2020 - 10/30/2020

### Team: GladFS
| Team Member | Tickets | Points |
|    :---:    |  :---:  | :----: |
| Adam        |    3    |   NA   |
| Daman       |    4    |   NA   |
| Shubham     |    1    |   NA   |

### Sprint Overview:
#### Planned
| Items | Points |
| :---: | :----: |
|   9   |   NA   |
#### Completed
| Items | Points |
| :---: | :----: |
|   6   |   NA   |

### Sprint Retrospective:
* What did you do this sprint?

  We started with the client server communication implementation across the
  master, chunk server and file system daemons. Set up directory structure
  and Makefile, implemented a user facing CLI based bootstrap service that 
  forks socket listener process with the corresponding input parameters.
  We used Berkeley/BSD sockets to establish connections, and tested them
  with an echo server program.

* What went well?
  
  Since the majority of last sprint was consumed in design discussions, we had
  a good idea of what we wanted to accomplish this sprint and were able to
  quickly jump into the implementation and made tangible progress.
  We were able to meet regularly, update each other of our progress and share
  our learnings.
  We found Github "issues" feature to be a better alternative to common file to
  document and track user stories.
  
* What didn’t go well?

  We faced a cross-platform compilation issue, where code written and tested 
  on one software platform did not compile on two other platforms. We found out 
  that this was because of differences in Linux's socket programming API, and
  macOS's version of clang not supporting initialization with braced-init-list.
  We faced issue with squashing commits on Github.
  Shubham was not able to contribute sufficient time to the project because of
  interview preparation.

* What should go better?

  We should be able to accelerate our development pace and expect fewer setup
  issues.

### Sprint Backlog

|  ID  | Type | Owner | Title | Status | Estimate |
| :--: | :--: | :---: | :---: | :----: | :------: |
| 1 | User Story | Daman      | Master Daemon Startup Configuration                   | Completed | NA |
| 2 | User Story | Adam       | Chunk Server Daemon Startup Configuration             | Completed | NA |
| 3 | User Story | Shubham    | File System Daemon Startup Configuration              | In Progress | NA |
| 4 | User Story | Daman      | Master Daemon Bootstrap                               | Completed | NA |
| 5 | User Story | Adam       | Chunk Server Bootstrap                                | Completed | NA |
| 6 | User Story | Unassigned | File System Bootstrap                                 | Unassigned | NA |
| 7 | User Story | Daman      | Introduce Listener Class                              | Completed | NA |
| 8 | Task       | Daman      | Implement socket and listener class without exception | Completed | NA |
| 9 | User Story | Adam       | Chunk Server Establish New Connections                | In Progress | NA |
