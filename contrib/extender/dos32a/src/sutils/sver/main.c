/*
 * Copyright (C) 1996-2002 Supernar Systems, Ltd. All rights reserved.
 *
 * Redistribution  and  use  in source and  binary  forms, with or without
 * modification,  are permitted provided that the following conditions are
 * met:
 *
 * 1.  Redistributions  of  source code  must  retain  the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2.  Redistributions  in binary form  must reproduce the above copyright
 * notice,  this  list of conditions and  the  following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. The end-user documentation included with the redistribution, if any,
 * must include the following acknowledgment:
 *
 * "This product uses DOS/32 Advanced DOS Extender technology."
 *
 * Alternately,  this acknowledgment may appear in the software itself, if
 * and wherever such third-party acknowledgments normally appear.
 *
 * 4.  Products derived from this software  may not be called "DOS/32A" or
 * "DOS/32 Advanced".
 *
 * THIS  SOFTWARE AND DOCUMENTATION IS PROVIDED  "AS IS" AND ANY EXPRESSED
 * OR  IMPLIED  WARRANTIES,  INCLUDING, BUT  NOT  LIMITED  TO, THE IMPLIED
 * WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN  NO  EVENT SHALL THE  AUTHORS  OR  COPYRIGHT HOLDERS BE
 * LIABLE  FOR  ANY DIRECT, INDIRECT,  INCIDENTAL,  SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE  GOODS  OR  SERVICES;  LOSS OF  USE,  DATA,  OR  PROFITS; OR
 * BUSINESS  INTERRUPTION) HOWEVER CAUSED AND  ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE)  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <typedefs.h>


	char *version		= "7.2";
	char *errstr		= "SVER fatal:";

	int fileisbound		= TRUE;
	int extendertype	= 0;		// 1=DOS/32A, 2=STUB/32A, 3=STUB32C
	char *fileverptr	= NULL;

	int extender_release;
	int extender_revision;
	int extender_major_version;
	int extender_minor_version;
	int extender_build_version;

	FILE *fp;
	char buffer[512];
	char filename[256];
	char orgfilename[256];


void err_usage() {
	printf("%s syntax is SV <execname.xxx>\n",errstr);
	printf("            use -h or -? to get more help\n");
	exit(1);
}

void err_open() {
	printf("%s cannot open file \"%s\"\n",errstr,filename);
	exit(1);
}

void err_read() {
	printf("%s cannot read from file \"%s\"\n",errstr,filename);
	exit(1);
}

void err_seek() {
	printf("%s error seeking in file \"%s\"\n",errstr,filename);
	exit(1);
}

void err_format() {
	printf("%s unsupported exec format in file \"%s\"\n",errstr,filename);
	exit(1);
}

void err_extender() {
	printf("%s \"%s\" is not a DOS/32A executable\n",errstr,filename);
	exit(1);
}


/****************************************************************************/
void ShowCopyright()
{
	printf("SVER -- SUNSYS Version Report Utility  Version %s\n",version);
	printf("Copyright (C) Supernar Systems, Ltd. 1996-2002\n");
}


void ShowHelp()
{
	printf("\n");
	printf("The SVER Utility Program should be used to determine the version\n");
	printf("of DOS/32 Advanced DOS Extender, STUB/32A Standard Stub file and\n");
	printf("STUB/32C Configurable Stub file.\n");

	printf("\n");
	printf("The  SVER Utility Program  can  also be used to  display any OEM\n");
	printf("(Original  Equipment Manufacturer) Information possibly bound to\n");
	printf("the LC-style executables.\n");

	printf("\n");
	printf("To display version and OEM information run the SVER Utility on a\n");
	printf("program you wish to inspect. Note that the SVER Utility supports\n");
	printf("DOS Extender version 7.0 or higher only.\n");

	printf("\n");
	exit(0);
}


/****************************************************************************/
void OpenFile()
{
	char buf[256];

	strcpy(buf,filename);
	if( (fp=fopen(buf,"rb")) == NULL)
	{
	  strcpy(buf,filename);
	  strcat(buf,".exe");
	  if( (fp=fopen(buf,"rb")) == NULL)
	  {
	    strcpy(buf,filename);
	    strcat(buf,".le");
	    if( (fp=fopen(buf,"rb")) == NULL)
	    {
	      strcpy(buf,filename);
	      strcat(buf,".lx");
	      if( (fp=fopen(buf,"rb")) == NULL)
	      {
	        strcpy(buf,filename);
	        strcat(buf,".lc");
	        if( (fp=fopen(buf,"rb")) == NULL)
	        {
			  err_open();
	        }
	      }
	    }
	  }
	}
	strcpy(orgfilename,buf);
}

void ReadFileHeader()
{
	fread(&buffer,1,sizeof(buffer),fp);
	if(ferror(fp)) err_read();
}

void CheckFileFormat()
{
	if(strncmp(buffer,"MZ",2) == 0) { fileisbound=TRUE; return; }
	if(strncmp(buffer,"LC",2) == 0) { fileisbound=FALSE; return; }
	err_format();
}

