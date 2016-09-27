/*
 * Author: Harsh Chaturvedi
 * email id: hchatur1@jhu.edu
 * Objective: Remote host profiler
 * Project: Covert Channels
 * Class: Cloud Computing Security
 * School: Johns Hopkins Univserity
 */
 
/* This is just a place holder for idea.
 * it hasn't been used in the actual implementation
 */
 
/*
 * This program sends 100 pings to 
 * the node mentioned in the input file
 * and stores the response in a target 
 * file
 * Run the binary without arguments to 
 * know the usage
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
/*
 * This function reads the Ping 
 * Output file to extract the reponse
 * times. The response time gets written
 * to an external file provided by the user
 */
int readFile(FILE *fp, float *time, FILE *writeTo)
{
	int i =0,j=0;
	char ch;
	//printf("Hey!");
	while((ch=fgetc(fp))!=EOF)
	{
		if(ch == '=')
		{
			i++;
			//printf("Got %d =\n",i);
		}
		if(i==3)
		{
			i=0;
			//printf("current ch = %c",ch);
			fscanf(fp,"%f",(time+j));
			fprintf(writeTo,"%f\n",time[j]);
			j++;
		}
	}
	return 0;
}
int main(int argc, char *argv[])
{
	if(argc<3)
	{
		printf("Error: Invalid Number of arguments. Usage : <binary> <file to read ip from> <file to write response times to> \n");
		return 0;
	}
	FILE *readFrom = fopen(argv[1],"r");
	FILE *writeTo = fopen(argv[2],"w");
	int ip[4], repeatitions=1;
	char command[70]; // Command container string
	char *ofile = "./PingOut.txt";
	float time[100];
	fscanf(readFrom,"%d %d.%d.%d.%d",&repeatitions,&ip[0],&ip[1],&ip[2],&ip[3]);
	printf("%d repeatitions",repeatitions);
	printf("%d",ip[0]);
	snprintf(command,70, "hping2 -c %d %d.%d.%d.%d > %s", repeatitions,ip[0],ip[1],ip[2],ip[3], ofile); // Constructing the command
	printf("%s",command);
	system(command);
	FILE *outfile = fopen("./PingOut.txt","r"); // The file that will have the ping output
	readFile(outfile,time,writeTo);
	fclose(outfile);
	fclose(readFrom);
	fclose(writeTo);
	return 0;
}
