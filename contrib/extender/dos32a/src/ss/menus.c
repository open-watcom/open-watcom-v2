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


void ShowMainMenu()
{
	SetColor(LIGHTWHITE);
	SetBackColor(BLUE);
	OpenWindow(2,35,10,42);
	SetColor(YELLOW);
	Print_At(2,38," Main Menu ");
	SetColor(LIGHTWHITE);

	Print_At( 3,37,"F1 -  Configure DOS/32A DPMI Kernel");
	Print_At( 4,37,"F2 -  Configure DOS/32A DOS Extender");
	Print_At( 5,37,"F3 -  Create Predefined Configuration");
	Print_At( 6,37,"F4 -  Restore Predefined Configuration");
	Print_At( 8,37,"F5 -  Restore Old Values");
	Print_At( 9,37,"F9 -  Discard Changes and Exit");
	Print_At(10,37,"F10 - Apply Changes and Exit");
}


/*--------------------------------------------------------------------------*/
void SelectMainMenu()
{
	do {
		ShowModified();
		ShowHelp(0,mainmenu_sel);
		ShowCursor(	mainmenu_ypos[mainmenu_sel],
				mainmenu_xpos[mainmenu_sel],
				mainmenu_xlen[mainmenu_sel]);
		GetKey();
		ShowCursor(	mainmenu_ypos[mainmenu_sel],
				mainmenu_xpos[mainmenu_sel],
				mainmenu_xlen[mainmenu_sel]);

		if(keycode==UP) mainmenu_sel--;
		if(keycode==DOWN) mainmenu_sel++;
		if(keycode==HOME) mainmenu_sel=0;
		if(keycode==END) mainmenu_sel=6;
		if(keycode==PAGEUP) mainmenu_sel=0;
		if(keycode==PAGEDOWN) mainmenu_sel=6;

		if(keycode==F1) { mainmenu_sel=0; break; }
		if(keycode==F2) { mainmenu_sel=1; break; }
		if(keycode==F3) { mainmenu_sel=2; break; }
		if(keycode==F4) { mainmenu_sel=3; break; }
		if(keycode==F5) { mainmenu_sel=4; break; }
		if(keycode==F6) { RestoreDefaults(); ClearConfigName(); ShowMemory(); }
		if(keycode==F7) { ValidateConfig(); ShowMemory(); }
		if(keycode==F8) { ShowMemReq(); ShowMemory(); }
		if(keycode==F9) { mainmenu_sel=5; break; }
		if(keycode==F10) { mainmenu_sel=6; break; }

		if(mainmenu_sel<0) mainmenu_sel=0;
		if(mainmenu_sel>6) mainmenu_sel=6;
	} while(keycode!=ESC && keycode!=ALT_X && keycode!=ENTER);
	if(keycode==ESC || keycode==ALT_X) AskExit();
}





