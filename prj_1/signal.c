//-------------------------------------------------------------
// Illinois Institute of Technology, Chicago
// CS 551 Implementation of Operating Systems
// Fall 2012,  Group 3
//
// --- Description --------------------------------------------
// Function module to handle Control-C Signal Handler ---------
//-------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>

#undef DEBUG
#define DEBUG 1 

// GLOBAL VARIABLES
jmp_buf env;
int state;


/* Function: signal_handler
 * Brief : Signal handler for control-C ( SIGINT -2 )
*/

void signal_handler( int signal )
{
   char ch;

   if ( DEBUG == 1 )
      printf ( "Signal Handler Called for signal %d\n", signal);

   if ( signal == SIGINT )
   {
      printf ("\n\n Are you sure you want to exit ? Press [y] to exit or any other key to continue: ");
      ch = getchar();

      if ( ch == 'y' || ch == 'Y')
      {
         // call cleanUp function to free any allocated memory
         exit (0);
      }
      else  
      {
         if ( ch != '\n')
            while ( (ch = getchar()) != '\n');
         longjmp (env, state);
      }
   }
   
}


int main () 
{
  int i =0;

  //Register signal and signal_handler. 
  //This piece of code should be copied in the main function. 
  signal (SIGINT, signal_handler);
  state = setjmp ( env );


  // Here goes the normal pgm, Below is just a Test sample pgm
  while ( 1 )
  {
     printf ( " Signal handle test %d\n", i);
     sleep (1);
     i++;
  }
  return 1;

}
