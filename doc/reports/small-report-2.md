## Week 3 - Small report #2

### Team: GLADFS - Shubham Gupta, Adam Monahan, Daman Mohan Kumar

1. What we did as a team during the week that just ended (later on this can become "which user stories"). Which team member worked on which parts?
	
  We discussed whether to use gRPC communication library or implement our own socket programming based on Shubham’s research and decided to do our own socket programming for this sprint and fall back on gRPC if we find the networking component too tedious. We merged in Daman’s code setting up the basic directory structure, Makefile and the config parsing. Adam did further work on the protocol design and on Friday set up the same config parsing as the master server on the chunk server.
 
2. What worked and what didn't work during the week. For items or tasks that didn't work, why did they not work?
	
  Using GitHub’s “issues” feature rather than documenting user stories in a markdown file is a much clearer way of keeping track of everything.
  We looked into using boost::asio for socket programming, but after further discussion, we decided that we might as well try implementing socket programming from scratch.
 
3. What do we plan to work on next week? This chapter #3 can be affected by chapters #1 and #2 above.

  We plan to complete the remaining user stories currently on our git repo, all of which have to do with implementing the bootstrap processes and getting the server daemons running with configurable port numbers, etc. Once that is working and all 3 types of servers can communicate we will continue to refine the communication protocol and message design between the servers and start implementing it.
