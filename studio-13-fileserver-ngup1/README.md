[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/AXk1GuYg)
# CSCI 2510: Studio - Network File Server

------------------------------------------------------------------------

Application layer protocols allow different processes to communicate
with one another in an orderly and meaningful fashion. By agreeing on a
pre-arranged set of messages (the protocol) the two processes can
communicate to achieve every internet-enabled application you can think
of. Internet hostnames are translated to IP addresses with the DNS
protocol. Web browsers recieve web pages with the HTTP protocol. Servers
provide secure terminal access through the SSH protocol, and etcetera.

In this studio, you will:

1.  Implement a basic file server

------------------------------------------------------------------------

Please complete the required exercises below, as well as any optional
enrichment exercises that you wish to complete.

As you work through these exercises, please record your answers, and
when finished submit your work via the git repository.

------------------------------------------------------------------------

### Required Exercises

In your previous studio you created a server and a client program where
the server would listen for connections from a client, and then echo the
contents of those messages to the output of the server. Your server
would also listen for a specific message (\"quit\") which would cause
the server to terminate. This demonstrated communication between a
server and a client, but without accomplishing anything meaningful. We
will modify your server and client so that the client can request a
specific file be retrieved from the server.

Modify your server so that it interprets incoming messages (other than
\"quit\") as file names. It should attempt to open these files with the
`open()` or `fopen()` functions. Check the return value: if you cannot
open the file then return an error message to the client by writing to
the socket (the one returned by `accept()` in the server). Otherwise,
write the contents of the file to the socket. Once the server is done
writing (either the error message or the file contents) you should
terminate the server\'s end of the connection by calling the function
`shutdown()` with option `SHUT_RDWR` (see `man 2 shutdown` for details).

Similarly, modify your client so that after sending its request it will
`read()` from the socket, printing everything it recieves to standard
output. Once the call to `read()` returns zero (signifying there is
nothing more to read from the socket) it should terminate.

*Hint:* We had several studios early on (two and four) that involved
reading and reprinting text files through streams. Feel free to use this
code again.

Now, add one more special context word along with \"quit\". If the
client sends the string \"ls\" then the server should execute the
command `ls` and send the result back to the client. I would suggest you
do this with the `popen()` function documented at `man 3 popen`. Note
that this function returns a `FILE*` type, so you will need to use
`fgets()` to read from the returned file pointer.

### Optional Enrichment Exercises

1.  Create a more complex protocol that involves telling the server
    whether to send or recive a file from the client. If the client
    wants to read a file then the server should behave as above. If the
    client wants to send a file then the server should write the
    contents from the scocket into the desried file. Now you can save
    and restore files on a remote server!

------------------------------------------------------------------------
