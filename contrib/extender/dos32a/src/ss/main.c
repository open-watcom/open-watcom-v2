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

#include "main.h"


void main(int argc, char *argv[])
{
	int n;

//	Debug_Init();

	memset(startdir,0,sizeof(startdir));
	memset(cfgfilename,0,sizeof(cfgfilename));
	ArgInit(argc, argv);
	VideoInit();
	ShowCopyright(argc, argv);
	CheckIfLocked();
	ShowSysInfo();
	ShowEnvInfo();
	ShowMainMenu();
	ShowMemory();
	CheckVersion();
	ValidateConfig();

l1:	ShowMemory();
	SelectMainMenu();
l2:	switch(mainmenu_sel)
	{
		case 0:		ShowKernelMenu();
					if(keycode==F2)
					{
						ShowMemory();
						mainmenu_sel=1;
						goto l2;
					} else break;
		case 1:		ShowExtenderMenu();
					if(keycode==F1)
					{
						ShowMemory();
						mainmenu_sel=0;
						goto l2;
					} else break;
		case 2:		CreateConfig(); break;
		case 3:		RestoreConfig();break;
		case 4:		id32=id32_old; ClearConfigName(); break;
		case 5:		DiscardExit(); break;
		case 6:		ApplyExit(); break;
	}
	goto l1;
}




/****************************************************************************/
void ArgInit(int argc, char *argv[])
{
	int n;
	char *ptr;
	char *envname;
	char envbuf[256];
	char buf[80];
	char buf2[80];

	if(argc==4)
	{
	  if(stricmp(argv[3],"/q")==0 ||
		stricmp(argv[3],"-q")==0 ||
		stricmp(argv[3],"/quiet")==0 ||
		stricmp(argv[3],"-quiet")==0) goto l1;

	  if(stricmp(argv[3],"/s")==0 ||
		stricmp(argv[3],"-s")==0 ||
		stricmp(argv[3],"/silent")==0 ||
		stricmp(argv[3],"-silent")==0) { silent=TRUE; goto l1; }
	}


	printf("SS/32A -- Protected Mode Setup Utility  Version %s\n",version);
	printf("Copyright (C) Supernar Systems, Ltd. 1996-2002\n");

l1:	if(argc<2)
	{
l2:	  printf("%s syntax is SS <execname.xxx> [config.d32 | command] [option]\n\n", errstr);
	  printf("Commands:\n");
	  printf("---------\n");
	  printf("/I or /INFO        Write DOS/32A Configuration to console\n");
	  printf("/L or /LOCK        Lock Configuration in <execname.xxx> file\n");
	  printf("/U or /UNLOCK      Unlock Configuration in <execname.xxx> file\n\n");
	  printf("Options:\n");
	  printf("--------\n");
	  printf("/Q or /QUIET       Quiet mode (partially disables console output)\n");
	  printf("/S or /SILENT      Silent mode (totally disables console output)\n");
	  printf("/H or /?           This help\n");
	  exit(1);
	}
	if(	stricmp(argv[1],"/h")==0 ||
		stricmp(argv[1],"-h")==0 ||
		stricmp(argv[1],"-?")==0) goto l2;

	strcpy(buf,argv[1]);
	n=ReadHeader(buf,&id32);
	if(n==1)
	{
		strcat(buf,".exe");
		n=ReadHeader(buf,&id32);
	}
	if(n==1) { printf("%s cannot open file \"%s\"\n", errstr, argv[1]); exit(1); }
	if(n==2) { printf("%s cannot read from file \"%s\"\n", errstr, argv[1]); exit(1); }
	if(n==3) { printf("%s unsupported executable format in file \"%s\"\n", errstr, argv[1]); exit(1); }
	if(id32.id!='23DI') { printf("%s \"%s\" is not a DOS/32A executable\n", errstr, argv[1]); exit(1); }
	id32_old=id32;

	if(argc>2)
	{
		if(	stricmp(argv[2],"-lock")==0 ||
			stricmp(argv[2],"/lock")==0 ||
			stricmp(argv[2],"-l")==0 ||
			stricmp(argv[2],"/l")==0)
		{
			id32.dos32a_misc2=((id32.dos32a_misc2&0x3F)|0x40);
			if(WriteHeader(&id32)==0) if(!silent) printf("SS/32A: Configuration has been successfully locked!\n");
			else printf("SS/32A: Could not lock configuration!\n");
			CloseFile();
			exit(0);
		}
		else
		if(	stricmp(argv[2],"-unlock")==0 ||
			stricmp(argv[2],"/unlock")==0 ||
			stricmp(argv[2],"-u")==0 ||
			stricmp(argv[2],"/u")==0)
		{
			id32.dos32a_misc2=((id32.dos32a_misc2&0x3F)&0xBF);
			if(WriteHeader(&id32)==0) if(!silent) printf("SS/32A: Configuration has been successfully unlocked!\n");
			else printf("SS/32A: Could not unlock configuration!\n");
			CloseFile();
	 		exit(0);
		}
		else
		if(	stricmp(argv[2],"-info")==0 ||
			stricmp(argv[2],"/info")==0 ||
			stricmp(argv[2],"-i")==0 ||
			stricmp(argv[2],"/i")==0)
		{
			ShowHeaderInfo();
			CloseFile();
			exit(0);
		}
		if((id32.dos32a_misc2&0x40)==0x40)
		{
			printf("SS/32A: Configuration in file \"%s\" is locked!\n",buf);
			exit(0);
		}


		n=open(argv[2],O_RDONLY | O_BINARY);
		if(n==-1)
		{
			strcpy(buf2,argv[2]);
			strcat(buf2,".d32");
			n=open(buf2,O_RDONLY | O_BINARY);
			if(n==-1)
			{
				envname=getenv("DOS32A");
				if(envname!=NULL)
				{
					ptr=strchr(envname,' ');
					if(ptr==NULL) ptr=strchr(envname,0);
					memset(envbuf,0,256);
					strncpy(envbuf,envname,(dword)ptr-(dword)envname);
					strcat(envbuf,"\\D32\\"); strcat(envbuf,buf2);
					n=open(envbuf,O_RDWR | O_BINARY);
				}
			}
		}
		if(n!=-1)
		{
			read(n,&id32,24);
			close(n);
			if(id32.id!='23DI') { printf("%s \"%s\" is not a DOS/32A configuration file\n", errstr, argv[2]); exit(1); }
			id32.dos32a_misc2=((id32.dos32a_misc2&0x3F) | (id32_old.dos32a_misc2&0xC0));
			if(WriteHeader(&id32)!=0) { printf("%s could not configure file \"%s\"\n", errstr, buf); exit(1); }
			CloseFile();
			if(!silent) printf("SS/32A: File \"%s\" has been successfully configured\n",buf);
			exit(0);
		}
		else
		{
			printf("%s cannot open configuration file \"%s\"\n", errstr, argv[2]);
			exit(1);
		}
	}
	file_type=GetFileType();
	extn_type=GetExtenderType();
}




