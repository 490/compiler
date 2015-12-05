#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char *argv[])
{	
	if(argc<3)
	{
		printf("请输入三个参数\n");
		return 0;
	}
	printf("参数个数:%d\n",argc-1 );
	printf("第一个字符串:%s  第二个字符串:%s 第三个字符串:%s\n",argv[1],argv[2],argv[3]);
	int a=strlen(argv[1]);//长度
	int b=strlen(argv[2]);
	int c=strlen(argv[3]);
	int n=0;//次数
	int i=0,j=0,k=0,m=0;
	for(i=0;i<a;i++)
	{

		for(j=i,k=0;argv[1][j]==argv[2][k];j++,k++)
			;
		if(j==a+1)
			k--;
		if(k==b)//第二个字符串扫完
		{
			j=i;
			while(m!=c)
				argv[1][j++]=argv[3][m++];
			m=0;
		}
	}
	printf("替换后的字符串一：%s\n",argv[1] );
	return 0;
}