/****************************************************************************/
void ShowKernelMenu()
{
	SetColor(LIGHTWHITE);
	SetBackColor(BLUE);
	OpenWindow(2,8,15,64);
	SetColor(YELLOW);
	Print_At(2,11," DPMI Kernel Configuration ");
	SetColor(LIGHTWHITE);

	SetColor(WHITE);
	Print_At( 3,10,"1)  VCPI/DPMI Detection Order ....................");
	Print_At( 4,10,"2)  XMS/VCPI Detection Order .....................");
	SetColor(LIGHTWHITE);
	Print_At( 5,10,"3)  VCPI SmartPage Allocation Mode .....................");
	Print_At( 6,10,"4)  VCPI+XMS Allocation Scheme .........................");
	SetColor(WHITE);
	Print_At( 7,10,"5)  Trap and Report Emulated IRQs ......................");
	Print_At( 8,10,"6)  Extended Memory Blocks Checking ....................");
	SetColor(LIGHTWHITE);
	Print_At( 9,10,"7)  Ignore DOS/4G API Extension Calls ..................");
	Print_At(11,10,"Selectors:");
	Print_At(12,10,"Callbacks:");
	Print_At(13,10,"RMode Stacks:");
	Print_At(14,10,"PMode Stacks:");
	Print_At(11,40,"VCPI PageTables:");
	Print_At(12,40,"VCPI PhysTables:");
	Print_At(13,40,"RMode Stack Length:");
	Print_At(14,40,"PMode Stack Length:");
	Print_At(15,20,"Extended Memory:             (Bytes)");

	do
	{
		SelectKernelMenu();
		if(keycode!=ESC && keycode!=F1 && keycode!=F2)
			switch(kernelmenu_sel)
			{
				case 0:		id32.kernel_misc^=0x01; break;
				case 1:		id32.kernel_misc^=0x02; break;
				case 2:		id32.kernel_misc^=0x04; break;
				case 3:		id32.kernel_misc^=0x08; break;
				case 4:		id32.kernel_misc^=0x10; break;
				case 5:		id32.kernel_misc^=0x20; break;
				case 6:		id32.kernel_misc^=0x80; break;
				case 7:		id32.kernel_selectors=Input(11,29,6,id32.kernel_selectors); break;
				case 8:		id32.kernel_callbacks=Input(12,29,6,id32.kernel_callbacks); break;
				case 9:		id32.kernel_rmstacks=Input(13,29,6,id32.kernel_rmstacks); break;
				case 10:	id32.kernel_pmstacks=Input(14,29,6,id32.kernel_pmstacks); break;
				case 11:	id32.kernel_pagetables=Input(11,63,6,id32.kernel_pagetables); break;
				case 12:	id32.kernel_phystables=Input(12,63,6,id32.kernel_phystables); break;
				case 13:	id32.kernel_rmstacklen=Input(13,63,6,id32.kernel_rmstacklen); break;
				case 14:	id32.kernel_pmstacklen=Input(14,63,6,id32.kernel_pmstacklen); break;
				case 15:
					if(hexmode==0) id32.kernel_maxextmem=Input(15,38,10,id32.kernel_maxextmem);
					else id32.kernel_maxextmem=Input(15,40,8,id32.kernel_maxextmem);
					break;
			}
	}
	while(keycode!=ESC && keycode!=F1 && keycode!=F2);
	CloseWindow();
}