/****************************************************************************/
void ShowCopyright(int argc, char *argv[])
{
	char buf[80];
	DrawBackground();
	SetColor(YELLOW);
	SetBackColor(BLUE);
	Print_At(0,31,"þþþ Setup Utility þþþ");
	SetColor(LIGHTBLUE);
	Print_At(0,1,"DOS/32 Advanced.");
	Print_At(0,67,"Version %s",version);
	Print_At(24,16,"Copyright (C) Supernar Systems, Ltd. 1996-2002");

	strcpy(buf,argv[1]);
	strupr(buf);
	Print_At(24,14,"³");
	Print_At(24,64,"³");
	SetColor(LIGHTWHITE);
	Print_At(24,1,"%.13s",buf);

}

void ShowHeaderInfo()
{
	printf("DOS/32A Header Info\n");
	printf("----------------------------\n");
	printf("Header signature:	%.4s\n",&id32.id);
	printf("Kernel misc. byte:	0x%02X\n",id32.kernel_misc);
	printf("Kernel PageTables #:	0x%02X\n",id32.kernel_pagetables);
	printf("Kernel PhysTables #:	0x%02X\n",id32.kernel_phystables);
	printf("Kernel Callbacks #:	0x%02X\n",id32.kernel_callbacks);
	printf("Kernel Selectors #:	0x%04X\n",id32.kernel_selectors);
	printf("Kernel RM stacks #:	0x%02X\n",id32.kernel_rmstacks);
	printf("Kernel PM stacks #:	0x%02X\n",id32.kernel_pmstacks);
	printf("Kernel RM stack size:	0x%04X\n",id32.kernel_rmstacklen);
	printf("Kernel PM stack size:	0x%04X\n",id32.kernel_pmstacklen);
	printf("Kernel max. ext. mem:	0x%08X\n",id32.kernel_maxextmem);
	printf("DOS/32A misc. byte:	0x%02X\n",id32.dos32a_misc);
	printf("DOS/32A 2nd misc. byte:	0x%02X\n",id32.dos32a_misc2);
	printf("DOS/32A low buf. size:	0x%04X\n",id32.dos32a_lowbufsize);
	printf("DOS/32A version #:	0x%04X\n",id32.dos32a_version);
	if(id32.dos32a_version>=0x0700)
	{
		printf("DOS/32A build name:	%s\n",id32.dos32a_buildname);
		printf("DOS/32A build version:	%s\n",id32.dos32a_buildvers);
		printf("DOS/32A copyright note:	%s\n",id32.dos32a_copyright1);
		printf("			%s\n",id32.dos32a_copyright2);
		printf("DOS/32A build date:	%s\n",id32.dos32a_builddate);
		printf("DOS/32A build time:	%s\n",id32.dos32a_buildtime);
	}
}

