/*
* Author: Harsh Chaturvedi
* email id: hchatur1@jhu.edu
* Objective: Remote host profiler
* Project: Covert Channels
* Class: Cloud Computing Security
* School: Johns Hopkins Univserity
*/

/*
 * Please read the function specific
 * explanations.
 */
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdint.h>

int decider(char *command, char mode, float *nload);
int automata(uint8_t seq, float *nload);
int covert_data(uint8_t *data,float *nload);

/*
 * Main function
 */
int main(int argc, char *argv[])
{
	/*if(argc<3)
	{
		printf("Error: Invalid Number of arguments. Usage : <binary> <file to read ip from> <file to write response times to> \n");
		return 0;
	}*/
	int ret =0;
	//FILE *readFrom = fopen(argv[1],"r");
	//int ip[4], repeatitions=1, interval=100;
	char command[70]; // Command container string
	float nload=0; // nload is the normal load value that has to be considered
	//fscanf(readFrom,"%d %d.%d.%d.%d",&repeatitions,&interval,&ip[0],&ip[1],&ip[2],&ip[3]);
	//printf("%d repeatitions",repeatitions);
	//printf("%d",ip[0]);
	//snprintf(command,70, "hping3 -c %d -i u%d %d.%d.%d.%d", repeatitions,interval,ip[0],ip[1],ip[2],ip[3], ofile); // Constructing the command
	//printf("%s",command);
	printf("==========Profiling normal load==========");
	if((ret = decider(command,'n',&nload))==-1)
	{
		printf("Error in hping command, please recheck your binaries, exiting\n");
		return 0;
	}
	printf("==========Listening for covert data==========");
	if((ret = decider(command, 'c',&nload))==-1)
	{
		printf("Error in hping command, please recheck your binaries, exiting\n");
		return 0;
	}
	printf("\n Reached End of Program Thank You!\n");
	return 0;
}

/*
 * This is a multipurpose function. It can be run
 * in two modes
 * 1. Normalizing Mode: In this mode, it calculates the load that
 * has to be considered as normal. The value returned
 * by hping would be an average rtt of 1000 packets.
 * This code takes average of 10 such averages and returns
 * an expected normal load.
 * The code needs to run in this mode for the initial time when
 * the stressor is sleeping.
 * 2. Covert Mode: Here the code goes into listening mode.
 * First it will listen for an initial sequence that marks
 * the beginning of interesting data (automata). Once it has
 * got the initial sequence it will start listening for actual
 * data (covert_data).
 */

int decider(char *command, char mode, float *nload)
{
	FILE *fp;
	int ret =0,i;
	float avg_rtt,total=0;
	uint8_t seq=0,set=1,unset=0;
	uint8_t data[125];
	for(i =0;i<125;i++) data[i]=0;
	//Profiling 'n'ormal modeo
	if(mode == 'n')
	{
		*nload =0;
		for(i=0;i<50;i++)
		{
			/*
			 * Need to automate this command, remove constants and
			 * take them as input from a file or from user
			 */
			fp = popen("hping3 -1 -c 1000 -i u2000 10.254.0.1","r");
			if(fp == NULL)
			{
				//printf("Could not run hping2 command please check your binaries");
				return -1;
			}
			fscanf(fp,"%f",&avg_rtt);
			printf("\n%f\n",avg_rtt);
			total+=avg_rtt;
			pclose(fp);
		}
		*nload = total/10;
		printf("\nNormal load is %f\n",(*nload));
		return 0;
	}
	//Listening in 'c'overt mode
	else if(mode == 'c')
	{
		/*
		* Look for the initial sequence of
		* 10101010 first. This would mean
		* that the message has started
		* Make an automaton of values =
		* [0-2-2-10-10-42-42-170]
		*/
		while(1)
		{
			fp = popen("hping3 -1 -c 1000 -i u2000 10.254.0.1","r");
			if(fp == NULL)
			{
				//printf("Could not run hping2 command please check your binaries");
				return -1;
			}
			fscanf(fp,"%f",&avg_rtt);
			printf("\navg : %f\n",avg_rtt);
			//if(avg_rtt > ((*nload)+4))
			if(avg_rtt > 10.5)
				seq = seq | set;
			else
				seq = seq | unset;
			if(seq == 1)
			{
				printf("\n got 1 sending automata\n");
				ret = automata(seq,(nload));
				printf("\n returned from automata\n");
			}
			else
				continue;
			if(ret == -1)
				return -1;
			else if(ret ==0)
			{
				printf("\nheading to covert_data\n");
				if((ret = covert_data(data,(nload)))==-1)
					return -1;
				else
				{
					for(i =0;i<125;i++)
					{
						printf(" %c ",data[i]);
					}
				}
					//sleep(1); // Need to figure out how
									 // how to parallelize it
									 // with writing data in memory
									 // to file.
			}
		}
	}
	return 0;
}

/*
 * This function looks for a particular
 * intial sequence that marks the beginning
 * of interesting data. The sequence being
 * 10101010. As soon as the sequence is seen,
 * it will return with a 0.
 */

 /*
  * Not sure if ndian-ness has to be taken
	* care of here, since we are considering
	* one byte of data.
  */
int automata(uint8_t seq, float *nload)
{
	FILE *fp;
	float avg_rtt;
	uint8_t set=1,unset=0;
	do
	{
		fp = popen("hping3 -1 -c 1000 -i u2000 10.254.0.1","r");
		if(fp == NULL)
		{
			//printf("Could not run hping2 command please check your binaries");
			return -1;
		}
		fscanf(fp,"%f",&avg_rtt);
		//if(avg_rtt > ((*nload)+4))
		if(avg_rtt > 10.5)
		{
			printf("\nI got a 1 with avg: %f\n",avg_rtt);
			seq = seq << 1;
			seq = seq | set;
		}
		else
		{
			printf("\nI got a 0 with avg: %f\n",avg_rtt);
			seq = seq << 1;
			seq = seq | unset;
		}
	} while(seq != 170);
	printf("\n==========Got Initial Sequence, Listening for actual data now==========\n");
	return 0;
}

/*
 * This function listens for
 * 1000 Bits or 125 Bytes of
 * Covert Data. Once it has
 * collected that much data
 * it will return to caller
 */

 /*
  * Endianness might become a problem here.
	* But since I am storing bytes at different
	* indices of an array, I doubt that it should
	* be a problem.
	*/

int covert_data(uint8_t *data,float *nload)
{
	int bitCount =0,byteCount=0;
	float avg_rtt;
	FILE *fp;
	uint8_t set=1,unset=0,endcode=0;
	while(byteCount < 125)
	{
		bitCount =0;
		while(bitCount < 8)
		{
			fp = popen("hping3 -1 -c 1000 -i u2000 10.254.0.1","r");
			if(fp == NULL)
			{
				//printf("Could not run hping2 command please check your binaries");
				return -1;
			}
			fscanf(fp,"%f",&avg_rtt);
			//if(avg_rtt > ((*nload)+4))
			if(avg_rtt > 10.5)
			{
				printf("\nI got a 1 avg: %f\n",avg_rtt);
				data[byteCount] = data[byteCount] << 1;
				data[byteCount] = data[byteCount] | set;
			}
			else
			{
				printf("\nI got a 0 avg: %f\n",avg_rtt);
				data[byteCount] = data[byteCount] << 1;
				data[byteCount] = data[byteCount] | unset;
			}
			bitCount++;
		}
		if(data[byteCount]==endcode)
		{
			printf("\nMessage Ended\n");
			return 0;
		}
		printf("\ndata so far : %c\n",data[byteCount]);
		byteCount++;
	}
	return 0;
}
