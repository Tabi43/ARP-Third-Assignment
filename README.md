# Third Assignment ARP

In this assignment, we have added two new features to the previous one - client and server connections - to allow communication between two similar applications running on different machines in the network.
When launching the application, you will be asked to choose one of the three execution modalities: normal (as in assignment 2), server, or client. If you choose server mode, the application will receive input from another application running in client mode. If you choose client mode, the application will run normally (as in assignment 2) and send its keyboard input to another application running in server mode.

## Installing and Run
----------------------
To compile the whole simulation is provided a shell script runnable by:
```
    $ ./compile.sh
```

To work properly you need to have installed the *libbitmap* library. *libbitmap* is a library to create and interact with simple bitmap object.

IF YOU HAVEN'T INSTALED THIS LIBRARY FOLLOW THESE STEPS:
* Download the source code from [this GitHub repo](https://github.com/draekko/libbitmap.git) in your file system.
* Navigate to the root directory of the downloaded repo and run the configuration through command ```./configure```. Configuration might take a while.  While running, it prints some messages telling which features it is checking for.
* Type ```make``` to compile the package.
* Run ```make install``` to install the programs and any data files and documentation.
* Upon completing the installation, check that the files have been properly installed by navigating to ```/usr/local/lib```, where you should find the ```libbmp.so``` shared library ready for use.
* In order to properly compile programs which use the *libbitmap* library, you first need to notify the **linker** about the location of the shared library. To do that, you can simply add the following line at the end of your ```.bashrc``` file:      
```export LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"```

Then you can run the simulation buy running directly the master process or running this code:
```
    $ ./run.sh
```
```
    $ ./bin/master
```
## User Guide
----------------------
after following the instructions to run the code, the user will see appear on the terminal the possibility of choosing between one of the modes in which he wants to run the program (normal, server, client).

* Normal mode: the application will run as described in the previous assignment.

* Server mode: the application will not use the keyboard for input. Instead, it will receive input from another application (running on a different machine) that is connected in client mode. In this mode, the application will prompt the user to enter the address and port of the companion application it will be communicating with. Once the connection is established, the server will be able to receive input from the client and display it on its ncurses window.

* Client mode: the application will run normally as in Assignment 2, but it will also send its keyboard input to another application running in server mode on a different machine. The user will be prompted to enter the address and port of the companion application to establish the connection. The application will act as a client, sending its keyboard inputs to the server application for processing.


## How it works
----------------------
Note that for a more specific explanation on the basic functioning of the program you can look at the readme of the second assignment that you can find by [clicking here](https://github.com/Massimo-Carlini/ARP_Second_Assignment).

The user will be asked to choose in which mode to run the program and based on the chosen mode, the program will work differently as explained below

* In the "normal" execution mode, process A is responsible for reading input from the keyboard and displaying it on its own ncurses window. It also sends the input to process B using shared memory. Process B receives the input from shared memory and displays it on its own ncurses window. The master process is responsible for launching both process A and B and managing the shared memory and semaphores used for inter-process communication.

* In the "server" execution mode, process A acts as a server and waits for a client connection from another instance of the application running in "client" mode. Once a connection is established, process A receives input from the connected client and displays it on its own ncurses window. The input is also sent to process B using shared memory. Process B receives the input from shared memory and displays it on its own ncurses window as usual.

* In the "client" execution mode, process A acts as a client and connects to a server instance of the application running in "server" mode. Process A sends keyboard input to the connected server, which then displays the input on its own ncurses window. Process B continues to receive input from shared memory and display it on its own ncurses window, as in the "normal" and "server" modes.