void CheckIfLocked()
{
	if((id32.dos32a_misc2&0x40)==0x40)
	{
		SetColor(LIGHTWHITE);
		SetBackColor(RED);
		OpenWindow(10,24,5,32);
		SetColor(YELLOW);
		Print_At(11,28,"Configuration is Locked!");
		SetColor(BLUE);
		SetBackColor(WHITE);
		SetBlink(ON);
		Print_At(13,27," Press any key to exit... ");
		SetBlink(OFF);
		GetKey();
		CloseAllWindows();
		VideoReset();
		exit(0);
	}
}

void CheckVersion()
{
	if(id32.dos32a_version==0)
		return;
	if(id32.dos32a_version!=version2)
	{
		SetColor(LIGHTWHITE);
		SetBackColor(RED);
		OpenWindow(10,18,8,44);
		SetColor(LIGHTWHITE);
		Print_At(11,20,"WARNING:");
		SetColor(YELLOW);
		Print_At(11,29,"DOS Extender's version does not");
		Print_At(12,20,"match the version of the Setup Program!!");
		SetColor(BLUE);
		SetBackColor(WHITE);
		SetBlink(ON);
		Print_At(15,27," Press any key to exit... ");
		SetBlink(OFF);
		GetKey();
		CloseWindow();
		DiscardExit();
	}
}

int CheckIfModified()
{
	if(	id32.id!=id32_old.id ||
		id32.kernel_misc!=id32_old.kernel_misc ||
		id32.kernel_pagetables!=id32_old.kernel_pagetables ||
		id32.kernel_phystables!=id32_old.kernel_phystables ||
		id32.kernel_callbacks!=id32_old.kernel_callbacks ||
		id32.kernel_selectors!=id32_old.kernel_selectors ||
		id32.kernel_rmstacks!=id32_old.kernel_rmstacks ||
		id32.kernel_pmstacks!=id32_old.kernel_pmstacks ||
		id32.kernel_rmstacklen!=id32_old.kernel_rmstacklen ||
		id32.kernel_pmstacklen!=id32_old.kernel_pmstacklen ||
		id32.kernel_maxextmem!=id32_old.kernel_maxextmem ||
		id32.dos32a_misc!=id32_old.dos32a_misc ||
		(id32.dos32a_misc2&0x3F)!=(id32_old.dos32a_misc2&0x3F) ||
		id32.dos32a_lowbufsize!=id32_old.dos32a_lowbufsize ||
		id32.dos32a_version!=id32_old.dos32a_version
	)	modified_flag=TRUE;
	else	modified_flag=FALSE;
	return(modified_flag);
}

void ShowModified()
{
	SetColor(LIGHTBLUE);
	SetBackColor(BLUE);
	if(CheckIfModified()==TRUE)	Print_At(24,0,"*");
	else				Print_At(24,0," ");
}


