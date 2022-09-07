
/* 本文件内容为程序相关定义以及函数声明 */

#ifndef _DEF_H_
#define _DEF_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INIT_SIZE 20         //初始化大小
#define INCREASEMENT 10      //每次增加的大小

/* 程序相关定义 */

typedef enum Bool { FALSE, TRUE } Bool;      //自定义布尔类型

typedef enum Token_kind         //各类单词种类的枚举类型
/*
    关键字：int、float、double、char、long、void、if、else、while、for、return、break、continue、define、include、const。
    类型和标识符：数组、整形数组、浮点型数组、字符串、字符串常量、错误符号、标识符、整形常量、浮点型常量、长整形常量、双精度浮点常量。
    运算符：==、!=、>、<、+、-、*、/、=、(、)、&&、||、!、#、[、]、{、}、;、，、%、++、--、’、”、<，>。
    注释：//、块注释符1、快注释符2、注释文本
*/
{
    INT, FLOAT, DOUBLE, CHAR, LONG, VOID, IF, ELSE, WHILE, FOR, RETURN,
    BREAK, CONTINUE, DEFINE, INCLUDE, CONST,                              //部分关键字
    ARRAY, INT_ARRAY, FLOAT_ARRAY, STRING, STRING_CONST,                  //数组类型
    ERROR_TOKEN, IDENT, INT_CONST, FLOAT_CONST, LONG_CONST, DOUBLE_CONST, //标识符和各类型常量
    EQ, UEQ, LH, RH, PLUS, SUB, MUL, DIV, ASSIGN, LP, RP,                 //各种符号
    AND, OR, NOT, WELL, LSB, RSB, LCB, RCB, SEMI, COMMA, MOD,
    PLUSPLUS, SUBSUB, SINGLE_QUOTE, DOUBLE_QUOTE, LAB, RAB,
    NOTE1, NOTE2, NOTE3, NOTETEXT
}Token_kind;

typedef enum Node_type          //结点类型
/*
   程序、外部定义序列、外部变量定义、外部变量序列、函数定义、形参序列、
   复合语句、局部变量定义序列、局部变量定义、局部变量序列、实参序列、
   语句序列、if子句、else子句、循环体、for初始语句、for更新语句、返回值、
   表达式、函数调用、函数、形参、外部变量、局部变量、实参、类型、变量、
   操作符、ifelse语句、赋值、while语句、for语句、if语句、条件、return语句、break语句、
   continue语句、include语句、define语句、文件名、宏名、宏名替换词、注释内容、注释
*/
{
    _PROGRAMMAR_, _EXTDEFLIST_, _EXTVARDEF_, _EXTVARLIST_, _FUNCDEF_, _FORMALPARLIST_,
    _COMPOUNDSTATEMENT_, _LOCVARDEFLIST_, _LOCVARDEF_, _LOCVARLIST_, _ACTUALPARLIST_,
    _STATEMENTLIST_, IFCLAUSE, ELSECLAUSE, LOOPBODY, _FOR_ONE_, _FOR_TWO_, _RETURNVAR_,
    _EXPRESSION_, _FUNCCALL_, FUNC, FORPARA, EXTVAR, LOCVAR, ACTPARA, TYPE, VAR,
    OPERATOR, IFELSE, _ASSIGN_, _WHILE_, _FOR_, _IF_, CONDITION, _RETURN_, _BREAK_,
    _CONTINUE_, _INCLUDE_, _DEFINE_, _FILENAME_, _DEFINE_ONE_, _DEFINE_TWO_, NOTE, _NOTE_
}Node_type;

struct Func
{
    char** funclist;        //函数名列表
    int num, size;          //数量和大小
} func = { NULL,0,0 };      //全局变量，保存源文件出现的函数

struct Note
{
    char* data;        //注释内容
    int row, type;     //注释出现的行、注释类型 
};      //注释结构体

