****** FS Tool folder consists of 
1) source code *.c 
2) executable tool 
3) Test Scripts and test data

The directory and file structure is

- cp_svn_to_src
- compile 
- Readme.txt
+ src 
    - callnr.h
    - fs_tool.c
    - fs_tool_lib.c
    - Makefile
    + vfs    
        - request.c
    	- proto.h
    	- table.c
    +mfs
        - misc.c
        - proto.h
        - table.c
        - Makefile
    +hgfs
        - table.c
    +src-lib-libvtreefs
        - table.c
+tool
    - fileinfo.c
    - Makefile
+test
    - open_multi.c 
+test_data
    - Makefile



How to compile ?  
- Run the following scripts to compile and make new OS image 
- ./cp_svn_to_src
- ./compile
The above command would create the new minix image that supports newly added system call for file system information tool.
- Goto $PRJ/tool
- make tool
- make link

How to invoke the calls ? 
Your shell now supports fileinfo command with parameters -p -b -h and filename about which you want to get info.
Ex: fileinfo /home/mthakare/ReadMe.txt
fraginfo with options as [-i] for internal fragmentation ,[-e] for internal fragmentation

Ex: fraginfo -i 
Ex: fraginfo -e 

Hope you find the information useful, for details please contact:
mthakare@hawk.iit.edu
rbaskar@hawk.iit.edu
pchikka1@hawk.iit.edu


