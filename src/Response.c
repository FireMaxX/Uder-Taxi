/* Response.c*/
/* Decode the commands string receivd from the socket, call functions from management module, response correspondingly and give responsive string back to the socket*/
/* Arthor: Yan Zhang*/
/* 3/19/2017*/
#include "Response.h"
#include "ServerManagement.h"
#include "map.h"
#include "utils.h"
#include "Pathfind.h"
#include "constants.h"
#include "GetTime.h"
#include<stdint.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<assert.h>
#include<regex.h>
#include<stdlib.h>
#include<assert.h>
#include<time.h>
#include<stdlib.h>

int match(const char *string, char *pattern){
	int status;
	regex_t re;
	
	if (regcomp(&re,pattern,REG_EXTENDED|REG_NOSUB)!=0){
		return 0;
	}
	status=regexec(&re,string,(size_t)0,NULL,0);
	regfree(&re);
	if (status==0){
		return 1;
	}
	else if (status==REG_NOMATCH){
		return 0;
	}
	return 2;
}



char* Response(char* req, Map *map,Taxi *taxis[],DriverOrder *DriverOrders[]){
	char *token;
	int Client_ID;
	int ConfirmNum;
	char id[10]="";
	char *preqtaxi="REQUEST_TAXI";
	char *preqposition="REQUEST_POSITION Taxi[0-9]+";
	char *pconfirm="CONFIRM #[0-9]+";
	char *pcancel="CANCEL #[0-9]+";
	char *pres="OK Taxi[0-9]+ PICKUP [A-Z][0-9]+ [0-9]+\\:[0-9]+ DROPOFF [A-Z][0-9]+ [0-9]+\\:[0-9]+ \\$[0-9]+.[0-9]+ CONFIRM \\#[0-9]+";
	char *presconfirm="OK Taxi[0-9]+ POSITION [A-Z][0-9]+ ETA [A-Z][0-9]+ [0-9]{2}\\:[0-9]{2}";
	char start[100]="";
	char end[100]="";
	extern char res[200];
	extern double Profit;
	extern Order *Orders[OrderNum];
	char pos[10]="";
	char taxinum[10]="";
	char *error="ERROR WRONG_REQUEST";
	Point* startp=malloc(sizeof(Point));
	Point* endp=malloc(sizeof(Point));
	Point* currp=malloc(sizeof(Point));
	int i=0,j=0,a=0,b=0,k=0;
	double fee;
	char strafee[10]="";
	char strbfee[10]="";
	int afee;
	int bfee;
	char oktaxi[20]="OK Taxi";
	char confirm[20]=" CONFIRM #";
	char pickup[20]=" PICKUP ";
	char dropoff[20]="DROPOFF ";
	char strposition[20]=" POSITION ";
	char dollar[10]="$";
	char dot[10]=".";
	DriverOrder *driverorder;
	Order *clientorder;
	Route *r;
	char taxi[10]="";
        int num;
	Taxi *currenttaxi;
	Point *nextp=malloc(sizeof(Point));
	DriverOrder* currdriverorder;
	Order* currorder;
	char coord[4]="";
	char currcoord[4]="";
	char ftime[10]=" 00:00 ";
	char feta[10]=" 00:00";
	char zero[10]="0";
	char zeros[10]="00";
	char eta[10]=" ETA ";
	char coordy[10]="";
	char currcoordy[10]="";
	char etacoord[4]="";
	char etacoordy[10]="";

	char endcoord[4]="";
	char endcoordy[10]="";
	char startcoord[4]="";
	char startcoordy[10]="";
	char startmark[100]="";
	char endmark[100]="";

	char row[100]="";
	char column[100]="";

	Array *stands=map->stands;
	Array *landmarks=map->landmarks;
	int rownum=map->row;
	int colnum=map->col;
	int* rows=map->rowNames;
	int* cols=map->colNames;
	
	TaxiStand* stand;
	Landmark* landmark;

	
	int rowcol=0;

	srand(time(NULL));
#ifdef DEBUG
	printf("Response to %s\n", req);
#endif
/*Request Taxi*/
	if (match(req,preqtaxi)==1){            
#ifdef DEBUG
		printf("Request_taxi received\n");
#endif 

/*get end and start*/
		token=strtok(req," \t");
		token=strtok(NULL," \t");

#ifdef DEBUG
		printf("1st %s\n",token);	
#endif 
		
/* coord start*/
		if (match(token,"[A-Z][0-9]+")==1){
			strcpy(start,token);
#ifdef DEBUG
			printf("start position is %s\n",start); 
#endif
			setPoint(startp,getAlphabeticIndex(start,&i),getIntegerIndex(start,&j));
#ifdef DEBUG
			printf("Start point is %d,%d\n",startp->x,startp->y);
#endif

			token=strtok(NULL," \t");
			token=strtok(NULL," \t");  
/* coord -> coord*/
			if (match(token,"[A-Z][0-9]+")==1){
				strcpy(end,token);
#ifdef DEBUG
				printf("end position is %s\n",end); 
#endif 			
				setPoint(endp,getAlphabeticIndex(end,&a),getIntegerIndex(end,&b));

#ifdef DEBUG
				printf("End point is %d,%d\n",endp->x,endp->y);
#endif		
/*coord -> stand*/
			}else if(match(token,"Taxi")==1){
				token=strtok(NULL," \t");
				token=strtok(NULL," \t");
#ifdef DEBUG
				printf("this is stand %s\n",token);
#endif 
				i=atoi(token)+1;
#ifdef DEBUG
				printf("this is stand %d\n",i);
#endif 
				stand=(TaxiStand*)(stands->items+stands->itemSize*i);
				endp=&stand->mark;
#ifdef DEBUG
				printf("end point is %d,%d\n",endp->x,endp->y);	
#endif 
				endcoord[0]=endp->x+65;
				sprintf(endcoordy,"%d",endp->y+1);
				strcat(endcoord,endcoordy);
				strcpy(end,endcoord);
#ifdef DEBUG
				printf("end position is %s\n",end); 
#endif 			
/*coord->corner*/		
			}else if (match(token,"CORNER")==1){
				token=strtok(NULL," \t");
				strcat(row,token);
				token=strtok(NULL," \t");
				while (match(token,"AND")!=1){
					strcat(strcat(row," "),token);
					token=strtok(NULL," \t");
				}
#ifdef DEBUG
				printf("corner row is %snospace\n",row);
#endif 
				
				token=strtok(NULL," \t");
				strcat(column,token);
				token=strtok(NULL," \t");
				while (token){
					strcat(strcat(column," "),token);
					token=strtok(NULL," \t");
				}
#ifdef DEBUG
				printf("corner column is %snospace\n",column);
#endif 	

				for (i=0;i<rownum;i++){
					if (strcmp(row,(char*)(uintptr_t)rows[i])==0){
#ifdef DEBUG
						printf("it is %dth row\n",i);
#endif 			
						rowcol=1;
						break;
					}
				}

				if (rowcol==1){
					for (j=0;j<colnum;j++){
						if (strcmp(column,(char*)(uintptr_t)cols[j])==0){
#ifdef DEBUG
							printf("it is %dth col\n",j);
#endif 
							
							break;
						}
					}

				}
				if (rowcol==0){
					for (i=0;i<colnum;i++){
						if (strcmp(row,(char*)(uintptr_t)cols[i])==0){
#ifdef DEBUG
							printf("it is %dth col\n",i);
#endif 
							break;
						}
					}
					for (j=0;j<rownum;j++){
						if (strcmp(column,(char*)(uintptr_t)rows[j])==0){
#ifdef DEBUG
							printf("it is %dth row\n",j);
#endif 
							break;
						}
					}
					k=i;
					i=j;
					j=k;

				}
				
				endcoord[0]=i+65;
				sprintf(endcoordy,"%d",j+1);
				strcat(endcoord,endcoordy);
				strcpy(end,endcoord);	
#ifdef DEBUG
				printf("end position is %s\n",end); 
#endif 
				
				setPoint(endp,getAlphabeticIndex(end,&a),getIntegerIndex(end,&b));
#ifdef DEBUG
				printf("End point is %d,%d\n",endp->x,endp->y);	
#endif 
				
/*coord->landmark*/			
			}else{
				strcat(endmark,token);
				token=strtok(NULL," \t");
				while (token){
					strcat(strcat(endmark," "),token);
					printf("%s\n",endmark);
					token=strtok(NULL," \t");
				}
#ifdef DEBUG
				printf("end landmark is %snospace\n",endmark);
#endif 
				
				for (i=0;i<landmarks->size;i++){
					landmark=(Landmark*)(landmarks->items+landmarks->itemSize*i);
#ifdef DEBUG
					printf("this landmark is %s\n", landmark->name);
#endif 
					if (strcmp(endmark,landmark->name)==0){
#ifdef DEBUG
						printf("found the landmark");
#endif 
						
						endp=&landmark->mark;
						break;
					}
				}
#ifdef DEBUG
				printf("end point is %d,%d\n",endp->x,endp->y);	
#endif 
				
				endcoord[0]=endp->x+65;
				sprintf(endcoordy,"%d",endp->y+1);
				strcat(endcoord,endcoordy);
				strcpy(end,endcoord);
#ifdef DEBUG
				printf("end position is %s\n",start);
#endif 
		
			}
			

/*stand start*/
		}else if(match(token,"Taxi")==1){
			token=strtok(NULL," \t");
			token=strtok(NULL," \t");
#ifdef DEBUG
			printf("this is stand %s\n",token);
#endif 
			
			i=atoi(token)+1;
#ifdef DEBUG
			printf("this is stand %d\n",i);
#endif 
			stand=(TaxiStand*)(stands->items+stands->itemSize*i);
			startp=&stand->mark;
#ifdef DEBUG
			printf("start point is %d,%d\n",startp->x,startp->y);	
#endif 			
			startcoord[0]=startp->x+65;
			sprintf(startcoordy,"%d",startp->y+1);
			strcat(startcoord,startcoordy);
			strcpy(start,startcoord);
#ifdef DEBUG
			printf("start position is %s\n",start); 
#endif 			

			token=strtok(NULL," \t");
			token=strtok(NULL," \t");
/*stand -> coord*/
			if (match(token,"[A-Z][0-9]+")==1){
				strcpy(end,token);
#ifdef DEBUG
				printf("end position is %s\n",end); 
#endif 			
				setPoint(endp,getAlphabeticIndex(end,&a),getIntegerIndex(end,&b));

#ifdef DEBUG
				printf("End point is %d,%d\n",endp->x,endp->y);
#endif		
/*stand -> stand*/
			}else if(match(token,"Taxi")==1){
				token=strtok(NULL," \t");
				token=strtok(NULL," \t");
#ifdef DEBUG
				printf("this is stand %s\n",token);
#endif 
				
				i=atoi(token)+1;
#ifdef DEBUG
				printf("this is stand %d\n",i);
#endif 
				
				stand=(TaxiStand*)(stands->items+stands->itemSize*i);
				endp=&stand->mark;
#ifdef DEBUG
				printf("end point is %d,%d\n",endp->x,endp->y);	
#endif 
				endcoord[0]=endp->x+65;
				sprintf(endcoordy,"%d",endp->y+1);
				strcat(endcoord,endcoordy);
				strcpy(end,endcoord);
#ifdef DEBUG
				printf("end position is %s\n",end); 
#endif 		
/*stand -> corner*/		
			}else if(match(token,"CORNER")==1){
				token=strtok(NULL," \t");
				strcat(row,token);
				token=strtok(NULL," \t");
				while (match(token,"AND")!=1){
					strcat(strcat(row," "),token);
					token=strtok(NULL," \t");
				}

#ifdef DEBUG
				printf("corner row is %snospace\n",row);
#endif 
				

				token=strtok(NULL," \t");
				strcat(column,token);
				token=strtok(NULL," \t");
				while (token){
					strcat(strcat(column," "),token);
					token=strtok(NULL," \t");
				}
#ifdef DEBUG
				printf("corner column is %snospace\n",column);
#endif 
				
				for (i=0;i<rownum;i++){
					if (strcmp(row,(char*)(uintptr_t)rows[i])==0){
#ifdef DEBUG
						printf("it is %dth row\n",i);
#endif 
						
						rowcol=1;
						break;
					}
				}
				if (rowcol==1){
					for (j=0;j<colnum;j++){
						if (strcmp(column,(char*)(uintptr_t)cols[j])==0){
#ifdef DEBUG
							printf("it is %dth col\n",j);
#endif 	
							break;
						}
					}

				}
				if (rowcol==0){
					for (i=0;i<colnum;i++){
						if (strcmp(row,(char*)(uintptr_t)cols[i])==0){
#ifdef DEBUG
							printf("it is %dth col\n",i);
#endif 
							
							break;
						}
					}
					for (j=0;j<rownum;j++){
						if (strcmp(column,(char*)(uintptr_t)rows[j])==0){

#ifdef DEBUG
							printf("it is %dth row\n",j);
#endif 
							
							break;
						}
					}
					k=i;
					i=j;
					j=k;

				}

				
				endcoord[0]=i+65;
				sprintf(endcoordy,"%d",j+1);
				strcat(endcoord,endcoordy);
				strcpy(end,endcoord);	

#ifdef DEBUG
				printf("end position is %s\n",end); 
#endif 
				
				setPoint(endp,getAlphabeticIndex(end,&a),getIntegerIndex(end,&b));
#ifdef DEBUG
				printf("End point is %d,%d\n",endp->x,endp->y);
#endif 
				
/*stand -> landmark*/	
			}else{
				strcat(endmark,token);
				token=strtok(NULL," \t");
				while (token){
					strcat(strcat(endmark," "),token);
					printf("%s\n",endmark);
					token=strtok(NULL," \t");
				}
#ifdef DEBUG
				printf("end landmark is %snospace\n",endmark);
#endif 
				
				for (i=0;i<landmarks->size;i++){
					landmark=(Landmark*)(landmarks->items+landmarks->itemSize*i);
#ifdef DEBUG
					printf("this landmark is %s\n",landmark->name);
#endif 
					
					if (strcmp(endmark,landmark->name)==0){
#ifdef DEBUG
						printf("found the landmark");
#endif 
						
						endp=&landmark->mark;
						break;
					}
				}
#ifdef DEBUG
				printf("end point is %d,%d\n",endp->x,endp->y);	
#endif 
				
				endcoord[0]=endp->x+65;
				sprintf(endcoordy,"%d",endp->y+1);
				strcat(endcoord,endcoordy);
				strcpy(end,endcoord);
#ifdef DEBUG
				printf("end position is %s\n",start);
#endif 
				


			}
/*corner start*/
		}else if (match(token,"CORNER")==1){
			token=strtok(NULL," \t");
			strcat(row,token);
			token=strtok(NULL," \t");
			while (match(token,"AND")!=1){
				strcat(strcat(row," "),token);
				token=strtok(NULL," \t");
			}

#ifdef DEBUG
			printf("corner row is %snospace\n",row);
#endif 
			

			token=strtok(NULL," \t");
			strcat(column,token);
			token=strtok(NULL," \t");
			while (match(token,"TO")!=1){
				strcat(strcat(column," "),token);
				token=strtok(NULL," \t");
			}

#ifdef DEBUG
			printf("corner column is %snospace\n",column);
#endif 
			

			for (i=0;i<rownum;i++){
				if (strcmp(row,(char*)(uintptr_t)rows[i])==0){
#ifdef DEBUG
					printf("it is %dth row\n",i);
#endif 
					
					rowcol=1;
					break;
				}
			}
			if (rowcol==1){
				for (j=0;j<colnum;j++){
					if (strcmp(column,(char*)(uintptr_t)cols[j])==0){
#ifdef DEBUG
						printf("it is %dth col\n",j);
#endif 
						
						break;
					}
				}

			}
			if (rowcol==0){
				for (i=0;i<colnum;i++){
					if (strcmp(row,(char*)(uintptr_t)cols[i])==0){
#ifdef DEBUG
						printf("it is %dth col\n",i);
#endif 
						
						break;
					}
				}
				for (j=0;j<rownum;j++){
					if (strcmp(column,(char*)(uintptr_t)rows[j])==0){
#ifdef DEBUG
						printf("it is %dth row\n",j);
#endif 
						
						break;
					}
				}
				k=i;
				i=j;
				j=k;

			}
		

			startcoord[0]=i+65;
			sprintf(startcoordy,"%d",j+1);
			strcat(startcoord,startcoordy);
			strcpy(start,startcoord);	
#ifdef DEBUG
			printf("start position is %s\n",start); 
#endif 
			
			setPoint(startp,getAlphabeticIndex(start,&a),getIntegerIndex(start,&b));
#ifdef DEBUG
			printf("start point is %d,%d\n",startp->x,startp->y);
#endif 
			
			token=strtok(NULL," \t");
/*corner ->coord*/
			if (match(token,"[A-Z][0-9]+")==1){
				strcpy(end,token);
#ifdef DEBUG
				printf("end position is %s\n",end); 
#endif 			
				setPoint(endp,getAlphabeticIndex(end,&a),getIntegerIndex(end,&b));

#ifdef DEBUG
				printf("End point is %d,%d\n",endp->x,endp->y);
#endif		
/*corner -> stand*/
			}else if(match(token,"Taxi")==1){
				token=strtok(NULL," \t");
				token=strtok(NULL," \t");
#ifdef DEBUG
				printf("this is stand %s\n",token);
#endif 
				
				i=atoi(token)+1;

#ifdef DEBUG
				printf("this is stand %d\n",i);
#endif 
				
				stand=(TaxiStand*)(stands->items+stands->itemSize*i);
				endp=&stand->mark;
#ifdef DEBUG
				printf("end point is %d,%d\n",endp->x,endp->y);	
#endif 
				

				endcoord[0]=endp->x+65;
				sprintf(endcoordy,"%d",endp->y+1);
				strcat(endcoord,endcoordy);
				strcpy(end,endcoord);
#ifdef DEBUG
				printf("end position is %s\n",end); 
#endif 		
/*corner->corner*/
			}else if(match(token,"CORNER")==1){
				token=strtok(NULL," \t");
				strcat(row,token);
				token=strtok(NULL," \t");
				while (match(token,"AND")!=1){
					strcat(strcat(row," "),token);
					token=strtok(NULL," \t");
				}

#ifdef DEBUG
				printf("corner row is %snospace\n",row);
#endif 
				

				token=strtok(NULL," \t");
				strcat(column,token);
				token=strtok(NULL," \t");
				while (token){
					strcat(strcat(column," "),token);
					token=strtok(NULL," \t");
				}

#ifdef DEBUG
				printf("corner column is %snospace\n",column);
#endif 
				

				for (i=0;i<rownum;i++){
					if (strcmp(row,(char*)(uintptr_t)rows[i])==0){

#ifdef DEBUG
						printf("it is %dth row\n",i);
#endif 
						
						rowcol=1;
						break;
					}
				}
				if (rowcol==1){
					for (j=0;j<colnum;j++){
						if (strcmp(column,(char*)(uintptr_t)cols[j])==0){

#ifdef DEBUG
							printf("it is %dth col\n",j);
#endif 
							
							break;
						}
					}

				}
				if (rowcol==0){
					for (i=0;i<colnum;i++){
						if (strcmp(row,(char*)(uintptr_t)cols[i])==0){

#ifdef DEBUG
							printf("it is %dth col\n",i);
#endif 
							
							break;
						}
					}
					for (j=0;j<rownum;j++){
						if (strcmp(column,(char*)(uintptr_t)rows[j])==0){

#ifdef DEBUG
							printf("it is %dth row\n",j);
#endif 
							
							break;
						}
					}
					k=i;
					i=j;
					j=k;

				}

				endcoord[0]=i+65;
				sprintf(endcoordy,"%d",j+1);
				strcat(endcoord,endcoordy);
				strcpy(end,endcoord);
#ifdef DEBUG
				printf("end position is %s\n",end);
#endif 
				 
				setPoint(endp,getAlphabeticIndex(end,&a),getIntegerIndex(end,&b));

#ifdef DEBUG
				printf("End point is %d,%d\n",endp->x,endp->y);	
#endif 
				
/*corner -> landmark*/	
			}else{
				strcat(endmark,token);
				token=strtok(NULL," \t");
				while (token){
					strcat(strcat(endmark," "),token);
					printf("%s\n",endmark);
					token=strtok(NULL," \t");
				}
#ifdef DEBUG
				printf("end landmark is %snospace\n",endmark);
#endif 
				
				for (i=0;i<landmarks->size;i++){
					landmark=(Landmark*)(landmarks->items+landmarks->itemSize*i);
#ifdef DEBUG
					printf("this landmark is %s\n",landmark->name);
#endif 
					
					if (strcmp(endmark,landmark->name)==0){
#ifdef DEBUG
						printf("found the landmark");
#endif 
						
						endp=&landmark->mark;
						break;
					}
				}
#ifdef DEBUG
				printf("end point is %d,%d\n",endp->x,endp->y);	
#endif 
				
				endcoord[0]=endp->x+65;
				sprintf(endcoordy,"%d",endp->y+1);
				strcat(endcoord,endcoordy);
				strcpy(end,endcoord);

#ifdef DEBUG
				printf("end position is %s\n",start);
#endif 
				
			}

/*landmark start*/
		}else{
			strcat(startmark,token);
			token=strtok(NULL," \t");
			while (match(token,"TO")!=1){
				strcat(strcat(startmark," "),token);
#ifdef DEBUG
				printf("%s\n",startmark);
#endif
				token=strtok(NULL," \t");

			}

#ifdef DEBUG
			printf("start landmark is %snospace\n",startmark);
#endif 
			
			for (i=0;i<landmarks->size;i++){
				landmark=(Landmark*)(landmarks->items+landmarks->itemSize*i);
#ifdef DEBUG
				printf("this landmark is %s\n",landmark->name);
#endif 
				
				if (strcmp(startmark,landmark->name)==0){
#ifdef DEBUG
					printf("found the landmark");
#endif 
					
					startp=&landmark->mark;
					break;
				}
			}
#ifdef DEBUG
			printf("start point is %d,%d\n",startp->x,startp->y);	
#endif 
			
			startcoord[0]=startp->x+65;
			sprintf(startcoordy,"%d",startp->y+1);
			strcat(startcoord,startcoordy);
			strcpy(start,startcoord);
#ifdef DEBUG
			printf("start position is %s\n",start);
#endif 
			
			token=strtok(NULL," \t");
/*landmark -> coord*/
			if (match(token,"[A-Z][0-9]+")==1){
				strcpy(end,token);
#ifdef DEBUG
				printf("end position is %s\n",end); 
#endif 			
				setPoint(endp,getAlphabeticIndex(end,&a),getIntegerIndex(end,&b));

#ifdef DEBUG
				printf("End point is %d,%d\n",endp->x,endp->y);
#endif		
/*landmark -> stand*/
			}else if(match(token,"Taxi")==1){
				token=strtok(NULL," \t");
				token=strtok(NULL," \t");
#ifdef DEBUG
				printf("this is stand %s\n",token);
#endif 
				
				i=atoi(token)+1;
#ifdef DEBUG
				printf("this is stand %d\n",i);
#endif 
				
				stand=(TaxiStand*)(stands->items+stands->itemSize*i);
				endp=&stand->mark;
#ifdef DEBUG
				printf("end point is %d,%d\n",endp->x,endp->y);	
#endif 
				

				endcoord[0]=endp->x+65;
				sprintf(endcoordy,"%d",endp->y+1);
				strcat(endcoord,endcoordy);
				strcpy(end,endcoord);
#ifdef DEBUG
				printf("end position is %s\n",end); 
#endif 		
/*landmark->corner*/
			}else if(match(token,"CORNER")==1){
				token=strtok(NULL," \t");
				strcat(row,token);
				token=strtok(NULL," \t");
				while (match(token,"AND")!=1){
					strcat(strcat(row," "),token);
					token=strtok(NULL," \t");
				}
#ifdef DEBUG
				printf("corner row is %snospace\n",row);
#endif 
				

				token=strtok(NULL," \t");
				strcat(column,token);
				token=strtok(NULL," \t");
				while (token){
					strcat(strcat(column," "),token);
					token=strtok(NULL," \t");
				}
#ifdef DEBUG
				printf("corner column is %snospace\n",column);
#endif 
				

				for (i=0;i<rownum;i++){
					if (strcmp(row,(char*)(uintptr_t)rows[i])==0){
#ifdef DEBUG
						printf("it is %dth row\n",i);
#endif 
						
						rowcol=1;
						break;
					}
				}
				if (rowcol==1){
					for (j=0;j<colnum;j++){
						if (strcmp(column,(char*)(uintptr_t)cols[j])==0){
#ifdef DEBUG
							printf("it is %dth col\n",j);
#endif 
							
							break;
						}
					}

				}
				if (rowcol==0){
					for (i=0;i<colnum;i++){
						if (strcmp(row,(char*)(uintptr_t)cols[i])==0){
#ifdef DEBUG
							printf("it is %dth col\n",i);
#endif
							
							break;
						}
					}
					for (j=0;j<rownum;j++){
						if (strcmp(column,(char*)(uintptr_t)rows[j])==0){
#ifdef DEBUG
							printf("it is %dth row\n",j);
#endif 
							
							break;
						}
					}
					k=i;
					i=j;
					j=k;

				}
			

				endcoord[0]=i+65;
				sprintf(endcoordy,"%d",j+1);
				strcat(endcoord,endcoordy);
				strcpy(end,endcoord);
#ifdef DEBUG
				printf("end position is %s\n",end); 
#endif 
				
				setPoint(endp,getAlphabeticIndex(end,&a),getIntegerIndex(end,&b));

#ifdef DEBUG
				printf("End point is %d,%d\n",endp->x,endp->y);	
#endif 
				
			
/*landmark -> landmark*/	
			}else{
				strcat(endmark,token);
				token=strtok(NULL," \t");
				while (token){
					strcat(strcat(endmark," "),token);
#ifdef DEBUG
					printf("%s\n",endmark);
#endif
					token=strtok(NULL," \t");
				}
#ifdef DEBUG
				printf("end landmark is %snospace\n",endmark);
#endif 
				
				for (i=0;i<landmarks->size;i++){
					landmark=(Landmark*)(landmarks->items+landmarks->itemSize*i);
#ifdef DEBUG
					printf("this landmark is %s\n",landmark->name);
#endif 
					
					if (strcmp(endmark,landmark->name)==0){
#ifdef DEBUG
						printf("found the landmark");
#endif 
						
						endp=&landmark->mark;
						break;
					}
				}
#ifdef DEBUG
				printf("end point is %d,%d\n",endp->x,endp->y);	
#endif 
				
				endcoord[0]=endp->x+65;
				sprintf(endcoordy,"%d",endp->y+1);
				strcat(endcoord,endcoordy);
				strcpy(end,endcoord);
#ifdef DEBUG
				printf("end position is %s\n",end);
#endif 
				

			}	

			
		}		

		if (strcmp(start,end)==0){
			stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(res,oktaxi),zero),pickup),start),ftime),dropoff),end),ftime),dollar),zeros),dot),zeros),confirm),zero);
			return res;
		}

		r=Pathfind(*startp,*endp,map);
		fee=Fee(startp,r,map);
