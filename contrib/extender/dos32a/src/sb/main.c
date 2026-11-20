/*
 * Copyright (C) 1996-2006 by Narech K. All rights reserved.
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

#include "main.h"

	char*	version = "9.1.2";
	char	newname[80];
	char	newname2[80];
	char	filename[80];
	char	name_bn[80];
	char	name_un[80];
	char*	bufptr;
	char*	bufptr2;
	char*	fileptr;

	char*	stubname;
	char*	stubname1	= "DOS32A.EXE";
	char*	stubname2	= "STUB32A.EXE";
	char*	stubname3	= "STUB32C.EXE";
	char*	errstr		= "SB/32A fatal:";
	char*	tempname	= "$$SB32$$.TMP";

	int	execargn = 1;
	int	filesize = 0;
	int	oldfilesize = 0;
	int	newfilesize = 0;

	int	Main_Type		= -1;
	int	Exec_Type		= -1;
	int	Extender_Type	= -1;

	int	bind		= FALSE;
	int	unbind		= FALSE;
	int	overwrite	= FALSE;
	int	quiet		= FALSE;
	int	silent		= TRUE;
	int	bind_name	= FALSE;
	int	unbind_name	= FALSE;




/****************************************************************************/
void err_open(char *str) {
	printf("%s cannot open file \"%s\"\n",errstr,str);
	exit(1);
}
void err_rdonly(char *str) {
	printf("%s cannot open Read-Only file \"%s\"\n",errstr,str);
	exit(1);
}
void err_seek(char *str) {
	printf("%s error seeking in file \"%s\"\n",errstr,str);
	exit(1);
}
void err_read(char *str) {
	printf("%s error reading from file \"%s\"\n",errstr,str);
	exit(1);
}
void err_support(char *str) {
	printf("%s unsupported exec format in file \"%s\"\n",errstr,str);
	exit(1);
}
void err_dest(char *str) {
	printf("%s destination file \"%s\" already exists\n",errstr,str);
	exit(1);
}
void err_mem(char *str) {
	printf("%s not enough memory to load file \"%s\"\n",errstr,str);
	exit(1);
}
void err_crtmp(void) {
	printf("%s error creating temp file\n",errstr);
	exit(1);
}
void err_wrtmp(void) {
	printf("%s error writing to temp file\n",errstr);
	exit(1);
}
void err_rdstub(void) {
	printf("%s error reading from stub file\n",errstr);
	exit(1);
}
void err_invstub(void) {
	printf("%s invalid stub file format\n",errstr);
	exit(1);
}
void err_nod32a(void) {
	printf("%s cannot find file \"DOS32A.EXE\"\n",errstr);
	exit(1);
}
void err_nostub(char *str) {
	printf("%s cannot find file \"%s\"\n",errstr,str);
	exit(1);
}
void err_arg(char *str) {
	printf("%s invalid or misplaced command or option \"%s\"\n",errstr,str);
	exit(1);
}
void err_sameact(void) {
	printf("%s cannot Bind and Unbind at the same time\n",errstr);
	exit(1);
}
void err_nullname(void) {
	printf("%s you must specify a file name with /BN or /UN options\n",errstr);
	exit(1);
}
void err_environment(void) {
	printf("%s DOS/32A environment variable is not set up properly\n",errstr);
	printf("You need to reinstall DOS/32 Advanced DOS Extender on this computer\n");
	exit(1);
}



/****************************************************************************/
void ShowCopyright()
{
	Print("SB/32A -- Bind Utility version %s\n",version);
	Print("Copyright (C) 1996-2006 by Narech K.\n");
}



