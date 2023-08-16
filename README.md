Project 3 Thread Synchronization - Moyata Ebisso ICS 462

a) How to compile and run program:
      1) cd to the correct directory
      2) compile the program with this line of code: 
              gcc -pthread -o run vehiclemain.c
      3) run the program with this line of code:
              ./run
    
b) How to enter information about a schedule:
      1) First you enter the number of groups in the schedule, and press enter. Ex) 3
      2) Next, you enter the number of vehicles in the group. Ex: 10
      3) Next, you enter the Northbound/Southbound probability of the group. Ex) 1.0/0.0
      4) Finally, you enter the delay before the next group. Ex) 10

c) A couple known problems my code has is:
    1) When the user inputs the information for a schedule, my code only takes in info for one group at a time, and then runs right after the group is entered, and asks for the next groups info after.
    2) My code doesn't print the bridge's status or waiting queue the exact same way that the project spec does.
   
