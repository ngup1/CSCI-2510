[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/lHL2eIbI)
# CSCI 2510: Studio - Sockets

------------------------------------------------------------------------

Sockets are a fundamental abstraction for many types of computer
communication- both inside a single machine and over networks. They are
the basic primitives used to connect machines together, and they are
used to implement all kinds of networked communication. Despite this,
they are surprisingly simple to use, as they encapsulate and hide
virtually all of the technical details need to accomplish such
communication, and are accessed (like many other things in Linux) just
like you were reading or writing a local file.

In this studio, you will:

1.  Set up and configure sockets with `socket()`, `bind()`, and
    `listen()`
2.  Recieve and make a socket connection with `accept()` and `connect()`
3.  Use sockets to send data between processes within a system
4.  Connect to a remote server on the internet

------------------------------------------------------------------------

Please complete the required exercises below, as well as any optional
enrichment exercises that you wish to complete.

As you work through these exercises, please record your answers, and
when finished submit your work via the git repository.

------------------------------------------------------------------------

### Required Exercises

As the answer to the first exercise, list the names of the people who
worked together on this studio.

The goal of this studio is to establish a local connection between two
processes on the same machine. We\'ll write two programs: a server and a
client. First we will write the server, whose role is to create the
communications channel and to *listen* for incoming connections. A good
example of how to construct a basic server can be found in `man 2 bind`
under the section labeled EXAMPLE. Feel free to copy this code, but be
sure to cite it.

Your server should perform the following actions:

1.  First, create a socket with the system call `socket()`. To create a
    local connection use the domain `AF_LOCAL` (or equivalently
    `AF_UNIX`), the connection type `SOCK_STREAM`, and protocol zero.
2.  Second, create the communications channel with the `bind()` system
    call. This associates your socket from step one with a
    machine-visible address. In the case of `AF_LOCAL`, this address is
    a path in the file system. Directions on how to specify a local
    address in this way can be found at `man 7 unix`.
3.  Now, having created the channel, your program needs to declare that
    it is going to listen for incoming connections with the `listen()`
    system call. Sockets only allow one process to connect at a time, so
    the second parameter determines how many connection requests can
    wait in line before they are rejected.
4.  Use the `accept()` system call to accept a connection over the
    socket interface. If no connection is immediately available then
    this system call will, by default, cause your program block until
    another program attempts to connect.
5.  When your program successfully returns from `accept()` the
    connection has been established. You may read and write data over
    the channel with the lower level `read()` and `write()` system
    calls, or you can instantiate a stream interface with the `fdopen`
    function. When doing so, be sure to use the socket descriptor
    returned by the call to `accept()`, not the one created by the
    initial call to `socket()`. Have your server read from the socket
    and print what it gets to the console.
6.  Finally, use the system call `unlink()` in order to destroy the
    socket and connection before the program returns.

***Note:*** Many things can go wrong with socket-based communication. As
usual, you should always check function return codes to detect errors.
Recall that most functions allow you to print a descriptive error
statement with a line of code such as: `perror("Error calling bind()")`.

Now we will write a client program. We will make use of the previously
created communications channel. In particular:

1.  Create a communications endpoint with a call to the `socket()`
    system call.
2.  Establish a connection with the `connect()` system call. This
    requires using the same socket address struct as the call to
    `bind()`.

At this point, barring any errors, your communcations channel is ready
to be used. Send some messages to the server program and print them to
standard output in order to validate the functionality. Copy and paste
the server output as the answer to this exercise. Note that a call to
read an empty socket will block until data is available.

If you haven\'t already, refactor your server with an event loop so that
it will stay alive through multiple connection attempts. This requires
that your server make multiple calls to `accept()`, but not to
`listen()` or `bind()`. Print a message each time a new connection is
established and run your client program several times with the same
server to validate this behavior. Copy and paste the server output as
the answer to this exercise.

Rather than having your server stay alive forever, have your program
terminate when it recieves the message \"quit\". Use the function
`strncmp` to test for string equality. The similar function `strcmp` is
unsafe.

Copy and paste terminal output demonstrating this functionality

### Optional Enrichment Exercises

1.  None

------------------------------------------------------------------------
