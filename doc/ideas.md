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
* Client sends write request to master, including offset
* Master determines which chunk corresponds to offset
* if chunk doesn't exist yet (i.e. file is empty or it needs to append a chunk)
	* Master creates new unique chunkid
		* new chunk is added to database, but currently the list of corresponding servers is empty
	* Master assigns file to chunkid
	* Master compiles a list of chunk servers to store new chunk
* if chunk exists
	* Master confirms that chunk is not locked for writing
	* Master locks chunk for writing
	* Master retrieves list of servers that contain that chunk
* Master sends client chunkid and list of chunk servers
* Client sends data to first chunk server on the list, along with the offset within the chunk and the list of remaining chunk servers
* Chunk server 1 writes data to a local file named chunkid
* Chunk server 1 sends data to remaining chunk servers
* Chunk server 1 sends master message saying it has chunkid
* Other chunk servers write data, send master same message
* Master adds incoming IP addresses to list of servers containing chunkid

### Inter-server communication needed:
* Client to master: write_request(filename, offset)
* Master to client: grant_request(chunkid, chunkserverIPs)
	* chunkserverIPs is a list of chunk servers
* Master to client: reject_request(filename, offset)
* Client to chunk: write_chunk(data, offset, chunkid, chunkserverIPs)
* Chunk to master: contain_chunk(chunkid)
  * maybe need a more obvious name, but this means "I contain chunk number `chunkid` "

## Reading

### Sequence of events
* Client sends read request to master including filename and chunk within the file
	* This doesn't require the file to know the chunkid, it just allows it to say e.g. "send me the third chunk in file.txt"
* Master looks up chunkid
* Master looks up list of chunk servers that contain chunkid
* Master chooses optimal server for client
* Master sends chunk server's IP address to client and the chunkid
* Client sends read request for chunkid to chunk server
* Chunk server sends data back to client

### Inter-server communication needed:
* Client to master: read_request(filename, chunk_number)
* Master to client: read_chunk(chunkserverIP, chunkid)
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
