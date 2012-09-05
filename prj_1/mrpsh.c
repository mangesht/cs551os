//----------------------------------------------------------
// Illinois Institute of Technology, Chicago
// CS 551 Implementatiom of Operating Systems
// Fall 2012 , Group 3 
// 
//-Description   -------------------------------------------
// This is the mail controler for mrpsh shell. 
//----------------------------------------------------------

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>

int main(int argc,char *argv[]) {
    static int debug_en;
    int i,j,k;
    // Get all the command line parameters 
    for(i=1;i<argc;i++){
        if(strcmp(argv[i],"-debug") == 0 ) {
            debug_en = 1 ;
        }
    }
    // Get the user name 
    char *username;
    username = getenv("USER");
    if(debug_en == 1 ) printf("UserName = %s \n",username);
    

}
