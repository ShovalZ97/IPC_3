# Task 3 - IPC 

### Yair Turgeman 209326776

### Shoval Zohar 318284668

## Part A - Cmd Chat Tool

Implementing a cmd chat tool that can send messages over the network, for that tool, listening in
The other side, and receiving the response, so that there will be communication from 2 sides, at the same time.

By "chat" is meant a tool that can read input from the keyboard, and at the same time listen to the socket
of the other side.

Client Side: stnc -c IP PORT
Server Side: stnc -s PORT

The communication will be performed using IPv4 UDP protocol.
The chat tool acts as a command-line interface that reads input from the keyboard and listens for messages from the other side due to using poll function.

In order to make the chat simultansily and synchronized in Server side as Client side , we use poll mechanishem.
The program creates a connection between the server and the client using sockets. After establishing a connection, the program uses a call to the poll() function to monitor two sockets: one for input from the client and one for input from the server. When the user types a message and presses enter, the software sends the message to the server. When the server receives a message, it sends it back to the client. Both the client and the server display the
Messages they receive from the other side.

## Part B: Performance Test
Network performance test tool.
We created a block of data, 100MB in size, we generated a check sum (hash) for the above data
And we transferred the data in a selected communication style, while measuring the time it takes
and we reported the result to stdOut.

The communications styles are:
tcp/udp ipv4/ipv6 (4 variants)
mmap a file. Named pipe (2 variants)
Unix Domain Socket (UDS) :stream and datagram (2 variants))

#### The usage for the client side is as follows:

stnc -c IP PORT -p <type> <param>
  
-p will indicate to perform the test

<type> will be the communication types: so it can be ipv4,ipv6,mmap,pipe,uds

<param> will be a parameter for the type. It can be udp/tcp or dgram/stream or file name:
  
#### The usage for the server side is as follows:

Server Side: stnc -s PORT -p -q

The -p flag indicates a performance test, and the -q flag enables quiet mode, where only the testing results are printed. This mode is essential for automatic testing.

The results will be in milliseconds (ms) and printed like this :

name_type,time

Examples:

ipv4_udp,112233

uds_stream,112233

mmap,223355

pipe,554411

### We have 8 combinations:
____ipv4 tcp____
  
Server:
 ./stnc -s 8080 -p 

 Client:
./stnc -c 127.0.0.1 8080 -p ipv4 tcp
  
____ipv4 udp____

Server:
./stnc -s 8080 -p 

Client:
./stnc -c 127.0.0.1 8080 -p ipv4 udp
  
____ipv6 tcp____
  
Server:
./stnc -s 8080 -p 

Client:
./stnc -c 127.0.0.1 8080 -p ipv6 tcp
  
____ipv6 udp____

Server:
./stnc -s 8080 -p 

Client:
./stnc -c 127.0.0.1 8080 -p ipv6 udp
 
_____uds dgram_____
 
Server:
./stnc -s 8080 -p 

Client:
./stnc -c 127.0.0.1 8080 -p uds udp dgram
  
____uds stream_____
 
Server:
./stnc -s 8080 -p 

Client:
./stnc -c 127.0.0.1 8080 -p uds udp stream
  
____mmap filename____
  
 Server:
./stnc -s 8080 -p 

Client:
./stnc -c 127.0.0.1 8080 -p uds mmap filename
  
_____pipe filename_____
 
 Server:
./stnc -s 8080 -p 

Client:
./stnc -c 127.0.0.1 8080 -p uds pipe filename
 ![תמונה 1](https://github.com/ShovalZ97/IPC_3/assets/118892976/23710c4e-5b12-4508-9813-262c44d228a9)

 <img src="https://user-images.githubusercontent.com/118892976/211491934-637d03b5-c3cd-4207-8d95-bf84b977128e.png" alt="drawing" width="500"/>
  
 *** To run the program you need to add the ssl library and run this commands in terminal :

sudo apt-get update

sudo apt-get install libssl-dev

openssl version
 
## HAVE FUN !!! 