void ArgInit(int argc, char *argv[])
{
	int n,m;
	int argn=14;
	char *args[] = { "bs", "bc", "bn", "rs", "rc", "un", "b", "r", "u", "o", "q", "s", "h", "?" };

	execargn=1;

	if(argc>=2)
	{
	    for(n=1; n<argc; n++)
	    {
			if(*argv[n]=='-' || *argv[n]=='/')
			{
				for(m=0; m<argn; m++)
				{
					if(strnicmp(argv[n]+1, args[m], strlen(args[m]))==0)
						switch(m)
						{

							case 0:	execargn++;
								bind=TRUE;
								stubname=stubname2;
								goto l0;

							case 1:	execargn++;
								bind=TRUE;
								stubname=stubname3;
								goto l0;

							case 2:	execargn++;
								bind_name=TRUE;
								strcpy(name_bn,argv[n]+3);
								if(strlen(name_bn)==0)
								{
									ShowCopyright();
									err_nullname();
								}
								goto l0;

							case 3:	execargn++;
								bind=TRUE;
								stubname=stubname2;
								overwrite=TRUE;
								goto l0;

							case 4:	execargn++;
								bind=TRUE;
								stubname=stubname3;
								overwrite=TRUE;
								goto l0;

							case 5:	execargn++;
								unbind_name=TRUE;
								strcpy(name_un,argv[n]+3);
								if(strlen(name_un)==0)
								{
									ShowCopyright();
									err_nullname();
								}
								goto l0;

							case 6:	execargn++;
								bind=TRUE;
								stubname=stubname1;
								goto l0;

							case 7:	execargn++;
								bind=TRUE;
								stubname=stubname1;
								overwrite=TRUE;
								goto l0;

							case 8:	execargn++;
								unbind=TRUE;
								goto l0;

							case 9:	execargn++;
								overwrite=TRUE;
								goto l0;

							case 10:	execargn++;
								quiet=TRUE;
								silent=FALSE;
								goto l0;

							case 11:	execargn++;
								quiet=TRUE;
								silent=TRUE;
								goto l0;

							case 12:
							case 13:	ShowCopyright();
								goto l1;

						}
				}
				ShowCopyright();
				err_arg(argv[n]);
			}
l0:			argn=14;
	    }
	}
	ShowCopyright();

	if(argc<2 || execargn>=argc)
	{
l1:	  Print("SB/32A fatal: syntax is SB [commands] [options] <execname.xxx>\n\n");
	  Print("Commands:\n");
	  Print("---------\n");
	  Print("/B                 Bind DOS/32A to Linear Executable\n");
	  Print("/BS                Bind STUB/32A to Linear Executable\n");
	  Print("/BC                Bind STUB/32C to Linear Executable\n");
	  Print("/R                 Replace existing Stub/Extender with DOS/32A\n");
	  Print("/RS                Replace existing Stub/Extender with STUB/32A\n");
	  Print("/RC                Replace existing Stub/Extender with STUB/32C\n");
	  Print("/U                 Unbind Linear Executable from the existing Stub\n\n");
	  Print("Options:\n");
	  Print("--------\n");
	  Print("/BNfilename.xxx    Specify destination file name when Binding\n");
	  Print("/UNfilename.xxx    Specify destination file name when Unbinding\n");
	  Print("/O                 Unconditionally Overwrite existing files\n");
	  Print("/Q                 Quiet mode (partially disables console output)\n");
	  Print("/S                 Silent mode (totally disables console output)\n");
	  Print("/H or /?           This help\n");
	  exit(1);
	}
}


void OpenExec(char *argv[])
{
	int n,m;

	strcpy(filename,argv[execargn]);
	n=open_exec(filename);

	if(n==-2)
		err_rdonly(argv[execargn]);
	if(n!=0)
	{
		strcpy(filename,argv[execargn]);
		strcat(filename,".exe");
		n=open_exec(filename);
		if(n==-2)
			err_rdonly(argv[execargn]);
		if(n!=0)
		{
			strcpy(filename,argv[execargn]);
			strcat(filename,".le");
			n=open_exec(filename);
			if(n==-2)
				err_rdonly(argv[execargn]);
			if(n!=0)
			{
				strcpy(filename,argv[execargn]);
				strcat(filename,".lx");
				n=open_exec(filename);
				if(n==-2)
					err_rdonly(argv[execargn]);
				if(n!=0)
				{
					strcpy(filename,argv[execargn]);
					strcat(filename,".lc");
					n=open_exec(filename);
					if(n==-2)
						err_rdonly(argv[execargn]);
				}
			}
		}
	}
	if(n==-1)
		err_open(argv[execargn]);
}




/****************************************************************************/
int GetExecType(char *argv[])
{
	int n;

	n=check_if_unbound_exec();

	if(n==-1)
		err_seek(argv[execargn]);
	else if(n==-2)
		err_read(argv[execargn]);
	else if(n==-3)
		err_support(argv[execargn]);

	return(n);
}