/*--------------------------------------------------------------------------*/
void ShowKernelConfig()
{
	SetColor(LIGHTWHITE);
	SetBackColor(BLUE);

//	if(id32.kernel_misc&0x01) Print_At(3,59," VCPI/DPMI");else Print_At(3,59," DPMI/VCPI");

//	if(id32.kernel_misc&0x02) Print_At(4,60," XMS/VCPI");else Print_At(4,60," VCPI/XMS");

	SetColor(WHITE);
	Print_At(3,60," reserved");
	Print_At(4,60," reserved");
	SetColor(LIGHTWHITE);


	if(id32.kernel_pagetables==0 && !(id32.kernel_misc&0x04)) { SetColor(LIGHTRED); SetBlink(1); }
	if(id32.kernel_misc&0x04) Print_At(5,65,". ON");else Print_At(5,65," OFF");
	SetColor(LIGHTWHITE); SetBlink(0);

	if(id32.kernel_misc&0x08) Print_At(6,65,". ON");else Print_At(6,65," OFF");

	SetColor(WHITE);
	Print_At(7,60," reserved");
	Print_At(8,60," reserved");
	SetColor(LIGHTWHITE);

	if(id32.kernel_misc&0x80) Print_At(9,65,". ON");else Print_At(9,65," OFF");

	if(hexmode==0)
	{
		if(id32.kernel_selectors<16) SetColor(YELLOW);
		if(id32.kernel_selectors==0) { SetColor(LIGHTRED); SetBlink(1); }
		if(id32.kernel_selectors>8176) { SetColor(LIGHTRED); SetBlink(1); }
		Print_At(11,27,"%8d",id32.kernel_selectors);
		SetColor(LIGHTWHITE); SetBlink(0);

		if(id32.kernel_callbacks<8) { SetColor(LIGHTRED); SetBlink(1); }
		Print_At(12,27,"%8d",id32.kernel_callbacks);
		SetColor(LIGHTWHITE); SetBlink(0);

		if(id32.kernel_rmstacks<2) SetColor(YELLOW);
		if(id32.kernel_rmstacks==0) { SetColor(LIGHTRED); SetBlink(1); }
		Print_At(13,27,"%8d",id32.kernel_rmstacks);
		SetColor(LIGHTWHITE); SetBlink(0);

		if(id32.kernel_pmstacks<2) SetColor(YELLOW);
		if(id32.kernel_pmstacks==0) { SetColor(LIGHTRED); SetBlink(1); }
		Print_At(14,27,"%8d",id32.kernel_pmstacks);
		SetColor(LIGHTWHITE); SetBlink(0);

		if(id32.kernel_maxextmem>0x7FFFFFFF) Print_At(15,40,"     All");
		else if(id32.kernel_maxextmem==0) Print_At(15,40,"    None");
		else Print_At(15,38,"%10d",id32.kernel_maxextmem);

		if(id32.kernel_pagetables>64) SetColor(YELLOW);
		if(id32.kernel_pagetables==0 && !(id32.kernel_misc&0x04)) { SetColor(LIGHTRED); SetBlink(1); }
		Print_At(11,61,"%8d",id32.kernel_pagetables);
		SetColor(LIGHTWHITE); SetBlink(0);

		Print_At(12,61,"%8d",id32.kernel_phystables);

		if(id32.kernel_rmstacklen<0x10) SetColor(YELLOW);
		if(id32.kernel_rmstacklen==0) { SetColor(LIGHTRED); SetBlink(1); }
		Print_At(13,61,"%8d",id32.kernel_rmstacklen);
		SetColor(LIGHTWHITE); SetBlink(0);

		if(id32.kernel_pmstacklen<0x10) SetColor(YELLOW);
		if(id32.kernel_pmstacklen==0) { SetColor(LIGHTRED); SetBlink(1); }
		Print_At(14,61,"%8d",id32.kernel_pmstacklen);
		SetColor(LIGHTWHITE); SetBlink(0);

	}
	else
	{

		if(id32.kernel_selectors<16) SetColor(YELLOW);
		if(id32.kernel_selectors==0) { SetColor(LIGHTRED); SetBlink(1); }
		if(id32.kernel_selectors>8176) { SetColor(LIGHTRED); SetBlink(1); }
		Print_At(11,27,"%7Xh",id32.kernel_selectors);
		SetColor(LIGHTWHITE); SetBlink(0);

		if(id32.kernel_callbacks<8) { SetColor(LIGHTRED); SetBlink(1); }
		Print_At(12,27,"%7Xh",id32.kernel_callbacks);
		SetColor(LIGHTWHITE); SetBlink(0);

		if(id32.kernel_rmstacks<2) SetColor(YELLOW);
		if(id32.kernel_rmstacks==0) { SetColor(LIGHTRED); SetBlink(1); }
		Print_At(13,27,"%7Xh",id32.kernel_rmstacks);
		SetColor(LIGHTWHITE); SetBlink(0);

		if(id32.kernel_pmstacks<2) SetColor(YELLOW);
		if(id32.kernel_pmstacks==0) { SetColor(LIGHTRED); SetBlink(1); }
		Print_At(14,27,"%7Xh",id32.kernel_pmstacks);
		SetColor(LIGHTWHITE); SetBlink(0);

		if(id32.kernel_maxextmem>0x7FFFFFFF) Print_At(15,40,"     All");
		else if(id32.kernel_maxextmem==0) Print_At(15,40,"    None");
		else Print_At(15,38," %8Xh",id32.kernel_maxextmem);

		if(id32.kernel_pagetables>64) SetColor(YELLOW);
		if(id32.kernel_pagetables==0 && !(id32.kernel_misc&0x04)) { SetColor(LIGHTRED); SetBlink(1); }
		Print_At(11,61,"%7Xh",id32.kernel_pagetables);
		SetColor(LIGHTWHITE); SetBlink(0);

		Print_At(12,61,"%7Xh",id32.kernel_phystables);

		if(id32.kernel_rmstacklen<0x10) SetColor(YELLOW);
		if(id32.kernel_rmstacklen==0) { SetColor(LIGHTRED); SetBlink(1); }
		Print_At(13,61,"%7Xh",id32.kernel_rmstacklen);
		SetColor(LIGHTWHITE); SetBlink(0);

		if(id32.kernel_pmstacklen<0x10) SetColor(YELLOW);
		if(id32.kernel_pmstacklen==0) { SetColor(LIGHTRED); SetBlink(1); }
		Print_At(14,61,"%7Xh",id32.kernel_pmstacklen);
		SetColor(LIGHTWHITE); SetBlink(0);
	}
	SetColor(LIGHTBLUE);
	if((id32.kernel_misc&0x01)!=(id32_old.kernel_misc&0x01)) Print_At(3,69,"*");else Print_At(3,69," ");
	if((id32.kernel_misc&0x02)!=(id32_old.kernel_misc&0x02)) Print_At(4,69,"*");else Print_At(4,69," ");
	if((id32.kernel_misc&0x04)!=(id32_old.kernel_misc&0x04)) Print_At(5,69,"*");else Print_At(5,69," ");
	if((id32.kernel_misc&0x08)!=(id32_old.kernel_misc&0x08)) Print_At(6,69,"*");else Print_At(6,69," ");
	if((id32.kernel_misc&0x10)!=(id32_old.kernel_misc&0x10)) Print_At(7,69,"*");else Print_At(7,69," ");
	if((id32.kernel_misc&0x20)!=(id32_old.kernel_misc&0x20)) Print_At(8,69,"*");else Print_At(8,69," ");
	if((id32.kernel_misc&0x80)!=(id32_old.kernel_misc&0x80)) Print_At(9,69,"*");else Print_At(9,69," ");
	if((id32.kernel_selectors)!=(id32_old.kernel_selectors)) Print_At(11,35,"*");else Print_At(11,35," ");
	if((id32.kernel_callbacks)!=(id32_old.kernel_callbacks)) Print_At(12,35,"*");else Print_At(12,35," ");
	if((id32.kernel_rmstacks)!=(id32_old.kernel_rmstacks)) Print_At(13,35,"*");else Print_At(13,35," ");
	if((id32.kernel_pmstacks)!=(id32_old.kernel_pmstacks)) Print_At(14,35,"*");else Print_At(14,35," ");
	if((id32.kernel_pagetables)!=(id32_old.kernel_pagetables)) Print_At(11,69,"*");else Print_At(11,69," ");
	if((id32.kernel_phystables)!=(id32_old.kernel_phystables)) Print_At(12,69,"*");else Print_At(12,69," ");
	if((id32.kernel_rmstacklen)!=(id32_old.kernel_rmstacklen)) Print_At(13,69,"*");else Print_At(13,69," ");
	if((id32.kernel_pmstacklen)!=(id32_old.kernel_pmstacklen)) Print_At(14,69,"*");else Print_At(14,69," ");
	if((id32.kernel_maxextmem)!=(id32_old.kernel_maxextmem)) Print_At(15,48,"*");else Print_At(15,48," ");
	SetColor(LIGHTWHITE);
}