/****************************************************************************/
void AskExit()
{
	if(CheckIfModified()==TRUE)
	{
		SetColor(LIGHTWHITE);
		SetBackColor(RED);
		OpenWindow(10,24,5,32);
		SetColor(YELLOW);
		Print_At(11,29,"Apply Changes on Exit?");
		SetColor(BLUE);
		SetBackColor(WHITE);
		SetBlink(ON);
		Print_At(13,32," <Yes> ");
		Print_At(13,42," <No> ");
		SetBlink(OFF);
		GetKey();
	}
	CloseAllWindows();
	VideoReset();
	if(keycode!=ESC && keychar!='n' && keychar!='N')
		if(WriteHeader(&id32)!=0)
		{
			printf("%s could not write to file\n" ,errstr);
			exit(1);
		}
	CloseFile();
	exit(0);
}

void ApplyExit()
{
	CloseAllWindows();
	VideoReset();
	if(WriteHeader(&id32)!=0)
	{
		printf("%s could not write to file\n", errstr);
		exit(1);
	}
	CloseFile();
	exit(0);
}

void DiscardExit()
{
	CloseAllWindows();
	VideoReset();
	CloseFile();
	exit(0);
}

void RestoreDefaults()
{
	id32.kernel_misc=		0x3F;
	id32.kernel_pagetables=		64;
	id32.kernel_phystables=		2;
	id32.kernel_callbacks=		16;
	id32.kernel_selectors=		256;
	id32.kernel_rmstacks=		8;
	id32.kernel_pmstacks=		8;
	id32.kernel_rmstacklen=		0x20;
	id32.kernel_pmstacklen=		0x20;
	id32.kernel_maxextmem=		0xFFFFFFFF;
	id32.dos32a_misc=		0x3F;
	id32.dos32a_misc2=		(id32.dos32a_misc2 & 0xC0) | 0x09;
	id32.dos32a_lowbufsize=		0x0200;
}


void ValidateConfig()
{
	id32.kernel_misc = (id32.kernel_misc & 0xBF);
	if(id32.kernel_maxextmem>=0x7FFFFFFF) id32.kernel_maxextmem=0xFFFFFFFF;
	id32.dos32a_lowbufsize = (id32.dos32a_lowbufsize & 0x0FFF);
	ShowModified();
}


void ShowMemory()
{
	memtotal=(
		(mem_dos32a)+
		(0x0400)+				// RM INT table
		(0x0800)+				// PM INT table
		(id32.dos32a_lowbufsize*16)+		// DOS buffer
		(id32.kernel_selectors*8)+		// Selectors
		(id32.kernel_callbacks*25)+		// Callbacks
		(id32.kernel_rmstacks*id32.kernel_rmstacklen*16)+
		(id32.kernel_pmstacks*id32.kernel_pmstacklen*16)
		)/1024;

	SetColor(LIGHTWHITE);
	SetBackColor(CYAN);
	Print_At(10,24,"      ");
	if(memtotal<1024)
		Print_At(10,24,"%3dKB",memtotal);
	else
	{
		SetColor(RED);
		Print_At(10,24," >1MB");
		SetColor(LIGHTWHITE);
	}

	if(id32.dos32a_version==0)
		Print_At(9,26,"N/A");
	else
	{
		Print_At(9,25,"%d.%02d",(id32.dos32a_version&0xFF00)>>8,id32.dos32a_version&0x00FF);
		if((id32.dos32a_misc2&0x80)!=0x80)
			Print_At(9,21,"Pro");
	}
}

void CreateConfig()
{
	int n;
	char *name;
	char *ptr;
	char buf[80];

	SetColor(LIGHTWHITE);
	SetBackColor(BLUE);
	OpenWindow(10,10,3,60);
	SetColor(YELLOW);
	Print_At(10,13," Create Predefined Configuration ");
	SetColor(LIGHTWHITE);
	Print_At(11,12,"Enter file name:");
	name=InputString(11,30,37,cfgfilename);
	if(name!=NULL && name[0]!=0)
	{
		strcpy(buf,name);
		ptr=(char *)strchr(buf,'.');
		if(ptr!=NULL) strset(ptr,0);
		strcpy(cfgfilename,buf);
		strcat(buf,".D32");
		n=open(buf,O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, S_IRWXU);
		if(n!=-1)
		{
			write(n,&id32,24);
			close(n);
			SetColor(LIGHTWHITE);
			SetBackColor(BLUE);
			strupr(buf);
			for(n=0; n<13; n++)
			{
				SetPos(24,66+n);
				PrintC(' ');
			}
			Print_At(24,66,"%.13s",buf);
		}
		else DiskError();
	}
	CloseWindow();
}

