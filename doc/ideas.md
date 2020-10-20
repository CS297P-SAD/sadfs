# Proposals
## API
`sadfs::create` - creates a file  
`sadfs::unlink` - deletes a file  
`sadfs::mkdir`  - creates a directory  
`sadfs::rmdir`  - deletes a directory  
`sadfs::open`   - opens a file for reading or writing  
`sadfs::close`  - closes an opened file descriptor  
`sadfs::read`   - reads data from an opened file  
`sadfs::write`  - writes data to an opened file  

## Specifics
`sadfs::open` returns a file descriptor like open(2). Master server keeps  
track of open files by using GUIDs. Client side code translates GUIDs to  
file descriptors.

# Questions
* Client side -- library or service (daemon)?  
  We will need to keep track of open file descriptors at the client side.  
  How do we do this?  
  **Service**  
  Pros  
    * Independent service can periodically inform the Master server that  
      the client is alive. Prevents a file from being lost to an unresponsive  
      client that has a locking handle.
    * Client application code will be simple without the need for boilerplate  
      code.

  Cons  
    * Increased complexity


# Examples

## Writing 

### Sequence of events
* Client sends write request to master
* Master verifies that "file.txt" is not locked for writing
* Master locks "file.txt" for writing
* Master assigns "file.txt" to a new chunk or looks up the chunkid of the last non-full chunk
	 * new chunk is added to database, but currently the list of corresponding servers is empty
* Master compiles a list of chunk servers to store new chunk or list that it is already on
* Master chooses a primary chunk server
* Master sends client chunk id and primary ip address, other ip addresses are in the message's metadata
* Client sends data to primary ip address along with forwarded metadata
* Chunk server 1 writes data to a local file named chunkid
* Chunk server 1 sends data to chunk servers 2 and 3
* Chunk server 1 sends master message saying it has chunkid
* Chunk servers 2 and 3 write data, send master same message
* Master adds incoming IP addresses to list of servers containing chunkid

### Inter-server communication needed:
* Client to master: write_request(filename)
* Master to client: grant_request(chunkid, chunkserverIP, metadata)
	* metadata includes list of secondary chunk servers, filename
* Master to client: reject_request(metadata)
* Client to chunk: write_chunk(data, chunkid, metadata)
* Chunk to chunk: write_chunk(data, chunkid)
	* can either have separate command for chunk to chunk or leave list of secondary chunkids empty
* Chunk to master: contain_chunk(chunkid)
  * maybe need a more obvious name, but this means "I contain chunk number `chunkid` "

## Reading 
(from file "file.txt")

### Sequence of events
* Client sends read request to master
* Master looks up first chunk for "file.txt"
* Master looks up list of chunk servers that contain chunk 35
* Master chooses optimal server for client
* Master sends chunk server's IP address to client and the chunk id
* Client sends read request for chunk 35 to chunk server
* Chunk server sends data back to client

### Inter-server communication needed:
* Client to master: read_request(filename)
* Master to client: read_chunk(chunkserverip, chunkid)
* Client to chunk: read_chunk_request(chunkid)
* Chunk to client: read_data(data)

## Starting up Chunk Server

### Sequence of events
* Chunk server sends message requesting to join the server, how much memory it is guaranteeing
* Master decides whether to accept request
	* If so, master adds chunk server to list of active chunk servers
* Once it receives accept, chunk server starts sending a message at regular intervals confirming it is active
* Each time the master receives one of these messages it resets the ttl field for that chunk server

### Inter-server communication needed
* Chunk to master: join_request(memory)
	* password?
* Master to chunk: reject_join_request()
* Master to chunk: accept_join_request()
* Chunk to master: active()

# Tables stored on master:
	Files: (persistent)
	filename, locked?, lockTTL, list of chunkids

	Chunks: (in memory)
	chunkid, list of chunk servers

  	Chunk servers: (in memory)
  	IP, port, total space, used space, ttl
