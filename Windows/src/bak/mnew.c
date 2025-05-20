
// ----------------------
//  AMuPLC (mnew.c) clang
// ----------------------

/*

  AMuPLC : Appyo Music Programming Language Compiler
  Copyright (c) 2025 appyo.ht All rights reserved.

  This program is distributed in the hope that it will be useful and/or
  interesting, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  This is released under the MIT license.
  https://opensource.org/licenses/mit-license.php

*/

#include "mnew.h"
int  help;
int  debug;
int  message;

int main(int argc,char *argv[])
//int  argc;
//char *argv[];
{
    int  i,j,k;
    int  ak;
    char *pa;
    char dtopt[STLN];   // debug or trace option 
    int  argfn[MAXA];
    char cmd[256];
    char sfile[STLN];
    char sfn[STLN];
    char tfn[STLN];

    help = 0;
    debug = 0;
    message = 0;
    if(argc == 1) {
        help_main();
        return(0);
    }
    for(i = 0; i < argc; i++) {
        k = i;
        pa = argv[i];
        j = 0;
        argfn[k] = 1;
        while(*pa) {
        dtopt[j] = toupper(*pa);
            j++;
            if(j > 10) break;
            pa++;
        }
        dtopt[j] = '\0';
        if(!strcmp(dtopt,"-H")) help = 1;
        if(!strcmp(dtopt,"-M")) message = 1;
        if(dtopt[0] == '-') argfn[k] = 0;
        if(!strcmp(SUPPORT,"Debug")) {
            if(!strcmp(dtopt,"-D")) debug  = 1;
        }
    }
    if(help) {
	help_main();
	return(0);
    }

    ak = 0;
    for(i = 0; i < argc; i++) {
        if(argfn[i]) {
            ak++;
            if(ak == 2) strcpy(sfile,argv[i]);
        }
    }

    /* ///////Core///////////// */

    /* cp mebrds.dat fnrdf.dat */
    sprintf(tfn,"%srds.dat",sfile);
    sprintf(sfn,"%smebrds.dat",TACTDIR);
    if(debug) printf("sfn=%s tfn=%s\n",sfn,tfn);
    fcp_mnew(sfn,tfn);

    /* cp mebcpj.dat fncpj.dat */
    sprintf(tfn,"%scpj.dat",sfile);
    sprintf(sfn,"%smebcpj.dat",TACTDIR);
    if(debug) printf("sfn=%s tfn=%s\n",sfn,tfn);
    fcp_mnew(sfn,tfn);

    /* cp mebcpi.dat fncpi.dat */
    sprintf(tfn,"%scpi.dat",sfile);
    sprintf(sfn,"%smebcpi.dat",TACTDIR);
    if(debug) printf("sfn=%s tfn=%s\n",sfn,tfn);
    fcp_mnew(sfn,tfn);

    /* cp meb.dat fn.dat */
    sprintf(tfn,"%s.dat",sfile);
    sprintf(sfn,"%smeb.dat",TACTDIR);
    if(debug) printf("sfn=%s tfn=%s\n",sfn,tfn);
    fcp_mnew(sfn,tfn);

    /* //////////////////////// */
    return(0);
}

int fcp_mnew(char *sfn, char *tfn)
{
    FILE *fp,*fpw;
    int  c,i;

    if(debug) printf("sfn=%s tfn=%s\n",sfn,tfn);
    if((fp = fopen(sfn,"r")) == NULL) {
        printf("fcp_mnew() Can't open file %s mode r\n",sfn);
        return(-1);
    } else {
        if((fpw = fopen(tfn,"w")) == NULL) {
            printf("fcp_mnew() Can't open file %s mode w\n",tfn);
            return(-1);
        }
        if(message) printf("mnew %s\n",tfn);
        for(i = 0;(c = fgetc(fp)) != EOF && i < BUFSIZE; i++) {
            fputc(c,fpw);
            if(debug) putchar(c);
        }
        fputc('\0',fpw);
    }
    fclose(fp);
    fclose(fpw);
    return(0);
}

FILE *fopen_h(char *file, char *mode)
//char *file;
//char *mode;
{
    FILE *fp;

    if((fp = fopen(file, mode)) == NULL) {
        printf("Error4! can't open file %s mode %s\n",file, mode);
        exit(1);
    }
    return(fp);
}

int help_main()
{
    int  c;
    FILE *fpd;
    char scrf[100];

    printf( "\n Music data New load V %s-%s-%s\n",VERSION,SUPPORT,OS);
    if(help) {
        if((fpd = fopen("mnew.dat","r")) == NULL) {
            sprintf(scrf,"%s",TACTDIR);
            //sprintf(scrf,"%s\0",TACTDIR);
            strcat(scrf,"mnew.dat");
            fpd = fopen_h(scrf,"r");
        }
        while((c = fgetc(fpd)) != EOF) putchar(c);
        fclose(fpd);
        return(0);
    }
    printf(" mnew fn\n");
    printf("          fn : File name\n");
    printf("      Option : -h(help)\n");
    printf("               -m(message)\n");
    printf(" usage: e.g.\n");
    printf("      mnew -h\n");
    printf("      mnew www -m\n");
    printf("\n");
    return(0);
}
