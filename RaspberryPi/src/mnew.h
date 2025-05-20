
// ----------------------
//  AMuPLC (mnew.h) RaspberryPi
// ----------------------

#define VERSION "1.01"
#define SUPPORT "Release"
//#define SUPPORT "Debug"


/*

  AMuPLC : Appyo Music Programming Language Compiler
  Copyright (c) 2025 appyo.ht All rights reserved.

  This program is distributed in the hope that it will be useful and/or
  interesting, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  This is released under the MIT license.
  https://opensource.org/licenses/mit-license.php

*/

#define OS "RaspberryPi"
//#define OS "FreeBSD"
#define TACTDIR "/usr/local/etc/tact/"

/*
#define OS "Windows"
#define TACTDIR "C:/Program Files/AMuPLC/dat/"
*/

/*
#define OS "Mac"
#define TACTDIR "/var/AMuPLC/"
*/

/*
#define OS "CentOS"
#define TACTDIR "/var/www/local/etc/tact/"
*/

#define COMPUTER_TYPE "IBMPC"
/*
#define COMPUTER_TYPE "SUN"
*/

#define MS_C      0
#define UNIX_C    1

#if UNIX_C == 1
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#endif

#define MAXA  20        /* max. argument number */
#define STLN  80        /* string get length */
#define BUFSIZE   80*2200    // Message Window size
#define CR    0x0d
#define LF    0x0a
#define BS    0x08
#define ESC   0x1b

// main()
int main(int argc,char *argv[]);
int exec_command(char *command, char *tfn);
FILE *fopen_h(char *file, char *mode);

//$$$$$$$$$$$$$$$$$$$$$$//

