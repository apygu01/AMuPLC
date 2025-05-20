// ----------------------
//  AMuPLC (meg.h) RaspberryPi
// ----------------------

#define VERSION "1.11"
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
#define SUPPORT "Release"
#define TACTDIR "/var/www/local/etc/tact/"
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define Cr '\0'

#define MAXA  20        /* max. argument number */
#define STLN  20        /* string get length */
#define MEXP  800       /* max. expression number (megsym.dat size) */
#define MTMP  100       /* max. temp sign. */
#define MSTC  100       /* max. card stack size */
#define MMST  100       /* max. memory symbol tabel */
#define CR    0x0d
#define LF    0x0a
#define BS    0x08
#define ESC   0x1b

// main.c
int multif_h(char *infn,char *megf,char *ftyp,int fnmb,int *prtn);
FILE *fopen_h(char *file, char *mode);
int strcat_h(char *tos, char *from);
int fhelp_meg();

// meg1.c
int scorechk_meg(int argc,char *argv[],char mfile[],int list);
int barmake_meg(int argc,char *argv[],char xfile[],int,int,int); 
int condmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track);
int condhed_meg(int list);
int lowbmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track);
int midbmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track);
int higbmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track);
int rtmbmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track);
int dodbmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track);
int synbmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track);
int tcfbmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track);
int gcfbmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track);
int altbmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track);
int basbmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track);
int ubsbmake_meg(int argc,char *argv[],char xfile[STLN],int list,int track);
int mbarout_meg(char *barop,int list);
int xbarout_meg(char *barop,char bkind,int list);
int midbar_meg(int barn,int list);
int lowbar_meg(int barn,int list);
int highbar_meg(int barn,int list);
int rtmbar_meg(int barn,int list);
int dodbar_meg(int barn,int list);
int synsbar_meg(int barn,int list);
int tclfbar_meg(int barn,int list);
int gclfbar_meg(int barn,int list);
int altbar_meg(int barn,int list);
int basbar_meg(int barn,int list);
int ubsbar_meg(int barn,int list);
int xcondle_meg(int barn,int list);
int mvlout_meg(int list);
int smvlout_meg(int list);
int xmvlout_meg(int list);
int finout_meg(char title[22],int list);
int condtitl_meg(int argc,char *argv[],int list);
int prbtitl_meg(int list);

// meg2.c
int scoremk_meg(int argc,char *argv[],int list);
int parthed_meg(int list);

// ====================================================