int FindExecType(char *argv[])
{
	int n;

	n=get_exec_start();
	if(n==-1)
		err_read(argv[execargn]);
	else if(n==-2)
		return(-1);

	n=get_exec_type();
	if(n==-1)
		err_read(argv[execargn]);
	else if(n==-2)
		return(-1);

	return(n);
}

int GetExtenderType(char *argv[])
{
	int n;

	n=get_extender_type();
	if(n==-1)
		err_read(argv[execargn]);

	return(n);
}










/****************************************************************************/
void main(int argc, char *argv[])
{
	int n=-1;
	int m=-1;
	int l=-1;
	int e=-1;
	int x,y;
	char buf[80];

	ArgInit(argc, argv);
	OpenExec(argv);
	filesize=filelength(exec_handle);
	n=GetExecType(argv);

	if(n==0)
	{
		m=FindExecType(argv);
		l=GetExtenderType(argv);
	}

	Main_Type=n;		/* file format: LE, LX, LC, PE */
	Exec_Type=m;		/* same as "n" when bound */
	Extender_Type=l;	/* Stub/Extender type */

	Print("\n");
	Print("Application Name:   \"%s\"\n",argv[execargn]);
	Print("Application Size:   %d bytes\n",filesize);
	Print("Application Type:  ");
	if(n!=0)
	{
		if(n==1) Print(" Unbound LE-style file format Linear Executable");
		if(n==2) Print(" Unbound LX-style file format Linear Executable");
		if(n==3) Print(" Unbound LC-style file format Linear Executable");
		if(n==4) Print(" Unbound PE-style file format Linear Executable");
		if(n==5) Print(" Unbound PMW1-style file format Linear Executable");
	}
	else
	{
		if(m==-1)
		{
			Print(" Standard DOS Executable\n");
			close_exec();
			exit(127);	/* signal error to SC compressor */
		}
		if(l==0) Print(" Unknown Stub File bound to\n");
		if(l==1) Print(" DOS/32 Advanced DOS Extender bound to\n");
		if(l==2) Print(" STUB/32C Configurable Stub File bound to\n");
		if(l==3) Print(" STUB/32A Standard Stub File bound to\n");
		if(l==4) Print(" DOS/4G DOS Extender bound to\n");
		if(l==5) Print(" PMODE/W DOS Extender bound to\n");

		if(m==1) Print("                    LE-style file format Linear Executable");
		if(m==2) Print("                    LX-style file format Linear Executable");
		if(m==3) Print("                    LC-style file format Linear Executable");
		if(m==4) Print("                    PE-style file format Portable Executable");
		if(m==5) Print("                    PMW1-style file format Linear Executable");
	}
	Print("\n");

	if(n==3 || m==3)
		DisplayOEMInfo();

	unlink(tempname);

	if(unbind_name==FALSE)
	{
		strcpy(newname,filename);
		bufptr=(char *)strchr(newname,'.');
		if(bufptr!=NULL) strset(bufptr,0);
		if(Main_Type==1 || Exec_Type==1) strcat(newname,".le");
		if(Main_Type==2 || Exec_Type==2) strcat(newname,".lx");
		if(Main_Type==3 || Exec_Type==3) strcat(newname,".lc");
		if(Main_Type==4 || Exec_Type==4) strcat(newname,".pe");
		if(Main_Type==5 || Exec_Type==5) strcat(newname,".pmw");
	}
	else
		strcpy(newname,name_un);

	if(bind_name==FALSE)
	{
		strcpy(newname2,filename);
		bufptr2=(char *)strchr(newname2,'.');
		if(bufptr2!=NULL)
			strset(bufptr2,0);
		strcat(newname2,".exe");
	}
	else
		strcpy(newname2,name_bn);


	if(bind==TRUE && unbind==TRUE)
		err_sameact();
	if(bind)
	{
		BindExec(argv);
		if(!silent) printf("SB/32A: File \"%s\" has been successfully bound\n", filename);
	}
	else if(n==0) if(unbind)
	{
		UnbindExec(argv);
		if(!silent) printf("SB/32A: File \"%s\" has been successfully unbound\n", filename);
	}

	close_exec();
	exit(0);
}










