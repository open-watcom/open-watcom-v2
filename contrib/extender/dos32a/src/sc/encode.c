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
#include <stdlib.h>
#include <typedefs.h>
//#include <debug.h>

#define N			4*1024	// size of the buffer
#define F			18
#define THRESHOLD	2
#define NIL			N


	uchar	text_buf[N+F-1];

	int		match_position;
	int		match_length;
	int		node[N+1];
	int		lnode[N+1];
	int		rnode[N+257];

	int		bytecount = 0;
	int		printcount = 0;
	ulong	textsize = 0;
	ulong	codesize = 0;

	char	cc = 0;
	char	xc = 0;
	char	*srcaddr = NULL;
	char	*destaddr = NULL;


int getbyte()
{
	if(bytecount-- > 0)
		return(*srcaddr++);
	else
		return(EOF);
}

void putbyte(char c)
{
	*destaddr++=(c^xc);
	xc=c;
}

void InitTree(void)
{
	int  i;

	for(i=N+1; i<=N+256; i++)
		rnode[i]=NIL;
	for(i=0; i<N; i++)
		node[i]=NIL;
}

void InsertNode(int r)
{
	int i,p,cmp;
	char *key;

	cmp=1;
	key=&text_buf[r];
	p=N+1+key[0];
	rnode[r]=lnode[r]=NIL;
	match_length=0;

	while(TRUE)
	{
		if(cmp>=0)
		{
			if(rnode[p]!=NIL)
				p=rnode[p];
			else
			{
				rnode[p]=r;
				node[r]=p;
				return;
			}
		}
		else
		{
			if(lnode[p]!=NIL)
				p=lnode[p];
			else
			{
				lnode[p]=r;
				node[r]=p;
				return;
			}
		}
		for(i=1; i<F; i++)
			if((cmp=key[i]-text_buf[p+i])!=0)
				break;
		if(i>match_length)
		{
			match_position=p;
			if((match_length=i)>=F)
				break;
		}
	}
	node[r]=node[p];
	lnode[r]=lnode[p];
	rnode[r]=rnode[p];
	node[lnode[p]]=r;
	node[rnode[p]]=r;
	if(rnode[node[p]]==p)
		rnode[node[p]]=r;
	else
		lnode[node[p]]=r;
	node[p]=NIL;
}

void DeleteNode(int p)
{
	int q;

	if(node[p]==NIL)
		return;
	if(rnode[p]==NIL)
		q=lnode[p];
	else if(lnode[p]==NIL)
		q=rnode[p];
	else
	{
		q=lnode[p];
		if(rnode[q]!=NIL)
		{
			do
			{
				q=rnode[q];
			}
			while(rnode[q]!=NIL);

			rnode[node[q]]=lnode[q];
			node[lnode[q]]=node[q];
			lnode[q]=lnode[p];
			node[lnode[p]]=q;
		}
		rnode[q]=rnode[p];
		node[rnode[p]]=q;
	}
	node[q]=node[p];
	if(rnode[node[p]]==p)
		rnode[node[p]]=q;
	else
		lnode[node[p]]=q;
	node[p]=NIL;
}

void Encode(void)
{
	int i, c, len, r, s, last_match_length, code_buf_ptr;
	char  code_buf[17], mask;

	bytecount=textsize;
	InitTree();
	code_buf[0]=0;

	code_buf_ptr=mask=1;
	s=0;
	r=N-F;

	for(i=s; i<r; i++)
		text_buf[i]=cc;
	for(len=0; len<F&&(c=getbyte())!=EOF; len++)
		text_buf[r+len]=c;
	if(textsize==0 || len==0)
		return;

	for(i=1; i<=F; i++)
		InsertNode(r-i);
	InsertNode(r);
	do
	{
		if(match_length>len)
			match_length=len;
		if(match_length<=THRESHOLD)
		{
			match_length=1;
			code_buf[0]|=mask;
			code_buf[code_buf_ptr++]=text_buf[r];
		}
		else
		{
			code_buf[code_buf_ptr++]=(uchar)match_position;
			code_buf[code_buf_ptr++]=(uchar)(((match_position>>4)&0xf0)|(match_length-(THRESHOLD+1)));
		}
		if((mask<<=1)==0)
		{
			for(i=0; i<code_buf_ptr; i++)
				putbyte(code_buf[i]);
			codesize+=code_buf_ptr;
			code_buf[0]=0;
			code_buf_ptr=mask=1;
		}
		last_match_length = match_length;
		for(i=0; i<last_match_length&&(c=getbyte())!=EOF; i++)
		{
			DeleteNode(s);
			text_buf[s]=c;
			if(s<F-1)
				text_buf[s+N]=c;
			s=(s+1)&(N-1);
			r=(r+1)&(N-1);
			InsertNode(r);
		}
		if(codesize>=printcount)
		{
			printcount+=256;
			if(bytecount<=0)
				ShowCount(99);
			else
				ShowCount((int)(((float)(textsize-bytecount)/(float)textsize)*100));
		}
		while(i++<last_match_length)
		{
			DeleteNode(s);
			s=(s+1)&(N-1);
			r=(r+1)&(N-1);
			if(--len)
				InsertNode(r);
		}
	}
	while(len>0);

	if(code_buf_ptr>1)
	{
		for(i=0; i<code_buf_ptr; i++)
			putbyte(code_buf[i]);
		codesize+=code_buf_ptr;
	}
}



/****************************************************************************/
int CompressData(char *src, char *dest, int size)
{

	srcaddr=src;
	destaddr=dest;
	textsize=size;
	codesize=0;
	printcount=0;
	cc=0;
	xc=0;
	Encode();
	return(codesize);
}


