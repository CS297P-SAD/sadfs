## Week 9 - Small report #5

### Team: GLADFS - Shubham Gupta, Adam Monahan, Daman Mohan Kumar

1. What we did as a team during the week that just ended (later on this can become "which user stories"). Which team member worked on which parts?
	
  Shubham worked on setting up gRPC for streaming data between nodes. Daman set up a working version of the protobuf messaging and implemented a uuid class. Adam worked on functions for maintaining the file metadata (generating chunkids and associating them with files).
 
2. What worked and what didn't work during the week. For items or tasks that didn't work, why did they not work?
	
  Our turnover time between creating and merging a pull-request has been slow throughout the project since the changes requested and changes being made can involve a lot of back and forth when done asynchronously. Starting on Friday we decided to hold daily meetings (in addition to scrum meetings) to go over all outstanding pull requests together to speed this up. This has already noiceably increased the volume. 
 
3. What do we plan to work on next week? This chapter #3 can be affected by chapters #1 and #2 above.

  We aim to finish the project by Monday or by mid week at the latest. This will involve 3 things: 
  - expanding the messaging to support all the control messages we have designed
  - have the servers respond to these messages by connecting them to the functionality we have been implementing all quarter
  - have the outward-facing API trigger the client to send the appropriate messages
