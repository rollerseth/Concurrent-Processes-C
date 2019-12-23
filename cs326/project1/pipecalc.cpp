/*
#Filename: pipecalc.cpp
#Name: Seth Roller
#Date: 9/19/19
#Program Name: pipecalc.cpp
#Assignment Number: 1
#Problem: Creating a user interface with fork and pipes
to imitate a basic calculator
#Flow: The file first contains all declarations and then
will fork and execute the child and parent processes
while communicating through a pipe

 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <wait.h>
#include <stdio.h>
#include <string.h>
#include "iostream"
using namespace std;

int main()
{

 int statloc;

 int parentPipe[2];
 int childPipe[2];

 string option = "";
 int operand1 = 0;
 int operand2 = 0;
 int result = 0;

 //two pipers
 pipe(parentPipe);
 pipe(childPipe);
 
 int retval;

 //forking
 retval=fork();
 if (retval<0)
   {
     return(-1); /*fork failed*/
   }

 if(retval==0) /*child */
   
   {

     close (parentPipe[1]);
     close (childPipe[0]);
     while (true) {
       
       read(parentPipe[0], &option, sizeof(option));
       if (option == "X")
	 break;

       read(parentPipe[0], &operand1, sizeof(int));
       read(parentPipe[0], &operand2, sizeof(int));

       if (option == "+")
	 result = operand1 + operand2;
       else if (option == "-")
	 result = operand1 - operand2;
       else if (option == "*")
	 result = operand1 * operand2;
       else if (option == "/")
	 result = operand1 / operand2;

       write(childPipe[1], &result, sizeof(int));
     
     }

     close(childPipe[1]);
   }

 else /* parent*/
   
   {
     
     /*parent closes pipe ends as it is notusing it */
     close (childPipe[1]);
     close (parentPipe[0]);

     bool theLoop = true;

     //display the interface
     cout << endl;
     cout << "Name: Seth Roller" << endl;
     cout << "Lab Name: Pipes" << endl;
     cout << "Problem: A calculator has been implemented through";
     cout << endl;
     cout << "the utilization of pipes." << endl;
     cout << endl;

     cout << "Enter the following below to perform calculator operations.";
     cout << endl;
     cout << "+ for addition" << endl;
     cout << "- for substraction" << endl;
     cout << "* for multiplication" << endl;
     cout << "/ for division" << endl;
     cout << "X for termination\n" << endl;

     //enter loop until X
     while (theLoop) {

       cout << "Enter your option: ";
       cin >> option;
       cout << endl; // for turnin
       // checks to see if the entered input is a string
       if (!cin.good()) {
	 cin.clear();
	 cin.ignore(60, '\n');
	 
	 cout << "Invalid input of " << option << "!" << endl << endl;
	 cout << "Please enter +, -, *, /, or X.";
	 cout << "\n\n";
	 
       }


       // then goes to check if the string is valid
       // and processes the string through if else
       else if (option == "+" || option == "-" || option == "/" ||
		option == "*" || option == "X") {
    
	 cout << "You entered " << option << endl << endl;
	
	 write(parentPipe[1], &option, sizeof(option));
	 
	  //checks if correct string was entered
	 if (option == "+" || option == "-" || option == "/" ||
	     option == "*") {
	   bool innerLoop = true;
	     
	     while (innerLoop) {
	       cout << "Enter your operand1: ";
	       cin >> operand1;
	       cout << endl; // for turnin
	       // checks to see if the entered input is an integer
	       if (!cin.good()) {
		 cin.clear();
		 cin.ignore(60, '\n');
		 
		 cout << "Invalid input!"
		      << endl << endl;
		 cout << "Please enter an integer.";
		 cout << "\n\n";
		 
	       }
	       else {
		 cout << "You entered " << operand1 << endl << endl;
		 innerLoop = false;
	       }
	     }
	     innerLoop = true;

	     while (innerLoop) {
	       cout << "Enter your operand2: ";
	       cin >> operand2;
	       cout << endl; // for turnin
	       // checks to see if the entered input is an integer
	       if (!cin.good()) {
		 cin.clear();
		 cin.ignore(60, '\n');
		 
		 cout << "Invalid input!"
		      << endl << endl;
		 cout << "Please enter an integer.";
		 cout << "\n\n";
		 
	       }
	       else {
		 innerLoop = false;
		 cout << "You entered " << operand2 << endl << endl;
	       }
	     }

	     //writes the operands to parentpipe
	     write(parentPipe[1], &operand1, sizeof(int));
	     write(parentPipe[1], &operand2, sizeof(int));


	     //read result and display
	     read(childPipe[0], &result, sizeof(int));
	     cout << "The result of " << operand1 << " " << option;
	     cout << " " << operand2 << " is " << result << endl << endl;
	   }

	 //exiting
	   else if (option == "X") {
	     cout << "Exiting\n";
	     write(parentPipe[1], &option, sizeof(option));
	     theLoop = false;
	   }

	   else {
	     cout << "You entered " << option << " and this ";
	     cout << "incorrect input." << endl;
	     cin.clear();
	     cin.ignore(60, '\n');
	   }
	 
       } // end of else if

       else {
	 cin.clear();
	 cin.ignore(60, '\n');
	 
	 cout << "Invalid input of " << option << "!" << endl << endl;
	 cout << "Please enter +, -, *, /, or X.";
	 cout << "\n\n";
       }
       
     } // end of while loop

     //wait for child to finish 
     wait(&retval);
     
   }
 
}