void UnbindExec(char *argv[])
{
	int n,m;

	Print("\n");
	Print("  Unbinding file:   \"%s\"\n",filename);
	Print("Destination file:   \"%s\"\n",newname);
	CheckIfExists(newname);

	n=unbind_exec();
	if(n==-1) err_mem(filename);
	if(n==-2) err_read(filename);
	if(n==-3) err_crtmp();
	if(n==-4) err_wrtmp();

	unlink(newname);
	copy_file(tempname,newname);
	unlink(tempname);

	oldfilesize=GetFileSize(newname);
	if(quiet!=TRUE)
		printf("Destination size:   %d bytes (%1.1f%%)\n", oldfilesize, (float)(oldfilesize+0.01)/(float)(filesize+0.01) *100);
}


void BindExec(char *argv[])
{
	int n,m;
	int stubsize;
	int execsize;
	int stubhandle;
	int exechandle;
	char *ptr;
	char *envname;
	char envbuf[256];

	CheckIfExists(newname2);
	if(Main_Type==0)
		UnbindExec(argv);

	Print("\n");
	Print("    Binding file:   \"%s\"\n",newname);
	Print("Destination file:   \"%s\"\n",newname2);

	stubhandle=open(stubname,O_RDWR | O_BINARY);
	if(stubhandle==-1)
	{
		envname=getenv("DOS32A");
		if(envname==0)
			err_nod32a();
		ptr=strchr(envname,' ');
		if(ptr==NULL)
			ptr=strchr(envname,0);
		memset(envbuf,0,256);
		strncpy(envbuf,envname,(dword)ptr-(dword)envname);
		strcat(envbuf,"\\BINW\\"); strcat(envbuf,stubname);
		stubhandle=open(envbuf,O_RDWR | O_BINARY);

		if(stubhandle==-1)
			err_nostub(envbuf);
	}
	stubsize=filelength(stubhandle);

	exechandle=open(newname,O_RDWR | O_BINARY);
	if(exechandle==-1)
		err_open(newname);

	execsize=filelength(exechandle);

	n=bind_exec(stubhandle, exechandle, stubsize, execsize);
	close(exechandle);
	close(stubhandle);
	unlink(newname);
	close_exec();
	switch(n)
	{
		case -1:	err_mem(newname);
		case -2:	err_rdstub();
		case -3:	err_read(newname);
		case -4:	err_crtmp();
		case -5:	err_wrtmp();
		case -6:	err_invstub();
	}

	unlink(newname2);
	copy_file(tempname,newname2);
	unlink(tempname);

	newfilesize=GetFileSize(newname2);
	if(quiet!=TRUE)
		printf("Destination size:   %d bytes (%1.1f%%)\n", newfilesize, (float)(newfilesize+0.01)/(float)(filesize+0.01) *100);
	if(quiet!=TRUE)
		printf("  Stub file used:   \"%s\"\n",stubname);
}



/****************************************************************************/
void Print(char *format, ...)
{
	va_list arglist;
	char buffer[1024];
	if(quiet==FALSE)
	{
		va_start(arglist,format);
		vsprintf(buffer,format,arglist);
		printf(buffer);
		va_end(arglist);
	}
}
void CheckIfExists(char *name)
{
	int n;
	n=open(name,O_RDWR | O_BINARY);
	if(n!=-1)
	{
		close(n);
		if(!overwrite)
			err_dest(name);
	}
}
int GetFileSize(char *name)
{
	int n;
	int m=0;
	n=open(name,O_RDWR | O_BINARY);
	if(n!=-1)
	{
		m=filelength(n);
		close(n);
	}
	return(m);
}
void CheckEnvironment()
{
	if(getenv("DOS32A")==NULL)
		err_environment();
}
void copy_file(char *f1, char *f2)
{
	int c;
	FILE *src;
	FILE *dest;

	if( (f1[1] != ':') && (f2[1] != ':') )
	{
		rename(f1,f2);
		return;
	}

	if((src=fopen(f1,"rb"))!=NULL)
	{
		if((dest=fopen(f2,"wb"))!=NULL)
		{
			while((c=fgetc(src))!=EOF) fputc(c,dest);
			fclose(dest);
		}
		fclose(src);
	}
}

void DisplayOEMInfo()
{

	char *ptr;

	if((ptr=find_oem_info()) == NULL)
		return;

	Print("\n");
	Print("Application OEM Information:\n");

	Print("-------------------------------------------------------------------------------\n");
	Print("%s\n",ptr);
	Print("-------------------------------------------------------------------------------\n");
}