struct NoteList
{
    struct Note* notelist;       //注释列表
    int num, size;               //数量和大小
}notes = { NULL,0,0 };           //全局变量，保存源文件出现的注释

typedef struct  ASTNode       //抽象语法树结点类型定义
{
    int tag;  //结点数据类型标记
    union     //结点数据
    {
        char ele_name[50];   //语法成分
        char var_name[50];   //变量名
        char func_name[50];  //函数名
        int op;              //运算符
        int var_type;        //变量类型
        struct Note note;    //注释
    }data;
    struct ASTNode* child, * sibling;     //孩子兄弟表示法
}ASTNode, * ASTree;

typedef struct Stack
{
    ASTree* ptr;        //存储元素
    int top, size;      //栈顶、栈容量
}Stack;                 //自定义栈

//全局变量
FILE* fin, * fout;                                 //指向读入文件和输出文件的指针
ASTree Root = NULL;                                //语法树的根结点
Bool error = FALSE;                                //错误标记
char sourcefile[100];                              //源文件路径
char* token_text = NULL, * token_text0 = NULL;     //当前和前一单词的字面值
int token_size = 0;                                //当前token_text的大小
int row = 1, col = 1;                              //记录当前读取行列号
int row0 = 1;                                      //格式化文件行数
int type, type0, type1 = -1;                       //暂时记录某一单词

const char* keyword[] =
{
    "int","float","double","char","long","void","if","else",
    "while", "for","return","break","continue","define",
    "include","const",NULL
};              //根据枚举顺序初始化关键字查找表    

/* 函数声明 */

//辅助函数
Bool IsType(int t);                           //判断是否是类型关键字
Bool IsOperater(int t);                       //判断是否是运算符
Bool IsFunc(char* s);                         //判断是否是函数名
Bool Pop(Stack* p, ASTree* t);                //出栈
ASTree New();                                 //创建新结点并初始化
ASTree Gettop(const Stack s);                 //获取栈顶元素
int Find_keyword();                           //查找关键字
void InitStack(Stack* p);                     //初始化栈
void Push(Stack* p, ASTree q);                //入栈
void Add_char(const char c);                  //拼接字符
void Copy();                                  //复制单词
void Info(int type);                          //输出token_text信息
void Traverse(ASTree p, int dep);             //遍历语法树
char Precede(int c1, int c2);                 //求运算符之间的优先级
void traverse(ASTree root);                   //遍历表达式树
void ftraverse(ASTree root);                  //遍历表达式树输入到文件中
void Addfunc(char* s);                        //添加一个函数
void Addnote(char* s, int r, int t);          //添加一个注释
void Warning(const char* s);                  //报错
void End();                                   //程序结束释放空间并关闭文件
void Destroy(ASTree root);                    //销毁语法树


//词法分析
int Get_token();                              //词法分析

//语法分析
ASTree Program();                             //程序
ASTree FileInclude();                         //文件包含
ASTree MacroDefine();                         //宏定义
ASTree ExtDefList();                          //外部定义序列
ASTree ExtVarDef();                           //外部变量定义
ASTree ExtVarList();                          //外部变量序列
ASTree ExtVar();                              //外部变量
ASTree ExtDef();                              //外部定义
ASTree LocVarDefList();                       //局部变量定义序列
ASTree LocVarDef();                           //局部变量定义
ASTree LocVarList();                          //局部变量序列
ASTree LocVar();                              //局部变量
ASTree FuncDef();                             //函数定义
ASTree FormalParList();                       //形参序列
ASTree FormalPara();                          //形参
ASTree ActualParList();                       //实参序列
ASTree CompoundStatement();                   //复合语句
ASTree StatementList();                       //语句序列
ASTree Statement();                           //语句
ASTree Expression(Token_kind endsym);         //表达式

//格式化源文件
void Format(ASTree root, int dep);            //格式化源文件
void Writenote();                             //将注释写入格式化文件

#endif