#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#define MAX 10000
#define nmax 20//最大位数
#define norw 22//保留字个数
#define levelMax 7 //最大嵌套层数
#define tableLength 300//符号表长度
#define codeArrayLength 5000//目标代码数组大小
#define symbolNum 50
#define stackSize 1000
int chPosi,lineLength,lineNum;//字符位置、行长度、行数
int paraNum;
int varParaNum;
int txp[7];//存进入过程之前符号表位置，栈
int tp;//指针
int errorNum;
int wordNum;//单词个数，p3用
int codeIndex,tableIndex;//符号表指针
float realValue;//实数数值
int numValue;//整数数值
char charValue;//字符的值
char stringValue[MAX];//字符串的值
char lineBuf[MAX];//行缓冲区
char temp[MAX];//临时存放读到的单词
char ch;
char id[MAX];
char cvalue;
char word[norw][nmax];//保留字表
char wordSym[symbolNum][nmax];//单词类型表，和symbol对应
char varType[5][10];//和typevar对应，int real char array
char mnemonic[16][6];//f助记符
enum paraKind
{
	paraValue,
	paraAddr
};
enum paraKind pkFactor;
enum paraKind paraTable[300];
int paraIndex;
enum symbol{
	nulsym,identsym,numbersym,constsym,varsym,
	integersym,charsym,realsym,stringsym,
	arraysym,ofsym,	//
	plussym, minussym,timessym,slashsym,		//加减乘除
	eqlsym,neqsym,lsssym,leqsym,gtrsym,geqsym,		//比较
	lparensym,rparensym,lbracketsym,rbracketsym,//()[]
	commasym,semicolonsym,periodsym,becomessym,colonsym,//, ; . := : 
	beginsym,endsym,				//
	ifsym,thensym,elsesym,			//if
	forsym,tosym,downtosym,dosym,	//for
	repeatsym,untilsym,				//repeat	
	functionsym,proceduresym,		//函数 过程
	readsym,writesym,				//读写
	singlequotationmarksym,doublequotationmarksym//引号
};
enum symbol wsym[norw];
enum symbol ssym[symbolNum*3];
enum symbol sym;
enum func{
	lit,opr,lod,sto,cala,Int,
	jmp,jpc,red,wrt,sta,loa,ladr,sadr,calv,subt
};
//=================================================
int declBegin[symbolNum];//声明开始符号集合
int stateBegin[symbolNum];//语句
int factorBegin[symbolNum];//因子
int stateEnd[symbolNum];
int fsys[symbolNum];//用作停止集
int fsys0[symbolNum];//一个全是空的数组
//=================================================
enum typeDecl{
	constDecl,
	varDecl,
	procedureDecl,
	functionDecl,
	arrayDecl,//因为Array只能是变量，又有int real char 三种类型，只能放这里
	addressDecl//参数中有var时用
};
enum typeVar{
	integerVar,
	realVar,
	charVar
};
struct Table
{
	char name[nmax];
	enum typeDecl kindDecl;//声明类型 0 常量，1 变量，2 过程，3 函数 
	float value;//常量的值//proc func 参数个数
	float returnValue;//func return value
	char cvalue;
	enum typeVar kindVar;//变量类型 0 int 1 real 2 char 3 array
	int level;
	int adr;
	int paraIndex;
};
struct Table table[tableLength];
struct Instruction
{
	enum func f;//0 lit, 1 opr, 2 lod, 3 sto, 4 cal, 5 int, 6 jmp, 7 jpc, 8 red, 9 wrt
	int l;
	float a;
};
struct Instruction code[codeArrayLength];
char err[60][100]={
	" ",//0
	" 应是=而不是:=",//1
	" =后应为常量",//2
	" 标识符后应为=",//3
	" 应为标识符",//4
	" 应为;",//5
	" 过程说明后的符号不正确",//6
	" 应为语句",//7
	" 程序体内语句部分后的符号不正确",//8
	" 应为 .",//9
	" 语句之间漏分号 @statement,<复合语句>",//10
	" 该标识符不在符号表中",//11
	" 不能向常量赋值 @statement",//12
	" 应为赋值运算符:=",//13
	" 应为:",//14
	" 不可调用常量或变量",//15
	" 应为then",//16
	" 应为end",//17
	" 应为do",//18
	" 语句后的符号不正确",//19
	" 应为关系运算符 @<条件>",//20
	" 表达式内不可有过程标识符",//21
	" 应为 )",//22
	" 因子后不可为此符号",//23
	" 表达式不能以此符号开始",//24
	" 应为[",//25
	" 应为]",//26
	" 应为'，字符只能一位",//27
	" 小数点后应为数字",//28
	" 小数部分太长",//29
	" 整数数太长",//30
	" 单词太长",//31
	" 字符串太长",//32
	" 缺少右\"",//33
	" 应为无符号整数",//34
	" 处理完各类声明后，应为复合语句，begin开始",//35
	" 变量声明的类型有错",//36
	" 应为begin",//37
	" 形式参数表中，标识符结束后应为:",//38
	" 应为to或downto",//39
	" 应为(",//40
	" 应为until",//41
	" 读语句只能读变量",//42
	" 嵌套层数太大",//43
	" 常量说明部分应以;结束",//44
	" 变量说明部分中，每种类型应以;结束",//45
	" 过程首部和函数首部应以;结束",//46
	" 过程标识符和函数标识符后不能缺少()",//47
	" 过程说明部分和函数说明部分应以;结束",//48
	" 函数首部中，(形参表)后应为:",//49
	" 函数应具有 基本类型",//50
	" 生成的目标代码太多（程序太长）",//51
	" 打开文件失败",//52
	" 在参数表中有var时，标识符应该是已经存在的",//53
	" @含参数的过程调用，可能调用的参数和声明的个数不符",//54
	" 形式参数表如果有var，则该变量应该已经被声明过"//55
};
//======================================================
void error(int n);
void getCh();
void getSym();
void test(int s1[],int s2[],int n);
void enter(enum typeDecl k,int lev,int *dx);
void recordVarKind(int tx_temp,int count,int *dx);
int  position(char id[]);
void constDeclaration(int lev,int *dx);
void varDeclaration(int lev,int *dx);
void listCode(int codeIndex_0);
void factor(int fsys[],int lev,enum typeVar *kind);
void term(int fsys[],int lev,enum typeVar *kind);
void expression(int fsys[],int lev,enum typeVar *kind);
void typeVarConvert(enum typeVar *k1,enum typeVar *k2);
void condition(int fsys[],int lev);
void statement(int fsys[],int lev);
void block(int lev,int fsys[]);
int  gen(enum func f,int l,float a);
void tablePrint();
void codePrint();
void parameter(int tableIndex,int lev,int *dx);//形式参数处理
int  base(int l,int b,float s[]);
void interpret();
void init();