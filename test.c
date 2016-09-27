#include<stdio.h>
#include<stdlib.h>
int main()
{
	float c=0;
	int i;
	//system("hping -c 10 -i u1000 192.168.169.2 > out.txt");
	FILE *fp;
	for(i=0;i<6000;i++)
	{
		fp = popen("hping3 -1 -c 1000 -i u10000 10.254.0.1","r");
		if(fp == NULL)
		{
			printf("Failed to run command");
			exit(1);
		}
		fscanf(fp,"%f",&c);
		printf("\n%f",c);
		pclose(fp);
	}
	return 0;
}
