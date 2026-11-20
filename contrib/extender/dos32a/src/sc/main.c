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
	char*	errstr="SC/32A fatal:";

	char*	bindname="SB.EXE";
	char*	tempname1="$$SC32$$.111";
	char*	tempname2="$$SC32$$.222";
	char*	titlename="OEMTITLE.INF";
	char*	execstyle[]= { "LE", "LX", "LC", "PE", "PMW1" };
	char*	stubnames[]= { "DOS32A.EXE", "STUB32A.EXE", "STUB32C.EXE" };
	char*	encodetype[] = { "Encoded", "Stored" };
	char	dos32aenv[512];
	char	dos32apath[512];

	char*	bufptr;
	char	oldfilename[80];
	char	newfilename[80];
	char	inffilename[80];
	int	Exec_Type = -1;

	int	execargn = 1;
	int	oldfilesize = 0;
	int	newfilesize = 0;

	int	bind = TRUE;
	int	bindtype = 0;
	int	smart = TRUE;		// auto-encode depending on size
	int	strip = TRUE;		// chop off post-zeroes
	int	encode = TRUE;		// encode Objects
	int	advanced = TRUE;	// pre-relocate exec
	int	verbose = FALSE;
	int	verbxtra = FALSE;	// display extra info
	int	quiet = FALSE;
	int	silent = TRUE;
	int	nocount = FALSE;



/****************************************************************************/
void err_open(char *str) {
	printf("%s cannot open file \"%s\"\n",errstr,str);
	DeleteTempFiles();
	exit(1);
}
void err_seek(char *str) {
	printf("%s error seeking in file \"%s\"\n",errstr,str);
	DeleteTempFiles();
	exit(1);
}
void err_read(char *str) {
	printf("%s error reading from file \"%s\"\n",errstr,str);
	DeleteTempFiles();
	exit(1);
}
void err_write(char *str) {
	printf("%s error writing to file \"%s\"\n",errstr,str);
	DeleteTempFiles();
	exit(1);
}
void err_temp(char *str) {
	printf("%s error creating temp file \"%s\"\n",errstr,str);
	DeleteTempFiles();
	exit(1);
}
void err_mem() {
	printf("%s not enough extended memory\n",errstr);
	DeleteTempFiles();
	exit(1);
}
void err_support(char *str) {
	printf("%s unsupported exec format in file \"%s\"\n",errstr,str);
	DeleteTempFiles();
	exit(1);
}
void err_arg(char *str) {
	printf("%s invalid or misplaced command or option \"%s\"\n",errstr,str);
	DeleteTempFiles();
	exit(1);
}
void err_nosb(void) {
	printf("%s cannot spawn SUNSYS Bind Utility \"%s\"\n",errstr,bindname);
	DeleteTempFiles();
	exit(1);
}
void err_formlc(char *str) {
	printf("%s application \"%s\" is already LC-encoded\n",errstr,str);
	DeleteTempFiles();
	exit(1);
}
void err_formpe(char *str) {
	printf("%s compression of PE-style application \"%s\" is not supported\n",errstr,str);
	DeleteTempFiles();
	exit(1);
}
void err_formpmw1(char *str) {
	printf("%s cannot compress PMW1-compressed application \"%s\"\n",errstr,str);
	DeleteTempFiles();
	exit(1);
}
void err_linker(char *str) {
	printf("%s application \"%s\" was incorrectly linked\n",errstr,str);
	DeleteTempFiles();
	exit(1);
}
void err_lcobjhdr(char *str) {
	printf("%s overflow in object header when encoding \"%s\"\n",errstr,str);
	DeleteTempFiles();
	exit(1);
}
void err_objects(char *str) {
	printf("%s too many Objects in application \"%s\"\n",errstr,str);
	DeleteTempFiles();
	exit(1);
}
void err_relocator(int n) {
	printf("%s an error (#%04X) had occured while preprocessing exec\n",errstr,n);
	DeleteTempFiles();
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
	Print("SC/32A -- Compression Utility version %s\n",version);
	Print("Copyright (C) 1996-2006 by Narech K.\n");
}



