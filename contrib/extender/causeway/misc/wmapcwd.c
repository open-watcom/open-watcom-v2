/*
	Convert Watcom MAP files to format digestable by CWD
*/
#include <stdio.h>
#include <string.h>

char InputFile[82],OutputFile[82];
char IOBuffer[255];
FILE *fpin,*fpout;

void main(int argc, char **argv)
{
	if(argc<2){
		printf("\nInput MAP file?\n");
		fgets(InputFile,80,stdin);
	}
	else{
		strcpy(InputFile,argv[1]);
	}
	if(argc<3){
		printf("\nOutput MAP file?\n");
		fgets(OutputFile,80,stdin);
	}
	else{
		strcpy(OutputFile,argv[2]);
	}
	if(!strcmp(InputFile,OutputFile)){
		printf("\nInput file name and output file name must be different!\007");
		exit(1);
	}
	ParseMapFile();
}

void ParseMapFile()
{
	int pos,len,len2,i;
	int ParseFlag=0,BlankLineFlag=0,Target1MatchFlag=0;
	char *TargetString1="Address";
	char *TargetString2="Symbol";
//	char *HeaderString="Address        Publics by Name\n";
	char *HeaderString="Address                 Publics by name\n";

	fpin=fopen(InputFile,"ra");
	if(fpin==NULL){
		printf("\nError opening file: %s \007",InputFile);
		exit(1);
	}
	fpout=fopen(OutputFile,"wa");
	if(fpout==NULL){
		printf("\nError opening file: %s \007",OutputFile);
		exit(1);
	}
    while(fpin!=NULL && fgets(IOBuffer,255,fpin)!=NULL){	// read until file end
		if(ParseFlag==2){	// processing symbols
			pos=0;
			len=strlen(IOBuffer);
			while(len && IOBuffer[pos]==' ' || IOBuffer[pos]==9){
				pos++;
				len--;
			}
			if(len && isdigit(IOBuffer[pos])){	// valid symbol entry
//				if(IOBuffer[pos+13]!='+'){	// kill special symbol sign, ignore locals
					IOBuffer[pos+13]=' ';
					fputs(IOBuffer,fpout);
//				}
			}
			if(!len){	// done processing symbol entry list
				break;
			}
		}
		else if(ParseFlag==1){	// found header, need to process symbols
			pos=0;
			len=strlen(IOBuffer);
			while(len && IOBuffer[pos]==' ' || IOBuffer[pos]==9){
				pos++;
				len--;
			}
			if(!len){	// blank line
				if(!BlankLineFlag){
					BlankLineFlag=1;	// flag blank line occurred
				}
				else{	// only one blank line allowed
					break;
				}
			}
			else if(isdigit(IOBuffer[pos])){	// we're at symbols now
				fputs(HeaderString,fpout);
//				if(IOBuffer[pos+13]!='+'){	// kill special symbol sign, ignore locals
					IOBuffer[pos+13]=' ';
					fputs(IOBuffer,fpout);
//				}
				ParseFlag=2;
			}
		}
		else{	// not found symbol header
			len=strlen(TargetString1);
			pos=0;
			len2=strlen(IOBuffer);
			Target1MatchFlag=0;
			for(i=0;i<=len2-len;i++){
				if(!strnicmp(&IOBuffer[pos],TargetString1,len)){
					Target1MatchFlag=1;
					pos+=len;
					break;
				}
				if(IOBuffer[pos]!=' ' && IOBuffer[pos]!=0){
					break;	// text in front of first target string
				}
				pos++;
			}
			if(Target1MatchFlag){
				len=strlen(TargetString2);
				len2=strlen(&IOBuffer[pos]);
				for(i=0;i<=len2-len;i++){
					if(!strnicmp(&IOBuffer[pos],TargetString2,len)){
						ParseFlag=1;
						break;
					}
					if(IOBuffer[pos]!=' ' && IOBuffer[pos]!=0){
						break;	// text in front of second target string
					}
					pos++;
				}
			}
		}
	}
}
