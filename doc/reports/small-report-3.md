## Week 5 - Small report #3

### Team: GLADFS - Shubham Gupta, Adam Monahan, Daman Mohan Kumar

1. What we did as a team during the week that just ended (later on this can become "which user stories"). Which team member worked on which parts?
	
  We implemented more user stories and did debugging related to basic inter-server communications. We explored how our project could use FUSE to overwrite i/o system calls with our filesystem and started looking at how to integrate protobuffs into our protocol.
 
2. What worked and what didn't work during the week. For items or tasks that didn't work, why did they not work?
	
  Our daily scrum meetings and process for assigning user stories still works well.
 
3. What do we plan to work on next week? This chapter #3 can be affected by chapters #1 and #2 above.

  We aim to finish inter-server communication including setting up a way for servers to send messages to each other. We also need to set up a mechanism for servers to open pipes to send raw data streams (as opposed to the infrastructure for control messages we've been working on).