/****************************************************************************/
void ArgInit(int argc, char *argv[]) {
	int n,m;
	int argn=18;
	char *args[] = { "bs", "bc", "b", "u", "q", "s",
			 "da", "de", "dr", "ds", "d",
			 "vx", "v2", "v1", "v0", "v", "h", "?" };

	execargn=1;

	if(argc>=2)
	{
	    for(n=1; n<argc; n++)
	    {
		if(*argv[n]=='-' || *argv[n]=='/')
		{
		    for(m=0; m<argn; m++)
		    {
				if(stricmp(argv[n]+1, args[m])==0) switch(m)
				{

					case 0:	execargn++;	// -BS
						bindtype=1;
						goto l0;

					case 1:	execargn++;	// -BC
						bindtype=2;
						goto l0;

					case 2:	execargn++;	// -B
						bindtype=0;
						goto l0;

					case 3:	execargn++;	// -U
						bind=FALSE;
						goto l0;

					case 4:	execargn++;	// -Q
						quiet=TRUE;
						silent=FALSE;
						goto l0;

					case 5:	execargn++;	// -S
						quiet=TRUE;
						silent=TRUE;
						goto l0;

					case 6:	execargn++;	// -DA
						advanced=FALSE;
						goto l0;

					case 7:	execargn++;	// -DE
						encode=FALSE;
						goto l0;

					case 8:	execargn++;	// -DR
						strip=FALSE;
						goto l0;

					case 9:	execargn++;	// -DS
						smart=FALSE;
						goto l0;

					case 10:	execargn++;	// -D
						smart=FALSE;
						strip=FALSE;
						encode=FALSE;
						advanced=FALSE;
						goto l0;

					case 11:			// -VX & -V2
					case 12:	execargn++;
						verbose=TRUE;
						verbxtra=TRUE;
						goto l0;

				   case 13:    	execargn++;	// -V1
						verbose=TRUE;
						verbxtra=TRUE;
						nocount=TRUE;
						goto l0;

				   case 14:    	execargn++;	// -V0
						verbose=TRUE;
						nocount=TRUE;
						goto l0;

					case 15:	execargn++;	// -V
						verbose=TRUE;
						goto l0;

					case 16:
					case 17:	ShowCopyright();
						goto l1;

				}
		    }
		    ShowCopyright();
		    err_arg(argv[n]);
		}
l0:		argn=18;
	    }
	}
	ShowCopyright();

	if(argc<2 || execargn>=argc)
	{
l1:	  Print("SC/32A fatal: syntax is SC [commands] [options] <execname.xxx>\n\n");
	  Print("Commands:\n");
	  Print("---------\n");
	  Print("/B                 Bind DOS/32A to produced LC Executable\n");
	  Print("/BS                Bind STUB/32A to produced LC Executable\n");
	  Print("/BC                Bind STUB/32C to produced LC Executable\n");
	  Print("/U                 Produce Unbound Linear Compressed Executable\n\n");
	  Print("Options:\n");
	  Print("--------\n");
	  Print("/D                 Turn off all optimization features\n");
	  Print("/DA                Disable Advanced preprocessing of Executable\n");
	  Print("/DE                Disable Encoding of LC Executable (no compression)\n");
	  Print("/DR                Disable Removing of post-zero areas from Executable\n");
	  Print("/DS                Disable Smart Encoding when compressing Executable\n");
	  Print("/V[0|1|2]          Verbose mode (displays additional information)\n");
	  Print("/Q                 Quiet mode (partially disables console output)\n");
	  Print("/S                 Silent mode (totally disables console output)\n");
	  Print("/H or /?           This help\n");
	  exit(0);

	}
}



/****************************************************************************/
void OpenExec()
{
	if(open_exec(tempname1)==-1)
	{
		CopyFile(oldfilename,tempname1);
		if(open_exec(tempname1)==-1)
			err_open(tempname1);
	}
}
void CloseExec()
{
	close_exec();
}
void CreateTemp()
{
	if(create_temp()==-1) err_temp(tempname2);
}
int FindExecType()
{
	int n;

	n=get_exec_type();
	if(n==-1)
		err_read(tempname1);
	if(n==-2)
		err_support(oldfilename);
	if(n==3)
		err_formlc(oldfilename);
	if(n==4)
		err_formpe(oldfilename);
	if(n==5)
		err_formpmw1(oldfilename);
	return(n);
}