void RestoreConfig()
{
	int n;
	char *envname;
	char envbuf[256];
	char *name;
	char *ptr;
	char buf[80];

	SetColor(LIGHTWHITE);
	SetBackColor(BLUE);
	OpenWindow(10,10,3,60);
	SetColor(YELLOW);
	Print_At(10,13," Restore Predefined Configuration ");
	SetColor(LIGHTWHITE);
	Print_At(11,12,"Enter file name:");

	name=InputString(11,30,37,cfgfilename);
	if(name!=NULL && name[0]!=0)
	{
		strcpy(buf,name);
		ptr=(char *)strchr(buf,'.');
		if(ptr!=NULL) strset(ptr,0);
		strcpy(cfgfilename,buf);
		strcat(buf,".d32");
		n=open(buf,O_RDONLY | O_BINARY);
		if(n==-1)
		{
			envname=getenv("DOS32A");
			if(envname!=NULL)
			{
				ptr=strchr(envname,' ');
				if(ptr==NULL) ptr=strchr(envname,0);
				memset(envbuf,0,256);
				strncpy(envbuf,envname,(dword)ptr-(dword)envname);
				strcat(envbuf,"\\D32\\"); strcat(envbuf,buf);
				n=open(envbuf,O_RDWR | O_BINARY);
			}
		}
		if(n!=-1)
		{
			read(n,&id32,24);
			close(n);
			SetColor(LIGHTWHITE);
			SetBackColor(BLUE);
			strupr(buf);
			for(n=0; n<13; n++)
			{
				SetPos(24,66+n);
				PrintC(' ');
			}
			Print_At(24,66,"%.13s",buf);
		}
		else DiskError();
	}
	ValidateConfig();
	CloseWindow();
	if(mainmenu_sel==1) ShowBannerStatus();

}

void ClearConfigName()
{
	SetColor(LIGHTWHITE);
	SetBackColor(BLUE);
	Print_At(24,66,"            ");
}


void ShowBannerStatus()
{
	if((id32.dos32a_misc2&0x80)==0)
	{
		SetColor(LIGHTWHITE);
		SetBackColor(BLUE);
		Print_At(12,10,"10)  Show Copyright Banner at Startup ..................");
	}
	else
	{
		SetColor(WHITE);
		SetBackColor(BLUE);
		Print_At(12,10,"10)  Show Copyright Banner at Startup ..................");
		SetColor(LIGHTWHITE);
	}
}

void DiskError()
{
	SetColor(LIGHTWHITE);
	SetBackColor(RED);
	OpenWindow(10,24,5,32);
	SetColor(YELLOW);
	Print_At(11,28,"Error Reading/Writing!");
	SetColor(BLUE);
	SetBackColor(WHITE);
	SetBlink(ON);
	Print_At(13,26," Press a key to continue... ");
	SetBlink(OFF);
	GetKey();
	CloseWindow();
	keychar=keycode=0;
}






/****************************************************************************/
void ShowSysInfo()
{
	unsigned long n;
	char *systab[6]={"  Clean","    XMS","   VCPI","   DPMI","WinDPMI","Unknown" };
	char *cputab[6]={ "80386","80486","80586","80686","80786","80886" };
	char *filetab[5]={ " N/A", "\"LE\"", "\"LX\"", "\"LC\"", "\"PE\"" };

	SetColor(LIGHTWHITE);
	SetBackColor(CYAN);
	OpenWindow(2,1,10,30);
	SetColor(LIGHTGREEN);
	Print_At(2,4," General Information ");

	SetColor(LIGHTBLUE);
	Print_At( 3,3,"CPU Type:");
	Print_At( 4,3,"System Type:");
	Print_At( 5,3,"Free DOS Memory:");
	Print_At( 6,3,"Free High Memory:");
	Print_At( 8,3,"Bound File Type:");
	if(extn_type==2) Print_At( 9,3,"STUB/32C Version:");
	else		 Print_At( 9,3,"DOS/32A Version:");
	Print_At(10,3,"DOS Memory Required:");
	SetColor(LIGHTWHITE);

	n=get_cpu_type();
	Print_At(3,24,cputab[n-3]);

	systype=n=get_sys_type();
	Print_At(4,22,systab[n]);

	n=get_lomem_size()+get_dosmem_used(mem_dos32a);
	Print_At(5,21,"%6dKB",(n/1024)+1);

	n=get_himem_size();
	Print_At(6,21,"%6dKB",n/1024);

	Print_At(8,25,filetab[file_type]);

}