/*--------------------------------------------------------------------------*/
void SelectKernelMenu()
{
	do {
		ShowModified();
		ShowHelp(1,kernelmenu_sel);
		ShowKernelConfig();
		ShowCursor(	kernelmenu_ypos[kernelmenu_sel],
				kernelmenu_xpos[kernelmenu_sel],
				kernelmenu_xlen[kernelmenu_sel]);
		GetKey();
		ShowCursor(	kernelmenu_ypos[kernelmenu_sel],
				kernelmenu_xpos[kernelmenu_sel],
				kernelmenu_xlen[kernelmenu_sel]);
//		CloseWindow();

		if(keychar>'0' && keychar<'8') kernelmenu_sel=keychar-'1';
		if(keycode==F3) CreateConfig();
		if(keycode==F4) RestoreConfig();
		if(keycode==F5)	{ id32=id32_old; ClearConfigName(); }
		if(keycode==F6) { RestoreDefaults(); ClearConfigName(); }
		if(keycode==F8) ShowMemReq();
		if(keycode==UP) kernelmenu_sel--;
		if(keycode==DOWN) kernelmenu_sel++;
		if(keycode==HOME) kernelmenu_sel=0;
		if(keycode==END) kernelmenu_sel=16;
		if(keycode==PAGEUP)
			if(kernelmenu_sel>11) kernelmenu_sel=11;
			else if(kernelmenu_sel>7) kernelmenu_sel=7;
			else if(kernelmenu_sel>0) kernelmenu_sel=0;
		if(keycode==PAGEDOWN)
			if(kernelmenu_sel<7) kernelmenu_sel=7;
			else if(kernelmenu_sel<11) kernelmenu_sel=11;
			else if(kernelmenu_sel<15) kernelmenu_sel=15;

		if(keycode==LEFT) if(kernelmenu_sel>10 && kernelmenu_sel<15) kernelmenu_sel-=4;
		if(keycode==RIGHT) if(kernelmenu_sel>6 && kernelmenu_sel<11) kernelmenu_sel+=4;

		if(keycode==TAB) hexmode^=1;
		if(kernelmenu_sel<0) kernelmenu_sel=0;
		if(kernelmenu_sel>15) kernelmenu_sel=15;
	} while(keycode!=ESC && keycode!=F1 && keycode!=F2 && keycode!=ENTER && keycode!=SPACE);
}