/****************************************************************************/
void main(int argc, char *argv[])
{
	int n;

//	Debug_Init();

	setbuf(stdout,NULL);
//	CheckEnvironment();
	ArgInit(argc, argv);
	DeleteTempFiles();
	CreateD32AEnvVar();
	CheckExec(argv);
	oldfilesize=GetFileSize(oldfilename);
	UnbindExec();

	OpenExec();
	Exec_Type=FindExecType();
	Print("\n");
	if(!verbose) Print("+ Unbinding Exec     :   Ok.\n");
	else	     Print("+ Unbinding Exec     :   Ok.  (%s-style)\n",execstyle[Exec_Type-1]);
	CloseExec();

	if(advanced)
	{
		if(!quiet) printf("+ Preprocessing Exec :   ");
		n=relocate_exec(tempname1);
		if(n!=0)
		{
			Print("Error!     \n");
			err_relocator(n);
		}
		if(!verbose) Print("Ok.\n");
		else	     Print("Ok.  (%d fixups)\n",relocated_fixups);
	}

	OpenExec();
	CompressExec();
	CloseExec();

	if(bind)
	{
		BindExec();
		if(!verbose) Print("+ Binding Exec       :   Ok.\n");
		else	     Print("+ Binding Exec       :   Ok.  (Stub file used: \"%s\")\n",stubnames[bindtype]);
	}
	else
	{
		unlink(newfilename);
		copy_file(tempname2,newfilename);
		unlink(tempname2);

	}
	newfilesize=GetFileSize(newfilename);
	DeleteTempFiles();

	if(!quiet)
	{
		printf("\n  Original Exec Stats:   \"%s\", %s-style, %d bytes\n", oldfilename, execstyle[Exec_Type-1], oldfilesize);
		printf("Compressed Exec Stats:   \"%s\", %s-style, %d bytes   (%1.1f%%)\n", newfilename, execstyle[2], newfilesize, (float)(newfilesize+0.01)/(float)(oldfilesize+0.01) *100);
	}
	else
	{
		if(!silent) printf("SC/32A: File \"%s\" has been successfully compressed\n", oldfilename);
	}
	exit(0);
}




/****************************************************************************/
void CompressExec()
{
	int n;

	CreateTemp();
	CreateLCHeader();
	CreateLCObjects();
	CreateLCFixups();
	AppendTitleFile();
	close_temp();
	unlink(tempname1);
}


/****************************************************************************/
void CreateLCHeader()
{
	int n;

	Print("+ Creating LC Header :   ");
	n=create_lc_header();
	if(n!=0)
	{
		Print("Error!     \n");
		if(n==-1)
			err_seek(tempname1);
		if(n==-2)
			err_read(tempname1);
		if(n==-3)
			err_write(tempname2);
		if(n==-4)
			err_linker(tempname1);
		if(n==-5)
			err_objects(tempname1);
	}
	else
	{
		if(!verbose)
			Print("Ok.\n");
		else
			Print("Ok.  (%d Objects)\n",app_num_objects);
	}
}


void CreateLCObjects()
{
	int n;
	int obj;
	char c;

	for(obj=1; obj<=app_num_objects; obj++)
	{
		Print("+ Encoding Object #%-2d:   ",obj);

		n=create_lc_object(obj);
		if(n!=0)
		{
			Print("Error!     \n");
			if(n==-1) err_seek(tempname1);
			if(n==-2) err_read(tempname1);
			if(n==-3) err_mem();
			if(n==-4) err_write(tempname2);
			if(n==-5) err_lcobjhdr(tempname1);
		}

		c = app_obj_seekattr[obj] ? '*':' ';

		if(!quiet)
		{
			printf("[û]  ");
			if(!verbose) printf("(%1.1f%%)\n", ((float)(obj_new_size+0.01)/(float)(obj_old_size+0.01))*100);
			else
			{	  		 printf("Old=%7d,   New=%7d,   (%1.1f%%)  (%s)%c\n", obj_old_size,obj_new_size, ((float)(obj_new_size+0.01)/(float)(obj_old_size+0.01))*100, encodetype[app_enc_status], c);
			if(verbxtra) printf("                               ** %s **,   ** %s **,   %7d - fixups\n", app_obj_iscode ? "CODE":"DATA", app_obj_is32bit ? "32bit":"16bit", app_obj_seekattr[obj]);
			}
		}
	}
}