#ifdef DEBUG
		printf("fee is %.2f\n",fee);
#endif
		afee=(int)fee;
#ifdef DEBUG
		printf("int fee is %d\n",afee);
#endif
		bfee=(int)((fee-afee)*100);
#ifdef DEBUG
		printf("frac fee is %d\n",bfee);
#endif		
		if (afee==0){
			strcpy(strafee,"0");
		}else{
			my_itoa(afee,strafee,10);
		}
		if (bfee==0){
			strcpy(strbfee,"00");
		}else{
			my_itoa(bfee,strbfee,10);
		}
		

		Client_ID=rand()%100000;
		clientorder=CreateNewOrder(Client_ID,4,startp,endp);
		driverorder=CreateNewDriverOrder(Client_ID);
		driverorder=ProcessOrder(clientorder,taxis,map);
		AppendDriverOrder(DriverOrders,driverorder);
		AppendOrder(Orders,clientorder);
#ifdef DEBUG		
		printf("it is taxi at %d %d\n",driverorder->taxi->position->x,driverorder->taxi->position->y);
#endif		
		sprintf(taxinum,"%d",driverorder->taxi->num);
#ifdef DEBUG
		printf("taxinum is %s\n",taxinum);
#endif		
		sprintf(id,"%d",driverorder->confirm_num[0]);		
			
		stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(res,oktaxi),taxinum),pickup),start),ftime),dropoff),end),ftime),dollar),strafee),dot),strbfee),confirm),id);