/****************************************************************************/
void ShowExtenderMenu()
{
	int n;
	SetColor(LIGHTWHITE);
	SetBackColor(BLUE);
	OpenWindow(2,8,15,64);
	SetColor(YELLOW);
	Print_At(2,11," DOS Extender Configuration ");
	SetColor(LIGHTWHITE);

	Print_At( 3,10," 1)  Report Warnings ...................................");
	Print_At( 4,10," 2)  Sound Alert on Run-Time Errors ....................");
	Print_At( 5,10," 3)  Restore Real Mode INTs on Exit ....................");
	Print_At( 6,10," 4)  Report Modified Real Mode INTs ....................");
	Print_At( 7,10," 5)  Object Loading Scheme .............................");
	Print_At( 8,10," 6)  Object Alignment when Loaded High ...............");
	Print_At( 9,10," 7)  Clear Screen on Run-Time Errors ...................");
	Print_At(10,10," 8)  Start Full-Screen under Windows ...................");
	Print_At(11,10," 9)  Install Null-Pointer Protection ...................");

	ShowBannerStatus();

	Print_At(13,10,"11)  Configure Using Environment .......................");
	Print_At(14,10,"12)  Start Extender in Verbose Mode ....................");
	Print_At(15,19,"DOS Transfer Buffer Size:         (Bytes)");

	do {
		SelectExtenderMenu();
		if(keycode!=ESC && keycode!=F1 && keycode!=F2) switch(extendermenu_sel) {
			case 0:		id32.dos32a_misc^=0x01; break;
			case 1:		id32.dos32a_misc^=0x02; break;
			case 2:		id32.dos32a_misc^=0x04; break;
			case 3:		id32.dos32a_misc^=0x08; break;
			case 4:		{ n=(id32.dos32a_misc&0x30)>>4;	n=(n+1)&0x03;
					id32.dos32a_misc=((id32.dos32a_misc&0xCF)|(n<<4)); } break;
			case 5:		id32.dos32a_misc2^=0x04; break;
			case 6:		id32.dos32a_misc^=0x40; break;
			case 7:		id32.dos32a_misc2^=0x02; break;
			case 8:		id32.dos32a_misc^=0x80; break;
			case 9:		if((id32.dos32a_misc2&0x80)==0) id32.dos32a_misc2^=0x08; break;
			case 10:	id32.dos32a_misc2^=0x01; break;
			case 11:	id32.dos32a_misc2^=0x10; break;
			case 12:	id32.dos32a_lowbufsize=
					Input(15,46,6,id32.dos32a_lowbufsize<<4)>>4;
					if(id32.dos32a_lowbufsize>0x0FFF) id32.dos32a_lowbufsize=0x0FFF;
					break;
		}
	} while(keycode!=ESC && keycode!=F1 && keycode!=F2);
	CloseWindow();
}



