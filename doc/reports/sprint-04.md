## Sprint: 4
11/14/2020 - 11/28/2020

### Team: GladFS
| Team Member | Tickets | Points |
|    :---:    |  :---:  | :----: |
| Adam        |  |  |
| Daman       |  |  |
| Shubham     |  |  |

### Sprint Overview:
#### Planned
| Items | Points |
| :---: | :----: |
|  |  |
#### Completed
| Items | Points |
| :---: | :----: |
|  |  |

### Sprint Retrospective:
This section should answer the following questions:
* **What did you do this sprint?**  
  We implemented the following:
  - a SQLite3-based database to persist information on the master server,
  and read it back into memory upon start-up.
  - (de)serialization of data using Google's Protocol Buffers instead of
  rolling our own.
  - runtime bookkeeping functionalities that will help in serving requests.
  - foundations of FUSE support (pending review).
  - foundations of a control message passing framework involving wrappers
  around Protocol Buffers, which we decided to redesign (in progress).
  - we decided to use the gRPC framework to stream file data between clients
  and chunk servers, and are working on its implementation (in progress).

* **What went well?**  
  We started working on the core functionality and have made considerable
  progress, after spending the last few sprints on setting up supporting
  functionalities. FUSE support from the get go is a huge bonus, and will
  make the file system much simpler to set up and use. The build automation
  and sanity checks we introduced on GitHub saved us a big headache later on
  by catching compatibility issues early on. The re-implemented control 
  message framework will make adding further control messages easy and less
  error-prone.

* **What didn’t go well?**  
  According to the road map we came up with, we should have progressed further
  than we did. Spending more time on designing the control message framework
  would have saved us a few days' effort that went into the re-implementation.
  In terms of planning, adding a single user story for the implementation of
  control messages was short-sighted. Our code review process is slower than
  we want it to be.

* **What should go better?**
  Our velocity needs to improve, and we should figure out a way to get pull
  requests through the door at a much quicker pace.

### Sprint Backlog
List all tasks you were working on.
|  ID  | Type | Owner | Title | Status | Estimate |
| :--: | :--: | :---: | :---: | :----: | :------: |
| 1 | User Story | Peter the Anteater | Eat all the ants he can find | In Progress | 3 |
| 2 | User Story | Peter the Anteater | Zot! Zot! Zot! | Completed | 1 |
|  |  |  |  |  |  |