#ifdef DEBUG
		printf("%s\n",res);
#endif
		assert(match(res,pres)==1);
		return res;
	}

/*Request Position*/
	else if(match(req,preqposition)==1){
#ifdef DEBUG
		printf("Request_position received\n ");
#endif
		token=strtok(req," \t");
		token=strtok(NULL," \t");
#ifdef DEBUG
		printf("token is %s\n",token);
#endif
		stpcpy(taxi,token);
#ifdef DEBUG
		printf("taxi is %s\n", taxi);
#endif
		token=strtok(taxi,"i");
		token=strtok(NULL,"\0");
		num=atoi(token);
#ifdef DEBUG
		printf("%d\n",num);
#endif
		for (i=0;i<MaxTaxi;i++){
			if (taxis[i]->num==num){
				currenttaxi=taxis[i];
#ifdef DEBUG
				printf("taxi%d 's num is %d\n",num,taxis[i]->num);
#endif

				break;
			}

		}
		
		ConfirmNum=currenttaxi->confirm_num[0];
#ifdef DEBUG
		printf("this taxi's conf num is %d\n",ConfirmNum);
#endif		
		for (i=0;i<MaxTaxi;i++){
			if (DriverOrders[i]->confirm_num[0]==ConfirmNum){
				currdriverorder=DriverOrders[i];
			}
		}
		
#ifdef DEBUG
		printf("got currdriverorder\n");
#endif	
		for (i=0;i<OrderNum;i++){
			if (Orders[i]->confirm_num==ConfirmNum){
				currorder=Orders[i];
			}
		}
#ifdef DEBUG
		printf("got currorder\n");	
#endif	
		nextp=NextStep(currdriverorder->taxiRoute);
#ifdef DEBUG
		PrintList(currdriverorder->taxiRoute);
#endif
		
		if ((nextp->x==currenttaxi->position->x) && (nextp->y==currenttaxi->position->y)){
#ifdef DEBUG
			printf("Back to Stand!\n");
#endif 
			
			RemoveOrder(Orders,ConfirmNum);
			RemoveDriverOrder(DriverOrders,ConfirmNum);
			currenttaxi->confirm_num[0]=0;
			currenttaxi->confirm_num[1]=0;
#ifdef DEBUG
			printf("DriverOrder and Order Removed!!!!!!\n");	
#endif 
			
		}else{
			currenttaxi->position->x=nextp->x;
			currenttaxi->position->y=nextp->y;
			Profit=Profit-0.20;
		
#ifdef DEBUG
			printf("currenttaxi is %d\n",currenttaxi->num);
			printf("success moved\n");		
			printf("%d %d\n",nextp->x,nextp->y);
#endif
		}
		
		if (currenttaxi->position->x==currorder->target_pos->x && currenttaxi->position->y==currorder->target_pos->y && currenttaxi->status==1){
#ifdef DEBUG
			printf("Arrived! Adding Profit\n");
			printf("fee calculated in order is %.2f\n",currorder->fee);
#endif 
			
			Profit=Profit+currorder->fee;
#ifdef DEBUG
			printf("Now Profit is %.2f\n",Profit);
#endif 
			
			currenttaxi->status=0;
		}
		coord[0]=nextp->x+65;
		sprintf(coordy,"%d",nextp->y+1);
		strcat(coord,coordy);

		etacoord[0]=currorder->target_pos->x+65;
		sprintf(etacoordy,"%d",currorder->target_pos->y+1);
		strcat(etacoord,etacoordy);
#ifdef DEBUG
		printf("%s\n",coord);
		printf("eta coord is %s\n",etacoord);
#endif
		
		strcpy(pos,coord);
#ifdef DEBUG
		printf("position is %s\n",pos);
#endif
		stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(res,oktaxi),token),strposition),pos),eta),etacoord),feta);
		
