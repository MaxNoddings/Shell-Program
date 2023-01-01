// Max Noddings
// Professor Trono
// Operating Systems - Shell Program Assignment
// September 21st, 2022

#include <stdio.h>  /* Acts like import java.io.*; */
#include <stdlib.h> /* for library function prototypes */
#include <unistd.h>
#include <sys/wait.h>
#include <sys/signal.h> // FOr access to SIGINT interrupt
#include <string.h> // For access to the strin function library
# define MAX_LINE 80 /* 80 characters per line for input */
# define listSize 10

char historyBuffer[listSize][MAX_LINE]; // buffer to hold the last 10 commands entered
int commandNum = 0; // Integer to track how many total commands have been entered.
int indexTracker = 0; // Integer to track what spot in the historyBuffer array to put commands into.

/* This function will now return an int - signaling to the main whether or not to proceed with execvp and fork, but 
will just read in the next command line and separate
it into distinct arguments, using blanks as delimiters and have the args array pointer to the beginning 
of those arguments, and will convert them into null terminated C-style strings. */
int setup(char inputBuffer[], char *args[], int *flag)
{
  int length, // number of characters in the command line
    i, 
    start, // index where beginning of next command parameter is
    ct, // index of where to place the next parameter into args[]
    success; // Integer value that will be returned by the function to decide whether the command was a success of failure. 

  success = 1;
  ct = 0;
  length = read(0, inputBuffer, MAX_LINE);
  
  // Turn the inputBuffer command into a null terminated C-style string!
  inputBuffer[length] = '\0';

  /* 0 is the system predefined file descriptor for stdin (standard input), which is the user's screen in
this case. inputBuffer by itself is the same as &inputBuffer[0], i.e. the starting address of where to 
store the command that is read, and length holds the number of characters read in. inputBuffer is not a 
null teminated C-string. */
  start = -1; // Initialize start to -1.
  
  // Check to make sure that the user entered something into the command prompt.
  if (length == 0)
  {
    exit(0);
  }
  if (length < 0)
  {
    perror("error reading the command");
    exit(-1); // terminate with an error code of -1 */
  }
  
  commandNum = commandNum + 1; // Increment 1 to the commandNum.
  if (indexTracker < 9) // If there has been less that 10 commands entered, increment the indexTracker
  {
    indexTracker = indexTracker + 1; // Increment 1 to the indexTracker
  }
  
  // Handle if the user types in "r" here
  if ((strcmp(inputBuffer, "r\n\0") == 0))
  {
    if ((commandNum) > listSize) // If the history buffer is already full with 10 elements
    {
      for (int k = 0; k < 9; k++)
      {
        strcpy(historyBuffer[k], historyBuffer[k + 1]); // Shift up all the elements up one spot in the historyBuffer array.
      }
      strcpy(historyBuffer[9], historyBuffer[8]); // Copy the most recent command into historyBuffer[9] 
      strcpy(inputBuffer, historyBuffer[9]); // set the inputBuffer equal to the latest command
    }
    else // if the history buffer isn't filled up yet with 10 elements
    {
      strcpy(historyBuffer[commandNum - 1], historyBuffer[commandNum - 2]); // Copy the most recent command into the historyBuffer.
      strcpy(inputBuffer, historyBuffer[commandNum - 1]); // set the inputBuffer equal to the latest command
    }
    
    // set the length to the new inputBuffer.
    length = strlen(inputBuffer);
    
    // print out the most recent command / inputBuffer
    printf("%s", inputBuffer);
  }
  
  // Here handle if the user types in "r __"
  else if ((inputBuffer[0] == 'r') &&  (inputBuffer[1] == ' ') && (inputBuffer[3] == '\n')) 
  {
    for (int z = indexTracker; z >= 0; z--)
    {
      if (historyBuffer[z][0] == inputBuffer[2]) // iterate through the history buffer and compare the first letter of commands
      {
        if ((commandNum) > listSize) // If the history buffer is already full
        {
          for (int s = 0; s < 9; s++)
          {
            strcpy(historyBuffer[s], historyBuffer[s + 1]); // Shift up all the elements up one spot in the historyBuffer array.
          }
          strcpy(historyBuffer[9], historyBuffer[z - 1]); // Copy historyBuffer[z] into the top of the history buffer 
          strcpy(inputBuffer, historyBuffer[z - 1]); // set the inputBuffer equal to historyBuffer[z]
        }
        else // if the history buffer isn't filled up yet
        {
          strcpy(historyBuffer[indexTracker - 1], historyBuffer[z]); // Copy historyBuffer[z] into the top of the history buffer 
          strcpy(inputBuffer, historyBuffer[z]); // set the inputBuffer equal to historyBuffer[z]
        }
        
        // set the length to the new inputBuffer.
        length = strlen(inputBuffer);
        
        // print out the new command / inputBuffer that will be executed
	printf("%s", inputBuffer);

        success = 1; // successful command
        z = -1; // So the loop will be exited.
      }
      else
      {
        success = 0;
      }
    }
    
    if (success == 0) // If success is still zero after exiting the loop
    {
      printf("ERROR, what you typed in was not found in the 10 most recent commands.\n\n");
      commandNum = commandNum - 1;
      return success;
    }
  }
  else
  {
    // Execute regular case command
    if ((commandNum) > listSize)
    {
      for (int t = 0; t < 9; t++)
      {
        strcpy(historyBuffer[t], historyBuffer[t + 1]); // Shift up all the elements up one spot in the historyBuffer array.
      }
      strcpy(historyBuffer[9], inputBuffer); // Insert the newest command into the index 9 of the history Buffer.
      // success = 1;
    }
    else
    {
      // copy the inputBuffer into the historyBuffer.
      strcpy(historyBuffer[commandNum - 1], inputBuffer);
    }
  }

  for (i = 0; i < length; i++) // examine every character in the inputBuffer
  { 
    switch (inputBuffer[i])
    {
      case ' ':
      case '\t':  // argument separators
        if (start != -1)
        {
          args[ct] = &inputBuffer[start]; // set up pointer
          ct++;
        }
  
        inputBuffer[i] = '\0'; // put in a null to end the argument
        start = -1;
        break;
  
      case '\n' : // same comment as for ' ', '\t' 
        if (start != -1 )
        {
          args[ct] = &inputBuffer[start];
          ct++;
        }
  
        inputBuffer[i] = '\0';
        start = -1;
        args[ct] = NULL;  // no more arguments to this command
        break;
  
      default :  // some other character
  
        if (start == -1)
          start = i;
        if (inputBuffer[i] == '&')
        {
          *flag = 1;
          inputBuffer[i] = '\0';
        }
    } // end of switch
  } // end of for
  
  args[ct] = NULL;
  return success;
} // end of setup routine