/*--------------------------------------------------------------------------*/
void ShowExtenderConfig()
{
	int n;
	SetColor(LIGHTWHITE);
	SetBackColor(BLUE);
	if(id32.dos32a_misc&0x01) Print_At(3,65,". ON");else Print_At(3,65," OFF");
	if(id32.dos32a_misc&0x02) Print_At(4,65,". ON");else Print_At(4,65," OFF");
	if(id32.dos32a_misc&0x04) Print_At(5,65,". ON");else Print_At(5,65," OFF");
	if(id32.dos32a_misc&0x08) Print_At(6,65,". ON");else Print_At(6,65," OFF");
	Print_At(7,65,". #%d",((id32.dos32a_misc&0x30)>>4)+1);
	if(id32.dos32a_misc2&0x04) Print_At(8,64," PAGE");else Print_At(8,64," PARA");
	if(id32.dos32a_misc&0x40) Print_At(9,65,". ON");else Print_At(9,65," OFF");
	if(id32.dos32a_misc2&0x02) Print_At(10,65,". ON");else Print_At(10,65," OFF");
	if(id32.dos32a_misc&0x80) Print_At(11,65,". ON");else Print_At(11,65," OFF");
	if((id32.dos32a_misc2&0x80)==0)
	{
		if(id32.dos32a_misc2&0x08) Print_At(12,65,". ON");else Print_At(12,65," OFF");
	}
	else
	{
		SetColor(WHITE);
		if(id32.dos32a_misc2&0x08) Print_At(12,65,". ON");else Print_At(12,65," OFF");
		SetColor(LIGHTWHITE);
	}
	if(id32.dos32a_misc2&0x01) Print_At(13,65,". ON");else Print_At(13,65," OFF");
	if(id32.dos32a_misc2&0x10) Print_At(14,65,". ON");else Print_At(14,65," OFF");

	if(((id32.dos32a_lowbufsize<<4)&0xFFFF)<4096) SetColor(YELLOW);
	if(((id32.dos32a_lowbufsize<<4)&0xFFFF)<1024) { SetColor(LIGHTRED); SetBlink(1); }
	if(hexmode==0)	Print_At(15,44,"%8d",(id32.dos32a_lowbufsize<<4)&0xFFFF);
	else 		Print_At(15,44,"%7Xh",(id32.dos32a_lowbufsize<<4)&0xFFFF);
	SetBlink(0);

	SetColor(LIGHTBLUE);
	if((id32.dos32a_misc&0x01)!=(id32_old.dos32a_misc&0x01)) Print_At(3,69,"*");else Print_At(3,69," ");
	if((id32.dos32a_misc&0x02)!=(id32_old.dos32a_misc&0x02)) Print_At(4,69,"*");else Print_At(4,69," ");
	if((id32.dos32a_misc&0x04)!=(id32_old.dos32a_misc&0x04)) Print_At(5,69,"*");else Print_At(5,69," ");
	if((id32.dos32a_misc&0x08)!=(id32_old.dos32a_misc&0x08)) Print_At(6,69,"*");else Print_At(6,69," ");
	if((id32.dos32a_misc&0x30)!=(id32_old.dos32a_misc&0x30)) Print_At(7,69,"*");else Print_At(7,69," ");
	if((id32.dos32a_misc2&0x04)!=(id32_old.dos32a_misc2&0x04)) Print_At(8,69,"*");else Print_At(8,69," ");
	if((id32.dos32a_misc&0x40)!=(id32_old.dos32a_misc&0x40)) Print_At(9,69,"*");else Print_At(9,69," ");
	if((id32.dos32a_misc2&0x02)!=(id32_old.dos32a_misc2&0x02)) Print_At(10,69,"*");else Print_At(10,69," ");
	if((id32.dos32a_misc&0x80)!=(id32_old.dos32a_misc&0x80)) Print_At(11,69,"*");else Print_At(11,69," ");
	if((id32.dos32a_misc2&0x08)!=(id32_old.dos32a_misc2&0x08)) Print_At(12,69,"*");else Print_At(12,69," ");
	if((id32.dos32a_misc2&0x01)!=(id32_old.dos32a_misc2&0x01)) Print_At(13,69,"*");else Print_At(13,69," ");
	if((id32.dos32a_misc2&0x10)!=(id32_old.dos32a_misc2&0x10)) Print_At(14,69,"*");else Print_At(14,69," ");
	if((id32.dos32a_lowbufsize)!=(id32_old.dos32a_lowbufsize)) Print_At(15,52,"*");else Print_At(15,52," ");
	SetColor(LIGHTWHITE);
}



