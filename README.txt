alexlv
Alex Levenzon 308636141
EX1 - String Pattern Matching Algorithm PartB
slist.c - Implementation of a generic linked list
pattern-matching.c - implementation of a patterns matching automat
README.txt - A readme text file

This is the second part of the exercise, containing the whole program.

The program allows to search a given set of patterns in a given text , and finding all of their appeareances 
in the text. Its efficiecny is O(n), n is the length of the pattern. 

Please use the program in the following order - this is how you should use it:
1. allocate the pm variable
2. use the init function to init it.
3. add patterns to the automat using the addString function
4. complete construction by using the makeFSM function
5. make a matches variable and send to it the result of the search function. you should put in it the text to search in.
6. Do something with the list of the matches...
7.destroy pm with the destroy function
8.free the allocated pm pointer
9. ENJOY :)

The main program uses the generic list the we implemented in the previous part (Part A).

The pattern-matching.c file contains two private functions:
1. createState - a function for creating a new state. it receives a pointer to a state and parameters, and returns
an allocated and initialized state.
2. fsm_destroy_helper - A recursive function that implements the DFS traversion of a tree, and it destroys the automat.
   it used by the main destroy function.

Other functions were implemented by the guide that we recieved from Shimrit.

You have to write your own main in order to check the program. ENJOY !

I made defines for all arrows (->) in the program. I hope that they are understood. 
Shimrit told us that this is a better way of coding with those macros.