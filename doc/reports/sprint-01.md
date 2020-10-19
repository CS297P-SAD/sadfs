## Sprint: 1
10/05/2020 - 10/16/2020

### Team: GladFS
| Team Member | Tickets | Points |
|    :---:    |  :---:  | :----: |
| Adam        |    4    |   NA   |
| Daman       |    4    |   NA   |
| Shubham     |    4    |   NA   |

### Sprint Overview:
#### Planned
| Items | Points |
| :---: | :----: |
|   9   |   NA   |
#### Completed
| Items | Points |
| :---: | :----: |
|   4   |   NA   |

### Sprint Retrospective:
* What did you do this sprint?

  We spent the first couple of days exploring our options and decided to
  implement a distributed file system. The rest of the week was spent reading
  relevant literature and finalizing the features of the file system. We then
  designed the API and high-level architecture of our file system while
  reviewing the architectures of existing file systems. We also set up a git
  repo. Implementing basic client server communication is currently in
  progress.

* What went well?

  We discovered that working virtually, the most effecive way to brainstorm
  was to come up with ideas offline and combine the best aspects of our ideas
  on our next Zoom call. In person, these would likely be whiteboard sessions,
  but for a variety of reasons brainstorming live over Zoom is less effective
  than this method. Meeting over video conference every day, as opposed to
  discussing topics offline over text messages, was really effective in
  getting our thought across, and quickly coming to conclusions. Although we
  did not complete all the work we had planned for the sprint, we are pleased
  with the thought we were able to put behind decisions and implementations;
  we believe these will pay dividends as the project progresses.
  
* What didn’t go well?

  As mentioned in our short report, consulting entire textbooks on distributed
  systems is too time consuming for a capstone project of this length. We found
  much more utility in looking at white papers or other documents describing
  the design choices of other file systems. We underestimated how long it would
  take to design our file system at a high level, and how long it would take to
  implement basic features -- we did not take into account the setup required
  in the beginning.

* What should go better?

  We haven't made much progress on the actual coding side yet. This is probably
  okay as we got a lot of preliminary design work done; this next sprint will
  hopefully have more tangible results.

### Sprint Backlog

|  ID  | Type | Owner | Title | Status | Estimate |
| :--: | :--: | :---: | :---: | :----: | :------: |
| 1 | Task       | Team       | Literature survey           | Completed | NA |
| 2 | Task       | Team       | Finalize semantics of usage | Completed | NA |
| 3 | Task       | Team       | API design                  | Completed | NA |
| 4 | Task       | Team       | Operation sequence design   | Completed | NA |
| 5 | Task       | Team       | Communication protocol design | In progress | NA |
| 6 | User Story | Unassigned | As an admin, I should be able to configure default options for the server/service daemons in a configuration file, and override them over the command line | Unassigned | NA |
| 7 | User Story | Unassigned | As an admin, I should be able to start/stop the server/service daemons | Unassigned | NA |
| 8 | User Story | Unassigned | The Master server daemon should be able to accept connections and respond | Unassigned | NA |
| 9 | User Story | Unassigned | The Chunk server daemon should be able to open connections to the Master server and other Chunk servers | Unassigned | NA |
