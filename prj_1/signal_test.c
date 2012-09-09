#include "signal.c"

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
