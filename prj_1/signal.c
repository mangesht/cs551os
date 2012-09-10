//-------------------------------------------------------------
// Illinois Institute of Technology, Chicago
// CS 551 Implementation of Operating Systems
// Fall 2012,  Group 3
//
// --- Description --------------------------------------------
// Function module to handle Control-C Signal Handler ---------
//-------------------------------------------------------------

//#include <setjmp.h>

// GLOBAL VARIABLES
//jmp_buf env;
//int state;

void signal_handler( int signal );

void register_signal(){
  signal (SIGINT, signal_handler);
  //state = setjmp ( env );

}


/* Function: signal_handler
 * Brief : Signal handler for control-C ( SIGINT -2 )
*/

void signal_handler( int signal )
{
   char ch;

   if(debug_en) printf ( "Signal Handler Called for signal %d\n", signal);

   if ( signal == SIGINT )
   {
      printf ("Are you sure you want to exit ? Press [y] to exit or any other key to continue: ");
      ch = getchar();

      if ( ch == 'y' || ch == 'Y')
      {
         // call cleanUp function to free any allocated memory
         exit (EXIT_SUCCESS);
      }
      else  
      {
         if ( ch != '\n')
            while ( (ch = getchar()) != '\n');
         //longjmp (env, state);
      }
   }
   
}



//#include "signal.c"
/*

int main () 
{
  int i =0;

  //Register signal and signal_handler. 
  //This piece of code should be copied in the main function. 
  //signal (SIGINT, signal_handler);
  //state = setjmp ( env );
  register_signal();


  // Here goes the normal pgm, Below is just a Test sample pgm
  while ( 1 )
  {
     printf ( " Signal handle test %d\n", i);
     sleep (1);
     i++;
  }
  return 1;

}

*/