#ifdef DEBUG
		printf("%s\n",res);
#endif
		assert(match(res,presconfirm)==1);
		return res;
	}
/*Cancel*/
	else if (match(req,pcancel)==1){
#ifdef DEBUG
		printf("cancel received\n");
#endif
		token=strtok(req," \t");
		token=strtok(NULL," \t");
		Client_ID=atoi(token);
#ifdef DEBUG
		printf("ID string is %s\n",token);
		printf("ID is %d\n",Client_ID);
#endif
		RemoveDriverOrder(DriverOrders,Client_ID);
		RemoveOrder(Orders,Client_ID);
#ifdef DEBUG
		printf("successfully clear the orders\n");
#endif
		stpcpy(res,"DECLINED USER_CANCEL");

		return res;
		
	}

/*CONFIRM*/
	else if (match(req,pconfirm)==1){
		token=strtok(req,"#");
		token=strtok(NULL,"#");
		ConfirmNum=atoi(token);
#ifdef DEBUG
		printf("confirm num is %d\n",ConfirmNum);
#endif 
		
		for (i=0;i<MAXTAXI;i++){
			if (taxis[i]->confirm_num[0]==ConfirmNum || taxis[i]->confirm_num[1]==ConfirmNum){
				currenttaxi=taxis[i];
				break;
			}
		}
		num=currenttaxi->num;
		sprintf(taxinum,"%d",num);
#ifdef DEBUG
		printf("taxi is no. %s\n",taxinum);
#endif 
		
		currp=currenttaxi->position;
		currcoord[0]=currp->x+65;
		sprintf(currcoordy,"%d",currp->y+1);
		strcat(currcoord,currcoordy);
#ifdef DEBUG
		printf("current location is%s\n",currcoord);
#endif 
		

		for (i=0;i<OrderNum;i++){
			if (Orders[i]->confirm_num==ConfirmNum){
				currorder=Orders[i];
				break;
			}
		}
		etacoord[0]=currorder->target_pos->x+65;
		sprintf(etacoordy,"%d",currorder->target_pos->y+1);
		strcat(etacoord,etacoordy);

		stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(stpcpy(res,oktaxi),taxinum),strposition),currcoord),eta),etacoord),feta);
		assert(match(res,presconfirm)==1);
		return res;
	}

/*ERROR*/
	else{
		return error;
	}		
}
