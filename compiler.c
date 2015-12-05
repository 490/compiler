#include "compiler.h"
//=========================================================
void error(int n)
{
	errorNum++;
	printf("errorNo.%d @lineNo.%d:%s\n",n,lineNum,err[n]);
}
void getCh()
{
	ch=getchar();
}
void getSym()
{
	int i=0,j=0,k=0,kk=30,first=0,last=norw-1;
	//memset(id,0,MAX);//防止前一次读的单词比本次的长，在本次id后面遗留有上个单词的后面部分
	memset(temp,0,MAX);
	while(ch==' '||ch=='\t'||ch=='\n'||ch=='\r')
	{
		if(ch=='\n')
		{
			lineNum++;
			//printf("\n");
		}
		getCh();
	}
	if( isalpha(ch) )
	{
		k=0;
		do{
			if(k<=nmax)
			{
				temp[k]=ch;
				k++;
			}
			else
				error(31);//不支持过长单词
			getCh();//得到一个新的ch
		}while(isalpha(ch)||isdigit(ch));
		strcpy(id,temp);
		id[k]='\0';
		do{
			k=(first+last)/2;		
			if(strcmp(id,word[k])<=0)
				last=k-1;
			if(strcmp(id,word[k])>=0)
				first=k+1;
		}while(first<=last);
		if(first-1>last)
			sym=wsym[k];
		else
			sym=identsym;
	}
	else if( isdigit(ch) )
	{
		k=0;
		numValue=0;
		sym=numbersym;
		do{
			numValue=10*numValue+(ch-'0');
			k++;
			getCh();
		}while(isdigit(ch));
		if(k-1>nmax)
			error(30);
		//sprintf(id,"%d",numValue);//把数字写进id数组
		if(ch=='.')
		{
			k=0;
			sym=realsym;
			realValue=(float)numValue;
			float realTemp=1.0;
			getCh();
			if(isdigit(ch))
			{
				do{
					realValue=realValue+(ch-'0')/(realTemp*10);
					realTemp=realTemp*10;
					k++;
					getCh();
				}while(isdigit(ch));
				if(k-1>nmax)
					error(29);
				//sprintf(id,"%g",realValue);//实数写进id
				//printf("getsym->realValue: %g\n",realValue );
			}
			else
				error(28);//小数点后跟的不是数字
		}
	}
	else if(ch==':')
	{
		getCh();
		if(ch=='=')
		{
			sym=becomessym;
			//strcpy(id,":=");
			getCh();
		}
		else
		{
			sym=colonsym;
			//strcpy(id,":");
		}
	}
	else if(ch=='<')
	{
		getCh();
		if(ch=='=')
		{
			sym=leqsym;
			//strcpy(id,"<=");
			getCh();
		}
		else if(ch=='>')
		{
			sym=neqsym;
			//strcpy(id,"<>");
			getCh();
		}
		else
		{
			sym=lsssym;
			//strcpy(id,"<");
		}
	}
	else if(ch=='>')
	{
		getCh();
		if(ch=='=')
		{
			sym=geqsym;
			//strcpy(id,">=");
			getCh();
		}
		else
		{
			sym=gtrsym;
			//strcpy(id,">");
		}
	}
	else if(ch=='=')
	{
		sym=eqlsym;
		//strcpy(id,"=");
		getCh();
	}
	else if(ch=='\'')
	{
		sym=charsym;
		getCh();
		if(isalpha(ch)||isdigit(ch))
		{		
			charValue=ch;
			getCh();
			if(ch=='\'')//是单个字符
			{
				cvalue=charValue;
				getCh();
			}
			else
			{
				error(27);
				do{
					getCh();
				}while(ch!=','||ch!=';'||ch!='.'||ch!='\0'||ch!='\n'||ch!='\t'||ch!=' ');
			}
		}
		else
		{
			sym=nulsym;
			//strcpy(id,"\'");
		}
	}
	else if(ch=='\"')
	{
		sym=stringsym;
		int strln=0;
		do{
			getCh();
			if(strln>MAX)
			{
				error(32);
				break;
			}
			else if(ch!='\"')
				stringValue[strln++]=ch;
		}while(ch!='\"');//如果永远没遇到右边"?
		stringValue[strln]='\0';
		//printf("string::::%s\n",stringValue );
		//strcpy(id,stringValue);
		if(ch!='\"')
			error(33);//没遇到”，长度超过MAX跳出的
		else 
			getCh();
	}
	else if(ch=='.')
	{
		sym=periodsym;
		//strcpy(id,".");
		getCh();
	}
	else 
	{
		sym=ssym[ch];//1、直接这步？2、下标？
		//id[0]=ch;
		getCh();
	}
}
void test(int s1[],int s2[],int n)
{
	//printf("进入test（）\n");
	//printf(" %s %d \n",wordSym[sym],n);
	if(s1[sym]==0)
	{
		//printf(" %s %d \n",wordSym[sym],n);
		error(n);
		//printf("%d\n",s1[nulsym] );
		while(s1[sym]==0 && s2[sym]==0)//既不在后继集 也不在停止集
		{
			getSym();
		//	printf("sym %s\n", wordSym[sym]);
		}	
	}
	//printf("离开test（）\n");
}
void enter(enum typeDecl k,int lev,int *dx)//*
{	
	//k=类型，0 常量，1 变量，2 过程，3 函数
	//printf("进入enter：");
	//printf("k=%d,lev=%d,tx=%d,dx=%d,id=%s,value=%d\n",k,lev,tableIndex,*dataAllocIndex,id ,numValue);
	tableIndex++;
	strcpy(table[tableIndex].name,id);
	//printf("id=%s  ",table[tableIndex].name );
	table[tableIndex].kindDecl=k;
	if(k==constDecl)
	{
		if(numValue>MAX||realValue>MAX)
		{
			error(30);
			numValue=0;
			realValue=0;
		}
		else
		{
			if(sym==charsym)//常量是字符
			{
				table[tableIndex].value=cvalue;
			}
			else
			{
				table[tableIndex].value=(realValue==0)?numValue:realValue;
				///printf("=====%d====\n",numValue );
				//printf("value=%g\n",table[tableIndex].value );
			}
		}
	}
	else if(k==varDecl)
	{
		//printf("=%s %d =\n",id,*dx );
		table[tableIndex].level=lev;
		table[tableIndex].adr=*dx;
		(*dx)++;
	}
	else if(k==procedureDecl)
	{
		table[tableIndex].level=lev;
		//printf("过程 操作。。。。。。。。。。。。。。。。。。。。。。?\n");
		strcpy(table[tableIndex].name,id);
		table[tableIndex].adr=*dx;
		table[tableIndex].paraIndex=paraIndex;
		(*dx)++;
		//printf("%d %d\n", tableIndex,table[19].adr);
		
	}
	else if(k==functionDecl)
	{
		table[tableIndex].level=lev;
		//printf("函数 操作。。。?\n");
		strcpy(table[tableIndex].name,id);
		table[tableIndex].adr=*dx;
		table[tableIndex].paraIndex=paraIndex;
		(*dx)++;
	}
	else if(k==addressDecl)
	{
		table[tableIndex].level=lev;
		table[tableIndex].adr=*dx;
		(*dx)++;
	}
	//printf("离开enter------------------------------\n");
}
void recordVarKind(int tx_temp,int count,int *dx)//登录一种类型
{
	//printf("------进入变量类型记录()-------\n");
	enum typeVar kindVar_temp;
	int isArray=0;
	//调用时，getsym取得:后面单词，即类型,<基本类型>|array'['<无符号整数>']' of <基本类型>
	if(sym==integersym)
		kindVar_temp=integerVar;
	else if(sym==realsym)
		kindVar_temp=realVar;
	else if(sym==charsym)
		kindVar_temp=charVar;
	else if(sym==arraysym)//array'['<无符号整数>']' of <基本类型>
	{
		getSym();
		isArray=1;//记录 是数组
		if(sym==lbracketsym)
			getSym();
		else
			error(23);
		if(sym==numbersym)
			getSym();
		else
			error(32);
		if(sym==rbracketsym)
			getSym();
		else
			error(24);
		if(sym==ofsym)
			getSym();
		else
			error(33);
		//-------------------此后应是array的类型
		if(sym==integersym)
			kindVar_temp=integerVar;
		else if(sym==realsym)
			kindVar_temp=realVar;
		else if(sym==charsym)
			kindVar_temp=charVar;
	}
	else
		error(34);
	for(int i=0;i<count;i++)//遍历同一类型的 变量
	{
		tx_temp++;//tx_temp是刚进varDeclaration时的符号表指针,++后存的第一个变量位置		
		table[tx_temp].kindVar=kindVar_temp;
		//table[tx_temp].adr=*dx;	普通变量的dx的登录在varDeclaration里进行完	
		if(isArray)
		{
			table[tx_temp].kindDecl=arrayDecl;
			table[tx_temp].adr=*dx;
			*dx+=numValue;//下一个地址 要给数组让出空间
		}
		//(*dx)++;//数据放在S中下一个位置
	}
	getSym();
	//printf("------离开变量类型记录()-------\n");
}
int position(char id[])
{
	int i=0;
	strcpy(table[0].name,id);
	i=tableIndex;
	while(strcmp(table[i].name,id)!=0)
		i--;
	//printf("position()----> %s %d %d\n",id,tableIndex,i );
	return i;
}
void constDeclaration(int lev,int *dx)//*
{	
	//<标识符>=<常量>，<常量>:=[+|-](<无符号整数>|<无符号实数>)|<字符>	
	if(sym==identsym)
	{
		getSym();
		if(sym==becomessym||sym==eqlsym)
		{
			if(sym==becomessym)
				error(1);//文法里常量的说明部分应是 标识符=常量 不是赋值
			getSym();
			if(sym==minussym)
			{
				getSym();
				if(sym==numbersym||sym==realsym)//
				{
					if(sym==numbersym)
						numValue*=-1;
					else
						realValue*=-1;
					enter(constDecl,lev,0);//登录符号表，类型是常量
					getSym();
				}		
				else
					error(2);
			}
			else if(sym==numbersym||sym==realsym||sym==charsym)//
			{
				enter(constDecl,lev,0);//登录符号表，类型是常量
				getSym();
			}		
			else
				error(2);
		}
		else
			error(3);
	}
	else
		error(4);
}
void varDeclaration(int lev,int *dx)//*
{
	//<标识符>:<类型>，<类型>:=<>
	//printf("%s  %d\n",id ,*dx);
	if(sym==identsym)
	{
		enter(varDecl,lev,dx);//变量
		getSym();
	}
	else
		error(4);
}
void parameter(int tableIndex,int lev,int *dx)//*
{
	// [var] <标识符>{, <标识符>}: <基本类型>{; <形式参数表>}
	//printf("开始分析<形式参数表>\n");
	int count=0;
	paraNum=0;
	varParaNum=0;
	enum paraKind pk=paraValue;
	int i=0;
	while(sym==identsym||sym==varsym)
	{
		if(sym==varsym)//如果是传地址
		{
			pk=paraAddr;
			getSym();
		}
		int tx_temp=tableIndex;
		*dx=3;//如果属于下一层，dx应从3重新开始
		//varDeclaration(lev+1,dx);//如果不是var，则该变量属于下一层
		if(pk==paraValue)
			varDeclaration(lev+1,dx);//预读，此时px也登录了
		else
		{
			enter(addressDecl,lev+1,dx);//没预读，此时px也登录了
			getSym();
		}	
		paraTable[paraIndex++]=pk;
		count++;
		//printf("next id[%s],next sym[%s]\n",id,wordSym[sym] );
		while(sym==commasym)
		{
			getSym();
			if(pk==paraValue)
				varDeclaration(lev+1,dx);
			else
			{
				enter(addressDecl,lev+1,dx);
				getSym();
			}
			paraTable[paraIndex++]=pk;
			count++;
		}
		//printf("%s\n",wordSym[sym] );
		if(sym==colonsym)// 冒号 标识符：类型
		{
			getSym();
			recordVarKind(tx_temp,count,dx);//最后记录变量类型
		}
		else
			error(38);//形式参数表中，标识符结束后应为：
		if(sym==rparensym)
			getSym();
		else
			error(22);//漏右括号
	}
	paraNum=count;
	//printf("%d\n",paraNum );
	//printf("离开分析<形式参数表>\n");
}
void call(int fsys[],int lev,int callTx)
{
	//进入后，此时已读进第一个参数
	int callParaNum=0;
	int declParaNum=(int)table[callTx].value;
	//printf("dpn:%d\n",declParaNum );
	int px_temp=table[callTx].paraIndex;
	enum typeVar kind_temp;
	for(callParaNum=0;callParaNum<declParaNum;callParaNum++)
	{
		pkFactor=paraTable[px_temp++];
		expression(fsys,lev,&kind_temp);
		pkFactor=paraValue;
		getSym();//exp里读了一个逗号，此句读到下一个参数
	}
	//printf("%s \n",wordSym[sym] );
	if(sym!=semicolonsym && stateEnd[sym]==0)
		error(54);//@含参数的过程调用，可能调用的参数和声明的个数不符
}
void factor(int fsys[],int lev,enum typeVar *kind)
{
	//<标识符>|<标识符>'['<表达式>']'|<无符号整数>|<无符号实数>|'('<表达式>')'|<函数调用语句>
	//printf("-------进入<因子>分析\n");
	int i=0;
	test(factorBegin,fsys,24);
	while(factorBegin[sym])//sym在因子合法开始集里
	{
		if(sym==identsym)
		{
			i=position(id);
			//printf("%s %s\n",id,wordSym[sym] );
			if(i==0)
			{
				error(11);//标识符不在符号表中
				getSym();
			}	
			else
			{
				*kind=table[i].kindVar;
				if(table[i].kindDecl==constDecl)//判断类型生成pcode指令，送入目标程序区
				{
					//printf("%g\n",table[i].value);
					gen(lit,0,table[i].value);
					getSym();
				}	
				else if(table[i].kindDecl==varDecl)
				{
					//printf("id:%s\n",id );
					//printf("factor->varDecl:lev:%d level:%d\n",lev,table[i].level);
					//printf("--%s--%s %d\n",varType[table[tableIndex].kindVar],id,tableIndex);
					gen(lod,lev-table[i].level,table[i].adr);
					getSym();
				}
				else if(table[i].kindDecl==arrayDecl)//因子是数组
				{
					getSym();
					if(sym==lbracketsym)
						getSym();
					else
						error(40);//应为左括号
					fsys[rbracketsym]=1;
					expression(fsys,lev,kind);
					fsys[rbracketsym]=0;
					gen(lit,0,table[i].adr);
					gen(opr,0,2);
					gen(loa,lev-table[i].level,0);
					if(sym!=rbracketsym)
						error(26);
					getSym();
				}
				else if(table[i].kindDecl==addressDecl)//是形参表var变量
				{
					//printf("factor->address   ");
					//printf("%s %d\n",table[i].name,pkFactor );
					if(pkFactor==paraAddr)
						gen(ladr,lev-table[i].level,table[i].adr);
					else
						gen(lod,lev-table[i].level,table[i].adr);
					getSym();
				}
				else if(table[i].kindDecl==procedureDecl)//过程 
				{
					error(21);		
					getSym();
				}	
				else if(table[i].kindDecl==functionDecl)//函数 <标识符>:=<函数调用语句>
				{
					//printf("函数------------\n");
					getSym();
					if(paraTable[table[i].paraIndex]==1)
					{
						if(sym==lparensym)
						{
							getSym();
							if(sym!=rparensym)
							{
								fsys[commasym]=1;
								fsys[rparensym]=1;
								call(fsys,lev,i);
								fsys[commasym]=0;
								fsys[rparensym]=0;	
								gen(cala,lev-table[i].level,table[i].value);//根据实参给形参赋值
							}
							else
							{
								getSym();
								gen(cala,lev-table[i].level,table[i].value);
							}					
							//在statement中，已经计算了返回值并存到名字处，此句从函数名处把其返回值取到栈顶
							//gen(lod,lev-table[i].level,table[i].adr);
							gen(lod,lev-table[i].level,0);
						}
						else
							error(40);//应为左括号
					}
					else
					{
						if(sym==lparensym)
						{
							getSym();
							if(sym!=rparensym)
							{
								gen(Int,0,(int)table[i].value);
								fsys[commasym]=1;
								fsys[rparensym]=1;
								call(fsys,lev,i);
								fsys[commasym]=0;
								fsys[rparensym]=0;	
								gen(calv,lev-table[i].level,table[i].value);//根据实参给形参赋值
							}
							else
							{
								getSym();
								gen(calv,lev-table[i].level,table[i].value);
							}					
							//在statement中，已经计算了返回值并存到名字处，此句从函数名处把其返回值取到栈顶
							gen(lod,lev-table[i].level,table[i].adr);
							//gen(lod,lev-table[i].level,0);
						}
						else
							error(40);//应为左括号
					}
				}	
			}
			//printf("%s\n",wordSym[sym] );
		}
		else if(sym==numbersym)
		{
			*kind=integerVar;
			if(numValue>MAX)
			{
				error(30);
				numValue=0;
			}
			else
			{
				//printf("factor.number  %d\n",numValue);
				gen(lit,0,numValue);
				getSym();
			}
		}
		else if(sym==realsym)
		{
			//printf("factor->realValue\n");
			*kind=realVar;
			if(realValue>MAX)
			{
				error(30);
				realValue=0;
			}
			else
			{
				//printf("gen lit 0 realvalue________\n");
				gen(lit,0,realValue);
				getSym();
			}
		}
		else if(sym==lparensym)
		{
			getSym();
			fsys[rparensym]=1;
			expression(fsys,lev,kind);//rparen+fsys
			fsys[rparensym]=0;
			if(sym==rparensym)
				getSym();
			else
				error(22);//缺右括号
		}

	}
	//printf("-------离开<因子>分析\n");
}
void term(int fsys[],int lev,enum typeVar *kind)
{
	//printf("-----进入<项>分析\n");
	enum symbol mulOp;
	enum typeVar kind_temp;
	fsys[timessym]=1;
	fsys[slashsym]=1;
	factor(fsys,lev,kind);
	while(sym==timessym||sym==slashsym)
	{
		mulOp=sym;
		getSym();
		factor(fsys,lev,&kind_temp);
		typeVarConvert(kind,&kind_temp);
		if(mulOp==timessym)
		{
			//printf("gen opr 0 4\n");
			gen(opr,0,4);			
		}
		else
		{
			//printf("gen opr 0 5\n");
			gen(opr,0,5);
		}
	}
	fsys[timessym]=0;
	fsys[slashsym]=0;
	//printf("-----离开<项>分析\n");
}
void expression(int fsys[],int lev,enum typeVar *kind)
{
	//<表达式>::=  [+|-]<项>{<加法运算符><项>}
	//printf("---进入<表达式>分析\n");
	enum symbol addOp;
	enum typeVar kind_temp;
	fsys[plussym]=1;
	fsys[minussym]=1;
	if(sym==plussym||sym==minussym)
	{
		addOp=sym;
		getSym();
		term(fsys,lev,kind);
		if(addOp==minussym)
		{
			//printf("gen opr 0 1\n");
			gen(opr,0,1);
		}
	}
	else
		term(fsys,lev,kind);
	while(sym==plussym||sym==minussym)
	{
		addOp=sym;
		getSym();
		term(fsys,lev,&kind_temp);
		if(addOp==plussym)
		{
			//printf("gen opr 0 2\n");
			gen(opr,0,2);
		}
		else
		{
			//printf("gen opr 0 3\n");
			gen(opr,0,3);
		}
		typeVarConvert(kind,&kind_temp);
	}
	fsys[plussym]=0;
	fsys[minussym]=0;
	//printf("---离开<表达式>分析\n");
}
void typeVarConvert(enum typeVar *k1,enum typeVar *k2)
{
	if(*k1==realVar||*k2==realVar)
		*k1=realVar;
	if(*k1==integerVar||*k2==integerVar)
		*k1=integerVar;
	else 
		*k1=charVar;
}
void condition(int fsys[],int lev)
{
	//printf("------开始<条件>分析\n");
	enum symbol relOp;
	enum typeVar kind_temp;
	fsys[eqlsym]=1;
	fsys[neqsym]=1;
	fsys[lsssym]=1;
	fsys[leqsym]=1;
	fsys[gtrsym]=1;
	fsys[geqsym]=1;
	expression(fsys,lev,&kind_temp);
	fsys[eqlsym]=0;
	fsys[neqsym]=0;
	fsys[lsssym]=0;
	fsys[leqsym]=0;
	fsys[gtrsym]=0;
	fsys[geqsym]=0;
	if( sym!=eqlsym && sym!=neqsym && sym!=lsssym && 
		sym!=leqsym && sym!=gtrsym && sym!=geqsym)
		error(20);//@<条件> 应为关系运算符
	else
	{
		relOp=sym;
		getSym();
		expression(fsys,lev,&kind_temp);
		//printf("%d %d\n",relOp,lsssym );
		switch (relOp)
		{
			//printf("relOp  %d\n", relOp);
			case eqlsym: gen(opr,0,7);break;
			case neqsym: gen(opr,0,8);break;
			case lsssym: gen(opr,0,9);break;
			case geqsym: gen(opr,0,10);break;
			case gtrsym: gen(opr,0,11);break;
			case leqsym: gen(opr,0,12);break;
			default:break;
		}
	}
	//printf("------结束<条件>分析\n");
}
void statement(int fsys[],int lev)
{
	//<赋值语句>|<条件语句>|<repeat循环语句>|<过程调用语句>
	//|<复合语句>|<读语句>|<写语句>|<for循环语句>|空
	//printf("进入<语句>分析\n");
	int i=0,codeIndex_1=0,codeIndex_2=0;
	if(sym==identsym)//赋值语句、过程调用语句
	{
		//printf("-开始分析<语句>\n");
		//printf("%s\n",id );
		enum typeVar kind_temp;
		i=position(id);
		if(i==0)//没有找到
			error(11);
		else if(table[i].kindDecl==constDecl)// 不能向常量 赋值
		{
			error(12);//不能向常量赋值
			i=0;
		}
		else if(table[i].kindDecl==varDecl)// 如果是变量
		{
			getSym();
			if(sym==becomessym)
				getSym();
			else
				error(13);//应为：=
			expression(fsys,lev,&kind_temp);
			gen(sto,lev-table[i].level,table[i].adr);
		}
		else if(table[i].kindDecl==addressDecl)
		{
			//printf("statement.ident.kindDecl==addressDecl\n");
			getSym();
			if(sym==becomessym)
				getSym();
			else
				error(13);
			//printf(">>>>>>>>>>>>>>>>>>>\n");
			expression(fsys,lev,&kind_temp);
			//printf("<<<<<<<<<<<<<<<<<<<\n");
			gen(sadr,lev-table[i].level,table[i].adr);
		}
		else if(table[i].kindDecl==arrayDecl)//=左边是数组
		{
			//printf("in <数组>\n");
			getSym();// 读取 [
			if(sym==lbracketsym)
				getSym();
			else
				error(40);
			fsys[rbracketsym]=1;
			expression(fsys,lev,&kind_temp);//预读一个 应是]
			fsys[rbracketsym]=0;
			gen(lit,0,table[i].adr);
			gen(opr,0,2);
			if(sym==rbracketsym)
				getSym();
			else
				error(26);
			if(sym==becomessym)//给数组元素赋值，应为：=
				getSym();
			else
				error(13);
			expression(fsys,lev,&kind_temp);//a[1]=3里的3
			gen(sta,lev-table[i].level,0);//将栈顶元素存到数组元素里
			//printf("out <数组>\n");
		}
		else if(table[i].kindDecl==procedureDecl)// 如果是过程(不能赋值，所以是过程调用语句)
		{
			//printf(" 这是<<过程调用语句>>\n");
			//fsys[endsym]=1;
			/*getSym();//(
			while(sym!=rparensym)
				getSym();
			getSym();*/
			getSym();
		//	printf("state->ident->proc.px:%d\n",paraTable[table[i].paraIndex]);
			if(paraTable[table[i].paraIndex]==1)
			{
				if(sym==lparensym)
				{
					getSym();
					if(sym!=rparensym)///说明有参数,id目前是第一个参数
					{
						gen(Int,0,(int)table[i].value);
						fsys[commasym]=1;
						fsys[rparensym]=1;
						call(fsys,lev,i);///i是调用的过程名字在符号表中的位置 //出来存的是semi
						fsys[commasym]=0;
						fsys[rparensym]=0;
						gen(cala,lev-table[i].level,table[i].value);//在cal里给参数赋值
					}
					else//有参数时，call函数已经将;读进
					{
						gen(cala,lev-table[i].level,table[i].value);
						//codeIndex_1=codeIndex;
						getSym();
					}
				}	
				else
					error(40);
			}
			else
			{
				if(sym==lparensym)
				{
					getSym();
					if(sym!=rparensym)///说明有参数,id目前是第一个参数
					{
						fsys[commasym]=1;
						fsys[rparensym]=1;
						call(fsys,lev,i);///i是调用的过程名字在符号表中的位置 //出来存的是semi
						fsys[commasym]=0;
						fsys[rparensym]=0;
						gen(calv,lev-table[i].level,table[i].value);//在cal里给参数赋值
					}
					else//有参数时，call函数已经将;读进
					{
						gen(calv,lev-table[i].level,table[i].value);
						//codeIndex_1=codeIndex;
						getSym();
					}
				}	
				else
					error(40);
			}
			int cx_temp=codeIndex;
			gen(jmp,0,0);
			gen(Int,0,(int)table[i].value+3);//为调用的子过程分配空间
			gen(jmp,0,table[i].adr);//分配完跳去执行子过程
			code[cx_temp].a=codeIndex;//执行完子过程跳回
			//printf("%d %d %d %f\n",i,lev,table[i].level,table[i].value);
		}
		else if(table[i].kindDecl==functionDecl)// 如果是函数,则<函数标识符>:=<表达式>
		{
			//printf(" 这是<<函数赋值语句>>\n");
			getSym();//:=
			if(sym==becomessym)
				getSym();
			else
				error(13);
			expression(fsys,lev,&kind_temp);
			//printf("adr %d\n",table[i].adr );
			gen(sto,lev-table[i].level,table[i].adr);//将栈顶值存入FUNC标识符所在的栈的位置
			//gen(sto,lev-table[i].level,0);
		}
		//printf("-结束分析<语句>\n");
	}
	else if(sym==ifsym)
	{
		//printf("---开始分析<if条件语句>\n");
		getSym();
		fsys[thensym]=1;
		fsys[dosym]=1;
		condition(fsys,lev);//condition会根据判断内容将栈顶置0或1
		fsys[thensym]=0;
		fsys[dosym]=0;
		if(sym==thensym)
			getSym();
		else
			error(16);
		codeIndex_1=codeIndex;
		gen(jpc,0,0);//cx+    条件跳转if(s[t]==0) p=code[i].a;		
		statement(fsys,lev);//cx+
		if(sym==elsesym)
		{
			codeIndex_2=codeIndex;
			gen(jmp,0,0);
			getSym();
			code[codeIndex_1].a=codeIndex;//应当执行else语句块，条件为假时跳到此处
			statement(fsys,lev);//else后的语句
			code[codeIndex_2].a=codeIndex;//条件为真时跳过else语句块
		}
		else
			code[codeIndex_1].a=codeIndex;
		//printf("---结束分析<if条件语句>\n");
	}
	else if(sym==beginsym)//复合语句::=begin<语句>{;<语句>}end
	{
		//printf("开始分析<复合语句>\n");
		getSym();
		fsys[semicolonsym]=1;
		fsys[endsym]=1;
		statement(fsys,lev);//begin后的第一条语句
		//printf("stateBegin=%d  sym=%s\n",stateBegin[sym], wordSym[sym]);
		while(stateBegin[sym]==1||sym==semicolonsym)//semicolon + stateBegin
		{
			//printf("-------%s %s\n",id,wordSym[sym] );
			if(sym==semicolonsym)
				getSym();
			else
			{
				error(10);
				getSym();
			}	
			statement(fsys,lev);
		}	
		//printf("after state->begin:%s\n",wordSym[sym] );
		if(sym==endsym)
			getSym();
		else
			error(17);
		fsys[semicolonsym]=0;
		fsys[endsym]=0;
		//printf("结束分析<复合语句>\n");
	}
	else if(sym==repeatsym)
	{
		//printf("开始分析<repeat>循环语句\n");
		codeIndex_1=codeIndex;
		getSym();
		fsys[semicolonsym]=1;
		fsys[untilsym]=1;
		statement(fsys,lev);	
		while(sym==semicolonsym||stateBegin[sym]==1)
		{
			if(sym==semicolonsym)
				getSym();
			else
				error(10);
			statement(fsys,lev);
		}
		fsys[semicolonsym]=0;
		fsys[untilsym]=0;
		if(sym==untilsym)//不是； 则是until
		{
			getSym();
			condition(fsys,lev);
			gen(jpc,0,codeIndex_1);
		}	
		else
			error(41);//应为until
		//printf("结束分析<repeat>循环语句\n");
	}		
	else if(sym==forsym)
	{
		//printf("开始分析<for>循环语句\n");
		int isTo=0;
		enum typeVar kind_temp;
		getSym();//标识符
		if(sym==identsym)
		{
			i=position(id);
			if(i==0)
				error(11);
			else
			{
				getSym();//:=
				if(sym==becomessym)
				{
					getSym();
					fsys[tosym]=1;
					fsys[downtosym]=1;
					expression(fsys,lev,&kind_temp);//number
					fsys[tosym]=0;
					fsys[downtosym]=0;
					if(table[i].kindDecl==varDecl)
						gen(sto,lev-table[i].level,table[i].adr);//array?var?
					else
						gen(sadr,lev-table[i].level,table[i].adr);
				}
				else
					error(13);//标识符后应是赋值号
			}
			codeIndex_1=codeIndex;
			if(sym==tosym)
			{
				isTo=1;
				getSym();
				fsys[dosym]=1;
				expression(fsys,lev,&kind_temp);//t++ 放栈顶
				fsys[dosym]=0;
				gen(lod,lev-table[i].level,table[i].adr);//取变量值 t++放栈顶
				gen(opr,0,12);//判断 栈顶<=次栈顶 i:=1顶 to 5次顶
				codeIndex_2=codeIndex;
				gen(jpc,0,0); //大于 跳出 栈顶==0 跳到code.a
			}
			else if(sym==downtosym)
			{
				getSym();
				fsys[dosym]=1;
				expression(fsys,lev,&kind_temp);//t++
				fsys[dosym]=0;
				gen(lod,lev-table[i].level,table[i].adr);//t++
				gen(opr,0,10);//判断 栈顶>=次栈顶 i:=5顶 to 3次顶
				codeIndex_2=codeIndex;
				gen(jpc,0,0);
			}
			if(sym==dosym)
			{
				getSym();
				statement(fsys,lev);//expression:t++  sto:t++
				gen(lod,lev-table[i].level,table[i].adr);//t++
				gen(lit,0,1);//把常量1 t++取到栈顶
				if(isTo==1)
					gen(opr,0,2);//变量+1
				else
					gen(opr,0,3);
				gen(sto,lev-table[i].level,table[i].adr);
				gen(jmp,0,codeIndex_1);
				code[codeIndex_2].a=codeIndex;
			}
		}	
		else
			error(4);//应为标识符
		//printf("结束分析<for>循环语句\n");
	}
	else if(sym==readsym)
	{
		//printf("---开始分析<读语句>\n");
		getSym();
		if(sym==lparensym)
		{
			do
			{
				getSym();
				if(sym==identsym)
				{
					i=position(id);
					if(i==0)
						error(11);
					else if(table[i].kindDecl!=varDecl)//不是变量
					{
						error(42);//读语句只能读变量
						i=0;
					}
					else
					{
						if(table[i].kindVar==charVar)//char 
							gen(red,0,0);
						else//int real
							gen(red,0,1);
						gen(sto,lev-table[i].level,table[i].adr);
					}
				}
				else
					error(4);//(后不是标识符
				getSym();
			}while(sym==commasym);			
		}
		else
			error(40);//read后不是(
		if(sym!=rparensym)
			error(22);//处理完后遇到的不是)
		getSym();
		//if(sym==semicolonsym)
		//	getSym();
		//printf("---结束分析<读语句>\n");
	}
	else if(sym==writesym)
	{
		//printf("---开始分析<写语句>\n");
		enum typeVar kind_temp;
		getSym();
		if(sym==lparensym)
		{
			getSym();
			//printf("%s\n", wordSym[sym]);
			if(sym==stringsym)
			{
				char char_temp;
				int i=0;
				do
				{
					char_temp=stringValue[i++];
					gen(lit,0,(int)char_temp);//ascii值加载到栈顶
					gen(wrt,0,0);//输出栈顶 0:string
				}while(char_temp!='\0');
				//memset(stringValue,0,MAX);//清零
				getSym();
				if(sym==commasym)//    wrt(<字符串>,<表达式>)
				{
					getSym();
					fsys[rparensym]=1;
					fsys[commasym]=1;
					expression(fsys,lev,&kind_temp);
					fsys[rparensym]=0;
					fsys[commasym]=0;
					//printf("==%s==\n",varType[table[tableIndex].kindVar] );
					if(kind_temp==integerVar)
						gen(wrt,0,1);
					else if(kind_temp==realVar)
						gen(wrt,0,2);
					else
						gen(wrt,0,0);
				}
			}
			else// wrt(<表达式>)
			{
				fsys[rparensym]=1;
				fsys[commasym]=1;
				expression(fsys,lev,&kind_temp);
				fsys[rparensym]=0;
				fsys[rparensym]=0;
				//printf("%d\n",kind_temp );
				if(kind_temp==integerVar)
					gen(wrt,0,1);
				else if(kind_temp==realVar)
					gen(wrt,0,2);
				else
					gen(wrt,0,0);
			}
			gen(lit,0,'\n');
			gen(wrt,0,0);
			if(sym!=rparensym)
				error(22);
			getSym();
		}
		else
			error(40);//应为左括号
		//printf("---结束分析<写语句>\n");
	}
	fsys[semicolonsym]=1;
	fsys[periodsym]=1;
	fsys[untilsym]=1;
	fsys[elsesym]=1;
	fsys[endsym]=1;
	//printf("%s--\n",wordSym[sym] );
	test(fsys,fsys0,19);//数组 传值 空？
	fsys[semicolonsym]=0;
	fsys[periodsym]=0;
	fsys[untilsym]=0;
	fsys[endsym]=0;
	fsys[elsesym]=0;
	//printf("离开<语句>分析\n");
}
void block(int lev,int fsys[])
{
	/* 每一个分程序在数据栈S中都被分配一个数据空间，
	其中RA（返回地址）单元保存应返回的调用点（下一条）指令的地址，
	DL（动态链）单元保存原调用过程的数据区的起始地址（基地址），
	SL（静态链）保存它的直接外层分程序数据区的基地址，
	按层次差沿着静态链可以找到引用的外层变量在S中的位置 */
	//printf("开始分析<分程序>\n");
	int dataAllocIndex=3+paraNum;//RA DL SL
	int dataAllocIndex_0=dataAllocIndex;
	int tableIndex_0=tableIndex-paraNum;//本层开始时 符号表位置
	int codeIndex_0;//本层开始时 代码段位置
	//printf("---%d %d\n",tableIndex,paraNum);
	table[tableIndex_0].value=(paraNum==0)?varParaNum:paraNum;//分var和非var两种
	table[tableIndex_0].adr=codeIndex;
	//paraNum=0;
	//printf("进入block\n");
	gen(jmp,0,0);//
	if(lev>levelMax)
		error(43);
	//printf("%d %d\n", tableIndex,table[19].adr);
	while(declBegin[sym]==1)//处理声明部分
	{
		if(sym==constsym)
		{
			getSym();
			//printf("开始<常量说明部分>\n");
			while(sym==identsym)
			{
				constDeclaration(lev,&dataAllocIndex);
				while(sym==commasym)
				{
					getSym();
					constDeclaration(lev,&dataAllocIndex);
				}
				if(sym==semicolonsym)
					getSym();
				else
					error(44);//常量说明应以;结束
			}
			//printf("结束<常量说明部分>\n");
		}
		if(sym==varsym)
		{
			getSym();
			//	printf("id[%s],sym[%s]\n",id,wordSym[sym] );
			//printf("开始<变量说明部分>\n");
			while(sym==identsym)
			{
				int count=0;
				int tx_temp=tableIndex;
				//printf("%s %d\n",id ,dataAllocIndex);
				varDeclaration(lev,&dataAllocIndex);
				count++;
				//printf("next id[%s],next sym[%s]\n",id,wordSym[sym] );
				while(sym==commasym)
				{
					getSym();
					varDeclaration(lev,&dataAllocIndex);
					count++;
				}
				if(sym==colonsym)// 冒号 标识符：类型
				{
					getSym();
					recordVarKind(tx_temp,count,&dataAllocIndex);//最后记录变量类型
				}
				else
					error(14);
				if(sym==semicolonsym)
					getSym();
				else
					error(45);//变量说明中，每种类型应以;结束
			}
			//printf("结束<变量说明部分>\n");
		}
		while(sym==proceduresym)
		{
			//<过程首部><分程序>;  <过程首部>::=procedure<标识符>'('[<形式参数表>]')';
			//printf("开始<过程说明部分>\n");
			getSym();
			if(sym==identsym)
			{
				enter(procedureDecl,lev,&dataAllocIndex);//kind= 过程
				getSym();
			}
			else
				error(4);
			if(sym==lparensym)
			{
				getSym();
				if(sym==rparensym)
				{
					getSym();
					if(sym==semicolonsym)
						getSym();
					else
						error(46);//过程首部应以;结束
				}
				else//左括号后面遇到的不是右括号，说明有参数，进入专门处理参数的函数
				{
					//printf("-%d %d-\n",tx_beforePara,codeIndex );
					parameter(tableIndex,lev,&dataAllocIndex);
					//printf("%s\n",wordSym[sym] );
					if(sym==semicolonsym)
						getSym();
					else
						error(46);//过程首部应以;结束
				}	
			}
			else
				error(47);//过程和函数标识符后不能缺少()
			fsys[semicolonsym]=1;
			txp[tp++]=tableIndex-paraNum;
			block(lev+1,fsys);
			--tp;
			tableIndex=txp[tp];
			//fsys[semicolonsym]=0;
			//printf("------%s\n", wordSym[sym]);
			if(sym==semicolonsym)
			{
				getSym();
				test(stateBegin,fsys,6);
			}
			else
				error(48);
			//printf("结束<过程说明部分>\n");
		}
		while(sym==functionsym)
		{
			//printf("开始<函数说明部分>\n");
			int tx_temp;
			getSym();
			if(sym==identsym)
			{
				enter(functionDecl,lev,&dataAllocIndex);//kind= 过程
				tx_temp=tableIndex;
				getSym();
			}
			else
				error(4);
			if(sym==lparensym)
			{
				getSym();
				if(sym==rparensym)
				{
					getSym();
					if(sym==semicolonsym)
						getSym();
					else
						error(46);
				}
				else//左括号后面遇到的不是右括号，说明有参数，进入专门处理参数的函数
				{
					parameter(tableIndex,lev,&dataAllocIndex);
					//printf("%s\n",wordSym[sym] );
					if(sym==colonsym)
						getSym();
					else
						error(49);//函数首部中，(形参表)后应为:
					enum typeVar kindVar_temp;
					if(sym==integersym)
						kindVar_temp=integerVar;
					else if(sym==realsym)
						kindVar_temp=realVar;
					else if(sym==charsym)
						kindVar_temp=charVar;
					else
						error(50);//函数应具有 基本类型
					table[tx_temp].kindVar=kindVar_temp;
					getSym();
					if(sym==semicolonsym)
						getSym();
					else
						error(46);
				}	
			}
			else
				error(47);//函数标识符后不能缺少()
			fsys[semicolonsym]=1;
			txp[tp++]=tableIndex-paraNum;
			block(lev+1,fsys);
			--tp;
			tableIndex=txp[tp];
			//printf("------%s\n", wordSym[sym]);
			if(sym==semicolonsym)
			{
				getSym();
				test(stateBegin,fsys,6);
			}
			else
				error(48);
			//printf("结束<函数说明部分>\n");
		}
		test(stateBegin,declBegin,7);
	}
	//printf("处理完声明部分\n");
	if(sym!=beginsym)
		error(35);//处理完各类声明后，应为复合语句，begin开始
	code[table[tableIndex_0].adr].a=codeIndex;
	table[tableIndex_0].adr=codeIndex;
	codeIndex_0=codeIndex;
	//printf("gen int 0 dx\n");
	gen(Int,0,dataAllocIndex_0);
	//printf("%d\n",fsys[1] );
	fsys[semicolonsym]=1;
	fsys[endsym]=1;
	stateBegin[endsym]=0;
	//printf("开始分析<复合语句>\n");
	statement(fsys,lev);
	//printf("结束分析<复合语句>\n");
	fsys[semicolonsym]=0;
	fsys[endsym]=0;
	//printf("gen opr 0 0\n");
	gen(opr,0,0);//从分程序返回，若0层意味着结束
	fsys[semicolonsym]=1;
	fsys[periodsym]=1;
	fsys[elsesym]=1;
	test(fsys,fsys0,8);
	fsys[semicolonsym]=0;
	fsys[periodsym]=0;
	fsys[elsesym]=0;
	tablePrint();
	//printf("结束分析<分程序>\n");
}
int gen(enum func f,int l,float a)
{
	//printf("进入gen()\n");
	if(codeIndex>codeArrayLength)
	{
		error(51);//生成的目标代码太多（程序太长）
		return -1;
	}
	code[codeIndex].f=f;
	code[codeIndex].l=l;
	code[codeIndex].a=a;
	//printf("%s %d %d\n", mnemonic[code[codeIndex].f],code[codeIndex].l,code[codeIndex].a);
	codeIndex++;
	//printf("离开gen()\n");
	return 0;
}
int base(int l,int b,float s[])
{
	int b1;
	b1=b;
	while(l>0)
	{
		b1=s[b1];
		l--;
	}
	return b1;
}
void interpret()
{
	//printf("进入interpret\n");
	FILE *stacktxt=fopen("stack.txt","w");
	FILE *scanftxt=fopen("scanf.txt","r");
	time_t timeNow;
	time(&timeNow);
	int p=0;//存下一条要执行的指令的地址
	int b=1;//存当前运行的分程序数据区在数据栈s中的起始地址
	int t=0;//存栈顶单元地址
	int i=0;//当前要执行的指令
	int ss=0;//输出stacktxt用
	int num=0;//数参数个数等用
	float s[stackSize];
	s[0]=0,s[1]=0,s[2]=0;
	memset(s,0,stackSize);
	do
	{
		i=p;
		p++;
		for(ss=t;ss>=0;ss--)
		{
			fprintf(stacktxt,"x = %-6g   s[%2d] = %-10g \n",s[base(code[i].l,b,s)+(int)code[i].a],ss,s[ss] );	
		}
		//printf("jinru %s   ",mnemonic[code[i].f] );
		//printf("%d %g\n",t,s[11] );
		switch (code[i].f)
		{
			case lit: 	t++;
					 	s[t]=code[i].a;
					 	//printf("lit:%d %g\n",t,s[t] );
						break;
			case opr:	switch((int)code[i].a)
						{
							case 0: t=b-1;
							//printf("t+3 %d   p %d\n",t+3,(int)s[t+3] );
									p=s[t+3];
									b=s[t+2];
									break;
							case 1: s[t]=-s[t];
									break;
							case 2: t--;
									s[t]=s[t]+s[t+1];
									//printf("opr: s[%d] %f + s[%d] %f\n",t,s[t],t+1,s[t+1] );
									break;
							case 3: t--;
									s[t]=s[t]-s[t+1];
									break;
							case 4: t--;
									s[t]=s[t]*s[t+1];
									break;
							case 5: t--;
									s[t]=s[t]/s[t+1];
									break;
							case 6: break;
							case 7: t--;
									s[t]=(s[t+1]==s[t]);
									break;
							case 8: t--;
									s[t]=(s[t+1]!=s[t]);
									break;
							case 9:t--;
									s[t]=(s[t+1]<s[t]);
									break;
							case 10:t--;
									s[t]=(s[t+1]>=s[t]);
									break;
							case 11:t--;
									//printf("opr11 次%f 顶%f \n",s[t],s[t+1] );
									s[t]=(s[t+1]>s[t]);
									//printf("?? %f\n",s[t] );
									break;
							case 12:t--;
									s[t]=(s[t+1]<=s[t]);
									break;
							default:break;
						}
						break;
			case lod:	t++;//
			//printf("\nlod %d %d\n",base(code[i].l,b,s),(int)code[i].a );
						s[t]=s[base(code[i].l,b,s)+(int)code[i].a];
						break;
			case sto:	//printf("sto: %f  %d \n",s[t],t);
						//printf("%d %d %g\n",base(code[i].l,b,s), (int)code[i].a,s[t]);
						//t--;
						s[base(code[i].l,b,s)+(int)code[i].a]=s[t];
						t--;
						break;
			case cala:	//printf(".a %g\n",code[i].a );
						for(num=0;num<code[i].a;num++)//a是table[i].value，即参数个数
						{
							s[t+5+num]=s[t+num-1];//对过程里的变量赋值，根据参数
							s[t+4+num]=t+num-1;
							//printf("cala::%d %d %d\n",t,t+4+num ,t+num-1);
							//printf("ssss %g %g\n",s[11],s[12] );
						}						
						s[t+1]=base(code[i].l,b,s);
						s[t+2]=b;
						s[t+3]=p;
						b=t+1;
						p=code[i].a;
						break;
			case calv:	//printf("-------------calv:.a %g\n",code[i].a );
						for(num=0;num<code[i].a;num++)//a是table[i].value，即参数个数
						{
							s[t+4+num]=s[t+num-1];//对过程里的变量赋值，根据参数
							//printf("-------------calv %d %g\n",t+4+num,s[t+4+num] );
						}
						s[t+1]=base(code[i].l,b,s);
						s[t+2]=b;
						s[t+3]=p;
						b=t+1;
						p=code[i].a;
						break;
			case Int:	t=t+code[i].a;
						//printf("int ..new t=%d .a=%d\n",t,(int)code[i].a );
						break;
			case jmp:	p=code[i].a;
						break;
			case jpc:	//t--;
						if(s[t]==0)
							p=code[i].a;	
						t--;					
						break;
			case red:	printf("--input--\n");
						t++;
						if(code[i].a==0)//read可以读标识符，但变量类型只有char
						{
							char c;
							fscanf(scanftxt,"%c",&c);
							s[t]=c;
						}	
						else
							fscanf(scanftxt,"%f",&s[t]);
						break;
			case wrt:	//printf("write:  %d %g\n",t,s[t]);
						if(code[i].a==0)//char
						{
							char c=s[t];
							printf("%c",c);
						}
						else if(code[i].a==1)//int
							printf("%d",(int)s[t] );
						else if(code[i].a==2)//real
							printf("%g",s[t] );
						t--;
						break;
			case sta:	//printf("sta: ");//base(code[i].l,b,s) 
						//printf("%d %f %d\n",t,s[t],(int)s[t-1]);
						s[(int)s[t-1]]=s[t];
						//printf("%d\n",(int)s[5] );
						t-=2;
						break;
			case loa:	//printf("loa: ");
						//printf("%d %d %d\n",t,(int)s[t],(int)s[5]);
						s[t]=s[(int)s[t]];
						//printf("%d\n",(int)s[t] );
						break;
			case ladr:	t++;
						//printf("ladr:%d %d %g\n",t,(int)(s[base(code[i].l,b,s)+(int)code[i].a]),s[(int)(s[base(code[i].l,b,s)+(int)code[i].a])]);
						s[t]=s[(int)(s[base(code[i].l,b,s)+(int)code[i].a])];
						break;
			case sadr:	//t--;
						//printf("sadr:%g %d\n",s[t],(int)(s[base(code[i].l,b,s)+(int)code[i].a]));
						s[(int)s[t-2]]=s[t];
						//printf("%g %g\n",s[t-1],s[t-2]);
						t--;
						break;
			case subt:  t--;
						break;
			default:	printf("没有此操作码\n");
						break;
		}
		//printf("%d %g\n",t,s[11] );
		fprintf(stacktxt,"=====================================================\n");
		fprintf(stacktxt,"i=%d %s 操作结束后 p=%d:\n",i, mnemonic[code[i].f],p);
	}while(p!=0);
	fprintf(stacktxt,"%s",ctime(&timeNow) );
}
void tablePrint()
{
	FILE *tabletxt=fopen("table.txt","a");
	time_t timeNow;
	time(&timeNow);
	for(int i=1;i<=tableIndex;i++)
	{
		switch(table[i].kindDecl)
		{
			case constDecl: fprintf(tabletxt,"%-4d CONST  %-8s ",i,table[i].name );
							fprintf(tabletxt,"value:%-5g\n",table[i].value );
							break;
			case varDecl:	fprintf(tabletxt,"%-4d VAR    %-8s ",i,table[i].name );
							fprintf(tabletxt,"%-7s  ",varType[table[i].kindVar] );
							fprintf(tabletxt,"lev:%-1d  adr:%-4d\n",table[i].level,table[i].adr);
							break;
			case addressDecl:fprintf(tabletxt,"%-4d ADDR   %-8s ",i,table[i].name );
							fprintf(tabletxt,"%-7s  ",varType[table[i].kindVar] );
							fprintf(tabletxt,"lev:%-1d  adr:%-4d\n",table[i].level,table[i].adr);
							break;
			case arrayDecl:	fprintf(tabletxt,"%-4d ARRAY  %-8s ",i,table[i].name );
							fprintf(tabletxt,"%-7s  ",varType[table[i].kindVar] );
							fprintf(tabletxt,"lev:%-1d  adr:%-4d\n",table[i].level,table[i].adr);
							break;
			case procedureDecl: fprintf(tabletxt,"%-4d PROC   %-8s ",i,table[i].name );
								fprintf(tabletxt,"value:%-2d lev:%-1d  adr:%-4d\n",(int)table[i].value ,table[i].level,table[i].adr);
								break;
			case functionDecl:  fprintf(tabletxt,"%-4d FUNC   %-8s ",i,table[i].name );
								fprintf(tabletxt,"%-7s  ",varType[table[i].kindVar] );
								fprintf(tabletxt,"value:%-2d lev:%-1d  adr:%-4d\n",(int)table[i].value ,table[i].level,table[i].adr);
								break;
			default:break;
		}
	}
	fprintf(tabletxt,"%s",ctime(&timeNow) );
	fprintf(tabletxt,"=========================================================\n");
}
void codePrint()
{
	FILE *codetxt=fopen("code.txt","w");
	time_t timeNow;
	time(&timeNow);
	for(int i=0;i<codeIndex;i++)
	{
		fprintf(codetxt, "%-5s  %d  %g\n", mnemonic[code[i].f],code[i].l,code[i].a);
	}
	fprintf(codetxt,"%s",ctime(&timeNow) );
}
void init()
{
	lineNum=1;
	/////////////////////////////////////
	declBegin[constsym]=1;
	declBegin[varsym]=1;
	declBegin[proceduresym]=1;
	declBegin[functionsym]=1;
	
	stateBegin[identsym]=1;
	stateBegin[ifsym]=1;
	stateBegin[repeatsym]=1;
	stateBegin[beginsym]=1;
	stateBegin[forsym]=1;
	stateBegin[readsym]=1;
	stateBegin[writesym]=1;
	stateBegin[proceduresym]=1;
	stateBegin[functionsym]=1;

	factorBegin[identsym]=1;
	factorBegin[numbersym]=1;
	factorBegin[realsym]=1;
	factorBegin[lparensym]=1;
	factorBegin[functionsym]=1;
	//factorBegin[add]

	fsys[constsym]=1;
	fsys[varsym]=1;
	fsys[proceduresym]=1;
	fsys[functionsym]=1;
	fsys[beginsym]=1;
	fsys[identsym]=1;
	fsys[ifsym]=1;
	fsys[repeatsym]=1;
	fsys[forsym]=1;
	fsys[readsym]=1;
	fsys[writesym]=1;
	fsys[periodsym]=1;
	fsys[nulsym]=1;//eof？？？可能是

	stateEnd[elsesym]=1;
	stateEnd[endsym]=1;
	stateEnd[untilsym]=1;
	/////////////////////////////////////
	strcpy(word[0],"array");
	strcpy(word[1],"begin");
	strcpy(word[2],"char");
	strcpy(word[3],"const");
	strcpy(word[4],"do");
	strcpy(word[5],"downto");
	strcpy(word[6],"else");
	strcpy(word[7],"end");
	strcpy(word[8],"for");
	strcpy(word[9],"function");
	strcpy(word[10],"if");
	strcpy(word[11],"integer");
	strcpy(word[12],"of");
	strcpy(word[13],"procedure");
	strcpy(word[14],"read");
	strcpy(word[15],"real");
	strcpy(word[16],"repeat");
	strcpy(word[17],"then");
	strcpy(word[18],"to");
	strcpy(word[19],"until");
	strcpy(word[20],"var");
	strcpy(word[21],"write");
	//////////////////////////////////////////
	wsym[0]=arraysym;
	wsym[1]=beginsym;
	wsym[2]=charsym;
	wsym[3]=constsym;
	wsym[4]=dosym;
	wsym[5]=downtosym;
	wsym[6]=elsesym;
	wsym[7]=endsym;
	wsym[8]=forsym;
	wsym[9]=functionsym;
	wsym[10]=ifsym;
	wsym[11]=integersym;
	wsym[12]=ofsym;
	wsym[13]=proceduresym;
	wsym[14]=readsym;
	wsym[15]=realsym;
	wsym[16]=repeatsym;
	wsym[17]=thensym;
	wsym[18]=tosym;
	wsym[19]=untilsym;
	wsym[20]=varsym;
	wsym[21]=writesym;
	///////////////////////////////////////////
	ssym['+']=plussym;
    ssym['-']=minussym;
    ssym['*']=timessym;
    ssym['/']=slashsym;
    ssym['(']=lparensym;
    ssym[')']=rparensym;
    ssym['[']=lbracketsym;
	ssym[']']=rbracketsym;
	ssym[',']=commasym;
	ssym['.']=periodsym;
	ssym[';']=semicolonsym;
	ssym['\'']=singlequotationmarksym;
	ssym['\"']=doublequotationmarksym;
	////////////////////////////////////////
	strcpy(wordSym[0],"NUL");
	strcpy(wordSym[1],"IDENT");
	strcpy(wordSym[2],"NUMBER");
	strcpy(wordSym[3],"CONST");
	strcpy(wordSym[4],"VAR");
	strcpy(wordSym[5],"INTEGER");
	strcpy(wordSym[6],"CHAR");
	strcpy(wordSym[7],"REAL");
	strcpy(wordSym[8],"STRING");
	strcpy(wordSym[9],"ARRAY");
	strcpy(wordSym[10],"OF");
	strcpy(wordSym[11],"PLUS");
	strcpy(wordSym[12],"MINUS");
	strcpy(wordSym[13],"TIMES");
	strcpy(wordSym[14],"SLASH");
	strcpy(wordSym[15],"EQL");
	strcpy(wordSym[16],"NEQ");
	strcpy(wordSym[17],"LSS");
	strcpy(wordSym[18],"LEQ");
	strcpy(wordSym[19],"GTR");
	strcpy(wordSym[20],"GEQ");
	strcpy(wordSym[21],"LPAREN");
	strcpy(wordSym[22],"RPAREN");
	strcpy(wordSym[23],"LBRACKET");
	strcpy(wordSym[24],"RBRACKET");
	strcpy(wordSym[25],"COMMA");
	strcpy(wordSym[26],"SEMICOLON");
	strcpy(wordSym[27],"PERIOD");
	strcpy(wordSym[28],"BECOMES");
	strcpy(wordSym[29],"COLON");
	strcpy(wordSym[30],"BEGIN");
	strcpy(wordSym[31],"END");
	strcpy(wordSym[32],"IF");
	strcpy(wordSym[33],"THEN");
	strcpy(wordSym[34],"ELSE");
	strcpy(wordSym[35],"FOR");
	strcpy(wordSym[36],"TO");
	strcpy(wordSym[37],"DOWNTO");
	strcpy(wordSym[38],"DO");
	strcpy(wordSym[39],"REPEAT");
	strcpy(wordSym[40],"UNTIL");
	strcpy(wordSym[41],"FUNCTION");
	strcpy(wordSym[42],"PROCEDURE");
	strcpy(wordSym[43],"READ");
	strcpy(wordSym[44],"WRITE");
	strcpy(wordSym[45],"SQUOTATION");
	strcpy(wordSym[46],"DQUOTATION");
	////////////////////////////////////////////
	strcpy(varType[0],"integer");
	strcpy(varType[1],"real");
	strcpy(varType[2],"char");
	strcpy(varType[3],"array");
	/////////////////////////////////////////////
	//0 lit, 1 opr, 2 lod, 3 sto, 4 cal, 5 int, 6 jmp, 7 jpc, 8 red, 9 wrt
	strcpy(mnemonic[0],"LIT");
	strcpy(mnemonic[1],"OPR");
	strcpy(mnemonic[2],"LOD");
	strcpy(mnemonic[3],"STO");
	strcpy(mnemonic[4],"CALA");
	strcpy(mnemonic[5],"INT");
	strcpy(mnemonic[6],"JMP");
	strcpy(mnemonic[7],"JPC");
	strcpy(mnemonic[8],"RED");
	strcpy(mnemonic[9],"WRT");
	strcpy(mnemonic[10],"STA");
	strcpy(mnemonic[11],"LOA");
	strcpy(mnemonic[12],"LADR");
	strcpy(mnemonic[13],"SADR");
	strcpy(mnemonic[14],"CALV");
	strcpy(mnemonic[15],"SUBT");
}
//=======================================================
int main()
{
	init();
	if(freopen("input3.txt","r",stdin)!=NULL)
	{
		getCh();	
		getSym();
		block(0,fsys);
		//tablePrint();
		codePrint();	
		if(sym!=periodsym)
			error(9);
		if(errorNum==0)
			interpret();
		else
			printf("\n【请改正所有错误后再运行】\n\n");
	}
	else
		error(52);
	return 0;
}