void CreateLCFixups()
{
	int n;

	Print("+ Encoding LC Fixups :   ");
	n=create_lc_fixups();
	if(n!=0)
	{
		Print("Error!\n");
		if(n==-1) err_seek(tempname1);
		if(n==-2) err_read(tempname1);
		if(n==-3) err_mem();
		if(n==-4) err_write(tempname2);
	}

	if(quiet!=TRUE)
	{
		printf("[û]  ");
		if(!verbose) printf("(%1.1f%%)\n", ((float)(obj_new_size+0.01)/(float)(obj_old_size+0.01))*100);
		else	     printf("Old=%7d,   New=%7d,   (%1.1f%%)  (%s)\n", obj_old_size,obj_new_size, ((float)(obj_new_size+0.01)/(float)(obj_old_size+0.01))*100, encodetype[app_enc_status]);
	}
}










/****************************************************************************/
void UnbindExec()
{
	int n;

	char temppath[128];		// passed as *path argument
	char tempname[128];		// passed as *argv[0]
	char tempparm[128];		// name of file to be processed

	const char *path=temppath;
	const char *arg0=bindname;
	const char *arg1="/U";
	const char *arg2="/O";
	const char *arg3="/S";
	const char *arg4=tempparm;
	const char *arg5=oldfilename;


	strcpy(temppath,bindname);
	strcpy(tempname,bindname);
	strcpy(tempparm,"/UN");
	strcat(tempparm,tempname1);
	n=spawnle(P_WAIT, path, arg0,arg1,arg2,arg3,arg4,arg5, NULL, NULL);

	if(n==-1)
	{
		GetDOS32APath();
		strcpy(temppath,dos32apath);
		strcat(temppath,bindname);
		strcpy(tempname,dos32apath);
		strcat(tempname,bindname);
		n=spawnle(P_WAIT, path, arg0,arg1,arg2,arg3,arg4,arg5, NULL, NULL);
		if(n==-1)
		{
			strcpy(temppath,bindname);
			strcpy(tempname,bindname);
			n=spawnlpe(P_WAIT, path, arg0,arg1,arg2,arg3,arg4,arg5, NULL, NULL);
			if(n==-1)
				err_nosb();
		}
	}
	if(n!=0)
	{
		if(n==127)
			err_support(oldfilename);
		else
		{
			DeleteTempFiles();
			exit(1);
		}
	}
}

