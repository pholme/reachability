#Calculating the forward and backward reachability of temporal networks

To compile:
1. Make sure `gcc` is installed.
2. Make a directory `o` for the object files.
3. Type `make`

To run:
Make sure your temporal network is in a contact list format. I.e., a file with three columns. Every row (*i*,*j*,*t*) represents a contact between *i* and *j* at time *t*. This file needs to be sorted in time. The time stamps need to be integers between 0 and 4,294,967,295. The node ID-numbers should be between 0 and *N* - 1 (where *N* is the total number of nodes). If `example.cl` is a text file by the above specifications, then you can run the program as `./reachability example.cl`.

The output:
Is a list with three columns: 1. The node ID. 2. It's forward reachability. 3. It's backward reachability.