// The shellHandler function will print out the historyBuffer commands in order from most recent to least recent with their corresponding CommandNums. 
void shellHandler()
{
  int j,
    amountLoops;
  
  if (commandNum > 9)
  {
    j = commandNum - 9;
    amountLoops = indexTracker + 1;
  }
  else
  {
    j = 1;
    amountLoops = indexTracker;
  }

  printf("j = %d",j);
  
  printf("\n\n");
  printf("Recent Commands\n");
  printf("----------------------");
  
  for (int g = 0; g < amountLoops; g++)
  {
    printf("\n%d.", j++);
    printf("%s", historyBuffer[g]);
  }
  
  printf("----------------------");
  printf("\n\n");
  printf("COMMAND->");
  printf("\n");
}





int main(void)
{
  signal(SIGINT, shellHandler);
  
  int flag; // equal 1 if a command is followed by '&'
  char *args[MAX_LINE/2];  // command line (of 80) must have < 40 arguments
  int child,  // process id of the child process
    status,  // result from execvp system call
    success;
    
  char inputBuffer[MAX_LINE];  // buffer to hold the command entered

  while (1)
  {  // program terminates normally inside setup
    flag = 0;
    printf("\nCOMMAND->\n");
    success = setup(inputBuffer, args, &flag); // get next command
    if (success != 0)
    {
        child = fork();  // creates a duplicate process
        switch(child) 
        {
        case -1:
          perror("could not fork the process");
          break; /* perror is a library routine that displays a system error message, according to the value of the
     system variable "errno" which will be set during  a function (like fork) that was unable to successfully complete
     its task. */
    
        case 0: // here is the child process
          status = execvp(args[0], args);
          if (status != 0)
          {
            perror("error in execvp");
            exit(-2); // terminate this process with error code -2
          }
          break;
    
        default:
          if (flag == 0) // handel parent, wait for child
            while (child !=  wait((int *) 0));
        }
    }
  }
}