void CheckStubExtender()
{

	if(fileisbound==FALSE)
		return;

	if(strcmp(&buffer[0x7C],"DOS/32A") == 0)
	{
		fileverptr=(char *)(buffer+0x85);
		extendertype=1;
		return;
	}
	if(strcmp(&buffer[0x9C],"DOS/32A") == 0)
	{
		fileverptr=(char *)(buffer+0xA5);
		extendertype=1;
		return;
	}
	if(strcmp(&buffer[0x20],"STUB/32A") == 0)
	{
		fileverptr=(char *)(buffer+0x29);
		extendertype=2;
		return;
	}
	if(strcmp(&buffer[0x40],"STUB/32A") == 0)
	{
		fileverptr=(char *)(buffer+0x49);
		extendertype=2;
		return;
	}
	if(strcmp(&buffer[0x4C],"STUB/32C") == 0)
	{
		fileverptr=(char *)(buffer+0x55);
		extendertype=3;
		return;
	}
	if(strcmp(&buffer[0x6C],"STUB/32C") == 0)
	{
		fileverptr=(char *)(buffer+0x75);
		extendertype=3;
		return;
	}
	err_extender();
}


void ShowApplicationName()
{

	printf("\n");
	printf("Application name:\n");
	printf("-----------------\n");
	printf("\"%s\"\n", orgfilename);
}


void ShowExtenderVersion()
{
	char buf[8];

	if(fileisbound==FALSE)
		return;

	memset(buf,0,sizeof(buf));
	buf[0] = *(fileverptr+1);
	extender_release=atoi(buf);

	memset(buf,0,sizeof(buf));
	buf[0] = *(fileverptr+6);
	buf[1] = *(fileverptr+7);
	extender_revision=atoi(buf);

	memset(buf,0,sizeof(buf));
	buf[0] = *(fileverptr+3);
	buf[1] = *(fileverptr+4);
	extender_major_version=atoi(buf);

	memset(buf,0,sizeof(buf));
	buf[0] = *(fileverptr+8);
	buf[1] = *(fileverptr+9);
	extender_minor_version=atoi(buf);

	memset(buf,0,sizeof(buf));
	buf[0] = *(fileverptr+11);
	buf[1] = *(fileverptr+12);
	buf[2] = *(fileverptr+13);
	buf[3] = *(fileverptr+14);
	extender_build_version=atoi(buf);


	printf("\n");
	if(extendertype==1)
	{
		printf("DOS/32 Advanced DOS Extender:\n");
		printf("-----------------------------\n");
	}
	else if(extendertype==2)
	{
		printf("STUB/32A Standard Stub File:\n");
		printf("----------------------------\n");
	}
	else if(extendertype==3)
	{
		printf("STUB/32C Configurable Stub File:\n");
		printf("--------------------------------\n");
	}

	printf("Release:        %d\n",		extender_release);
	printf("Version:        %d.%d\n",	extender_major_version, extender_minor_version);

	if(extender_revision != 0)
	printf("Revision:       [%c]\n",	extender_revision+'A'-1);

	if(extender_build_version != 0)
	printf("Build:          %04d\n",	extender_build_version);
}

void ShowOEMInfo() {
	int n;
	int objects;
	unsigned long offs;
	unsigned long size;

// file type must be MZ or LC
	if(strncmp(buffer,"MZ",2) != 0 && strncmp(buffer,"LC",2) != 0) return;

// if MZ, move to Linear Exec start
	if(strncmp(buffer,"MZ",2) == 0)
	{
		offs=0;
		offs|=(unsigned long)buffer[0x3C];
		offs|=(unsigned long)buffer[0x3D]<<8;
		offs|=(unsigned long)buffer[0x3E]<<16;
		offs|=(unsigned long)buffer[0x3F]<<24;
		if(offs == 0) return;
	}
	else 	offs=0;

	if(fseek(fp,offs,SEEK_SET) != 0) err_seek();
	fread(buffer,1,16,fp);
	if(ferror(fp)) err_read();

// check if LC-style file format
	if(strncmp(buffer,"LC",2) != 0) return;

// check if OEM Info is appended
	if( (buffer[5]&0x80) != 0x80) return;

	objects=buffer[4];

// scan through Objects
	for(n=0; n<objects; n++)
	{
		fread(buffer,1,16,fp);
		if(ferror(fp)) err_read();
		size=0;
		size|=(unsigned long)buffer[4];
		size|=(unsigned long)buffer[5]<<8;
		size|=(unsigned long)buffer[6]<<16;
		size|=(unsigned long)buffer[7]<<24;
		if(fseek(fp,size,SEEK_CUR) != 0) err_seek();
	}

// skip Fixups
	fread(buffer,1,12,fp);
	size=0;
	size|=(unsigned long)buffer[4];
	size|=(unsigned long)buffer[5]<<8;
	size|=(unsigned long)buffer[6]<<16;
	size|=(unsigned long)buffer[7]<<24;
	if(fseek(fp,size,SEEK_CUR) != 0) err_seek();

	memset(buffer,0,sizeof(buffer));
	fread(buffer,1,sizeof(buffer),fp);
	if(ferror(fp)) err_read();

	printf("\n");
	printf("Application OEM Information:\n");

	printf("-------------------------------------------------------------------------------\n");
	printf("%s\n",buffer);
	printf("-------------------------------------------------------------------------------\n");


}


/****************************************************************************/
void main(int argc, char *argv[])
{

	setbuf(stdout, NULL);
	ShowCopyright();

	if(argc<2) err_usage();
	if(	stricmp(argv[1],"-h") == 0 ||
		stricmp(argv[1],"/h") == 0 ||
		stricmp(argv[1],"-?") == 0 ||
		stricmp(argv[1],"/?") == 0
	) ShowHelp();
	strcpy(filename,argv[1]);

	OpenFile();
	ReadFileHeader();
	CheckFileFormat();
	CheckStubExtender();

	ShowApplicationName();
	ShowExtenderVersion();
	ShowOEMInfo();

}


/* R7-07.0300.0967 */
/* 0123456789ABCDEF  */