/****************************************************************************/
void BindExec()
{
	int n;

	char temppath[128];		// passed as *path argument
	char tempname[128];		// passed as *argv[0]
	char tempparm[128];		// -UN name of file
	char tempparm2[128];		// -BN name of file
	char temptype[8];		// action type, either "-R" or "-RS"

	const char *path=temppath;
	const char *arg0=bindname;
	const char *arg1=temptype;
	const char *arg2="/S";
	const char *arg3=tempparm;
	const char *arg4=tempparm2;
	const char *arg5=tempname2;


	if(bindtype==0)
	 	strcpy(temptype,"/R");
	else if(bindtype==1)
		strcpy(temptype,"/RS");
	else if(bindtype==2)
		strcpy(temptype,"/RC");

	strcpy(temppath,bindname);
	strcpy(tempname,bindname);

	strcpy(tempparm,"/UN");
	strcat(tempparm,tempname2);
	strcpy(tempparm2,"/BN");
	strcat(tempparm2,newfilename);
	n=spawnle(P_WAIT, path, arg0,arg1,arg2,arg3,arg4,arg5, NULL, NULL);

	if(n==-1)
	{
		GetDOS32APath();
		strcpy(temppath,dos32apath);
		strcat(temppath,bindname);
		strcpy(tempname,dos32apath);
		strcat(tempname,bindname);
		n=spawnle(P_WAIT, path, arg0,arg1,arg2,arg3,arg4,arg5, NULL, NULL);
		if(n==-1)
		{
			strcpy(temppath,bindname);
			strcpy(tempname,bindname);
			n=spawnlpe(P_WAIT, path, arg0,arg1,arg2,arg3,arg4,arg5, NULL, NULL);
			if(n==-1)
				err_nosb();
		}
	}
	if(n!=0)
	{
		if(n==127)
			err_support(tempname2);
		else
		{
			DeleteTempFiles();
			exit(1);
		}
	}
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
int GetFileSize(char *name)
{
	int n,m;
	if( (n=open(name,O_RDONLY | O_BINARY)) == -1) return(0);
	m=filelength(n);
	close(n);
	return(m);
}
void GetDOS32APath()
{
	char *ptr;
	char *envname;
	char envbuf[512];

	dos32apath[0]=NULL;
	envname=getenv("DOS32A");
	if(envname!=0)
	{
		ptr=strchr(envname,' ');
		if(ptr==NULL)
			ptr=strchr(envname,0);
		memset(envbuf,0,256);
		strncpy(envbuf,envname,(dword)ptr-(dword)envname);
		strcat(envbuf,"\\BINW\\");
		strcpy(dos32apath,envbuf);
	}
	else
		strcat(dos32apath, ".\\");
}
void CreateD32AEnvVar()
{
	int n;
	char *envname;

	envname=getenv("DOS32A");
	if(envname!=0)
	{
		strcpy(dos32aenv,envname);
		strcat(dos32aenv," /NOC");
	}
	else
		strcpy(dos32aenv," /NOC");
	n=setenv("DOS32A",dos32aenv,1);
//	if(n!=0)
//		Print("WARNING: could not adjust DOS32A environment variable\n");
}
void DeleteTempFiles()
{
	close_temp();
	close_exec();
	unlink(tempname1);
	unlink(tempname2);
}
void ShowCount(int count)
{
	if(quiet!=TRUE)
		if(nocount!=TRUE)
			printf("%02d%%\b\b\b",count);
}
void CopyFile(char *f1, char *f2) {
	int c;
	FILE *src;
	FILE *dest;
	unlink(f2);
	if((src=fopen(f1,"rb"))!=NULL)
	{
		if((dest=fopen(f2,"wb"))!=NULL)
		{
			while((c=fgetc(src))!=EOF)
				fputc(c,dest);
			fclose(dest);
		}
		fclose(src);
	}
}
void CheckExec(char *argv[])
{
	char *bufptr;

	strcpy(oldfilename,argv[execargn]);
	strcpy(newfilename,argv[execargn]);
	bufptr=(char *)strchr(newfilename,'.');
	if(bufptr!=NULL)
		strset(bufptr,0);
	if(bind==TRUE)
		strcat(newfilename,".exe");
	else
		strcat(newfilename,".lc");

	if(CheckIfExists(oldfilename)==TRUE)
		return;
	bufptr=(char *)strchr(oldfilename,'.');
	if(bufptr!=NULL)
		return;
	strcat(oldfilename,".exe");

	if(CheckIfExists(oldfilename)==TRUE)
		return;
	bufptr=(char *)strchr(oldfilename,'.');
	if(bufptr!=NULL)
		strset(bufptr,0);
	strcat(oldfilename,".le");

	if(CheckIfExists(oldfilename)==TRUE)
		return;
	bufptr=(char *)strchr(oldfilename,'.');
	if(bufptr!=NULL)
		strset(bufptr,0);
	strcat(oldfilename,".lx");

	if(CheckIfExists(oldfilename)==TRUE)
		return;
	strcpy(oldfilename,argv[execargn]);
}
int CheckIfExists(char *name)
{
	int n;
	n=open(name,O_RDWR | O_BINARY);
	if(n!=-1)
	{
		close(n);
		return(TRUE);
	}
	return(FALSE);
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
			while((c=fgetc(src))!=EOF)
				fputc(c,dest);
			fclose(dest);
		}
		fclose(src);
	}
}

void AppendTitleFile()
{
	int n,m,k;
	char *ptr;
	char *dirptr;
	char tempbuf[512];

	dirptr=inffilename-1;
	strcpy(inffilename,oldfilename);
	for(ptr=inffilename; *ptr!=0; ptr++) if(*ptr=='\\') dirptr=ptr;

	*(dirptr+1)=0;
	strcat(inffilename,titlename);

	m=GetFileSize(inffilename);
	if(m==0 || m>512) return;

	if( (n=open(inffilename,O_RDONLY | O_BINARY)) != -1)
	{
		k=0;
		Print("+ Appending OEM Info :   ");
		memset(tempbuf,0,512);
		if(read(n,&tempbuf,m) == -1) k++;
		if(write_oem_to_temp(temp_handle,&tempbuf,m+1) == -1) k++;
		if(k==0)
		{
			Print("Ok.");
			if(verbose) Print("  (%d bytes)",m);
			Print("\n");
		}
		else
			Print("Error!\n");
		close(n);
	}
}