/*--------------------------------------------------------------------------*/
void SelectExtenderMenu()
{
	int n;

	do {
		ShowModified();
		ShowHelp(2,extendermenu_sel);
		ShowExtenderConfig();
		ShowCursor(	extendermenu_ypos[extendermenu_sel],
				extendermenu_xpos[extendermenu_sel],
				extendermenu_xlen[extendermenu_sel]);
		GetKey();
		ShowCursor(	extendermenu_ypos[extendermenu_sel],
				extendermenu_xpos[extendermenu_sel],
				extendermenu_xlen[extendermenu_sel]);
//		CloseWindow();

		if(keychar>'0' && keychar<='9') extendermenu_sel=keychar-'1';
		if(keychar=='0' || keychar=='a' || keychar=='A') extendermenu_sel=9;
		if(keychar=='b' || keychar=='B') extendermenu_sel=10;
		if(keychar=='c' || keychar=='C') extendermenu_sel=11;
		if(keycode==F3) CreateConfig();
		if(keycode==F4) RestoreConfig();
		if(keycode==F5)	{ id32=id32_old; ClearConfigName(); ShowBannerStatus(); }
		if(keycode==F6) { RestoreDefaults(); ClearConfigName(); }
		if(keycode==F8) ShowMemReq();
		if(keycode==UP) extendermenu_sel--;
		if(keycode==DOWN) extendermenu_sel++;
		if(keycode==HOME) extendermenu_sel=0;
		if(keycode==END) extendermenu_sel=12;
		if(keycode==PAGEUP)
			if(extendermenu_sel>11) extendermenu_sel=11;
			else if(extendermenu_sel>6) extendermenu_sel=6;
			else extendermenu_sel=0;
		if(keycode==PAGEDOWN)
			if(extendermenu_sel<6) extendermenu_sel=6;
			else if(extendermenu_sel<11) extendermenu_sel=11;
			else extendermenu_sel=12;
		if(keycode==TAB) hexmode^=1;
		if(extendermenu_sel<0) extendermenu_sel=0;
		if(extendermenu_sel>12) extendermenu_sel=12;
	} while(keycode!=ESC && keycode!=F1 && keycode!=F2 && keycode!=ENTER && keycode!=SPACE);
}

