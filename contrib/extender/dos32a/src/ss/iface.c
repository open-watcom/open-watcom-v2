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


void CloseAllWindows()
{
	int n;
	for(n=0; n<16; n++) CloseWindow();
}

void Print_At(int y, int x, char *format, ...)
{
	va_list arglist;
	char buffer[256];
	SetPos(y,x);
	va_start(arglist,format);
	vsprintf(buffer,format,arglist);
	Prints(buffer);
	va_end(arglist);
}

void Print(char *format, ...)
{
	va_list arglist;
	char buffer[256];
	va_start(arglist,format);
	vsprintf(buffer,format,arglist);
	Prints(buffer);
	va_end(arglist);
}


unsigned long Input(int y, int x, int l, unsigned long old)
{
	int n;
	char c;
	char buf[80];

	Print_At(y,x-1,"<");
	Print_At(y,x+l,">");
	for(n=0; n<l; n++) Print_At(y,x+n,".");

	for(n=0; n<l; n++)
	{
l0:		SetBlink(1); Print_At(y,x+n,"Û"); SetBlink(0);

		do
		{	GetKey(); c=toupper(keychar);
			if(c=='X') c='x';
			if(c==0x1B) goto l1;
			if(c==0x0D) goto l1;
			if(c==0x08) break;
		} while(c<0x20);

		if(c==0x08)
		{
			if(n>0)
			{
				Print_At(y,x+n,".");
				n--;
				goto l0;
			} else goto l0;
		}
		buf[n]=c;
		Print_At(y,x+n,"%c",c);
	}

l1:	buf[n]=0;

	Print_At(y,x-1," ");
	Print_At(y,x+l," ");
	for(n=0; n<l; n++) Print_At(y,x+n," ");

	keychar=keycode=0;
	if(c==0x1B) return(old);
	if(strlen(buf)==0 && c==0x0D) return(old);
	if(hexmode==0) return((unsigned long)strtol(buf,NULL,10));
	else return((unsigned long)strtol(buf,NULL,16));
}

char *InputString(int y, int x, int l, char *tmp)
{
	int n,m, xx;
	char c;
	static char buf[80];
	char *defbuf[] = {
		"DEFAULT.D32",
		"DOS4GW.D32",
		"FAILSAFE.D32",
		"MAXIMUM.D32",
		"MINIMUM.D32",
		"PMODEW.D32",
		"STANDARD.D32",
		"VERBOSE.D32",
		"CUSTOM.D32",
		""
	};

	m=-1; xx=x;
	if(tmp!=NULL) strcpy(buf,tmp);
l8:	Print_At(y,x-1,"<");
	Print_At(y,x+l,">");
	for(n=0; n<l; n++) Print_At(y,x+n,".");
	for(n=0; buf[n]!=NULL; n++) { SetPos(y,x+n); PrintC(buf[n]); }

	for(; n<l; n++)
	{
l0:		SetBlink(1); Print_At(y,x+n,"Û"); SetBlink(0);

		do
		{	GetKey();
			c=toupper(keychar);
			if(c==0x1B) goto l1;
			if(c==0x0D) goto l1;
			if(c==0x08) break;
			if(keycode==UP)   { m++; if(m>8) m=0; x=xx; strcpy(buf,defbuf[m]); goto l8; }
			if(keycode==DOWN) { m--; if(m<0) m=8; x=xx; strcpy(buf,defbuf[m]); goto l8; }
		} while(c<0x20);

		if(c==0x08)
		{
		  if(n>0)
		  {
			Print_At(y,x+n,".");
			n--;
			goto l0;
		  } else goto l0;
		}
		buf[n]=c;
		Print_At(y,x+n,"%c",c);
	}

l1:	buf[n]=0;

	Print_At(y,x-1," ");
	Print_At(y,x+l," ");
	for(n=0; n<l; n++) Print_At(y,x+n," ");

	keychar=keycode=0;
	if(c==0x1B) return(NULL);
	else if(strlen(buf)==0) return(NULL);
	else return(buf);
}