void ShowEnvInfo()
{
	char *str;
	SetColor(LIGHTWHITE);
	SetBackColor(WHITE);
	OpenWindow(14,1,3,76);
	SetColor(LIGHTGREEN);
	Print_At(14,4," DOS/32A Environment String ");
	SetColor(LIGHTWHITE);
	str=getenv("DOS32A");
	if(str==NULL) Print_At(15,3,"(Not Defined)");
	else Print_At(15,3,"DOS32A=%.74s",str);
}

void ShowHelp(int menu, int header)
{
	char c;
	SetColor(LIGHTWHITE);
	SetBackColor(GREEN);
	DrawWindow(18,1,5,76);
	SetColor(LIGHTGREEN);
	Print_At(18,4," Quick Help ");
	SetColor(LIGHTWHITE);

	SetPos(19,3);
	if(menu==0) PrintHelp(mainhlp[header]);
	if(menu==1) PrintHelp(kernelhlp[header]);
	if(menu==2) PrintHelp(extenderhlp[header]);
}



void ShowMemReq()
{
	int n,m;

	SetColor(LIGHTWHITE);
	SetBackColor(BLUE);
	OpenWindow(6,14,8,52);
	SetColor(YELLOW);
	Print_At(6,17," DOS/32A Memory Requirements ");
	SetColor(LIGHTWHITE);


	Print_At( 8,16,"Clean System:");
	Print_At( 9,16,"XMS System:  ");
	Print_At(10,16,"VCPI System: ");
	Print_At(11,16,"DPMI System: ");

/*** Clean ***/
/*** XMS ***/
	n=	(
		(mem_dos32a)+
		(0x0400)+				// RM INT table
		(0x0800)+				// PM INT table
		(id32.dos32a_lowbufsize*16)+		// DOS buffer
		(id32.kernel_selectors*8)+		// Selectors
		(id32.kernel_callbacks*25)+		// Callbacks
		(id32.kernel_rmstacks*id32.kernel_rmstacklen*16)+
		(id32.kernel_pmstacks*id32.kernel_pmstacklen*16)
		)/1024;
	Print_At( 8,30,"%3dKB",n);
	Print_At( 9,30,"%3dKB",n);

/*** VCPI ***/
	m=	get_total_mem()/1024;
	if(id32.kernel_pagetables<m) m=id32.kernel_pagetables;
	if((id32.kernel_misc&0x04)!=0) m=m-1;
	if(m<1) m=1;
	n=	(
		(mem_dos32a)+
		(0x0400)+				// RM INT table
		(0x0800)+				// PM INT table
		(id32.dos32a_lowbufsize*16)+		// DOS buffer
		(id32.kernel_selectors*8)+		// Selectors
		(id32.kernel_callbacks*25)+		// Callbacks
		(id32.kernel_rmstacks*id32.kernel_rmstacklen*16)+
		(id32.kernel_pmstacks*id32.kernel_pmstacklen*16)+
		(m*4096)+				// Page Tables
		(id32.kernel_phystables*4096)+		// Phys Tables
		(4096)					// Page Directory
		)/1024;
	Print_At(10,30,"%3dKB",n);

	if(systype==2)	Print_At(10,40,"(approximately)");
	else		Print_At(10,40,"(advisory info only)");

/*** DPMI ***/
	n=	(
		(mem_dos32a)+
		(0x0400)+				// RM INT table
		(id32.dos32a_lowbufsize*16)+		// DOS buffer
		(get_dpmi_bufsize())			// DPMI buffer
		-mem_kernel
		)/1024;
	Print_At(11,30,"%3dKB",n);
	if(systype>=3)	Print_At(11,40,"(approximately)");
	else		Print_At(11,40,"(advisory info only)");


	GetKey();
	CloseWindow();
	keychar=keycode=0;

}

#include "menus.c"
#include "iface.c"

