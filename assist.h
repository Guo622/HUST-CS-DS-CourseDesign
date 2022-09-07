
/* 本文件内容为程序所需辅助函数的定义 */

#ifndef _ASSIST_H_
#define _ASSIST_H_
#include "def.h"

void Add_char(const char c)
//拼接字符
{
    if (token_text == NULL)                                  //token_text为空则进行初始化
    {
        token_text = (char*)malloc(sizeof(char) * INIT_SIZE);
        token_size = INIT_SIZE;
        token_text[0] = c;                                   //添加字符
        token_text[1] = '\0';                                //形成字符串
        return;
    }
    int len;
    if ((len = strlen(token_text)) == token_size - 1)       //token_text满了
    {
        token_text = (char*)realloc(token_text,             //重新分配空间
            sizeof(char) * (INCREASEMENT + token_size));
        token_size += INCREASEMENT;
    }
    token_text[len] = c;
    token_text[len + 1] = '\0';
}

void Addfunc(char* s)
//添加一个函数
{
    if (func.funclist == NULL)           //函数列表为空则进行初始化
    {
        func.funclist = (char**)malloc(sizeof(char*) * INIT_SIZE);
        func.size = INIT_SIZE;
    }
    if (func.num == func.size)      //列表满了则重新分配空间
    {
        func.funclist = (char**)realloc(func.funclist, sizeof(char*)
            * (func.size + INCREASEMENT));
        func.size += INCREASEMENT;
    }
    func.funclist[func.num] = (char*)malloc(sizeof(char)
        * (strlen(s) + 1));
    strcpy(func.funclist[func.num], s);
    func.num++;
}

void Addnote(char* s, int r, int t)
//添加一个注释
{
    if (notes.notelist == NULL)     //注释列表为空则进行初始化
    {
        notes.notelist = (struct Note*)malloc(
            sizeof(struct Note) * INIT_SIZE);
        notes.size = INIT_SIZE;
    }
    if (notes.size == notes.num)    //列表满了则重新分配空间
    {
        notes.notelist = (struct Note*)realloc(notes.notelist,
            sizeof(struct Note) * (notes.size + INCREASEMENT));
        notes.size += INCREASEMENT;
    }
    notes.notelist[notes.num].data = (char*)malloc(sizeof(char) * (strlen(s) + 1));
    notes.notelist[notes.num].row = r, notes.notelist[notes.num].type = t;
    strcpy(notes.notelist[notes.num].data, s);
    notes.num++;
}

void Copy()
//复制单词,将token_text的内容复制到token_text0
{
    if (token_text0)     free(token_text0);
    token_text0 = (char*)malloc(
        sizeof(char) * (strlen(token_text) + 1));
    strcpy(token_text0, token_text);
}

int Find_keyword()
//查找关键字
{
    int i = 0;
    while (keyword[i] && strcmp(keyword[i], token_text))
        i++;
    if (keyword[i] == NULL)    return -1;    //查找失败
    return i;                                //查找成功
}

Bool IsType(int t)
//判断是否是类型关键字
{
    if (t >= INT && t <= VOID)    return TRUE;
    return FALSE;
}

Bool IsOperater(int t)
//判断是否是运算符
{
    if (t >= EQ && t <= WELL)   return TRUE;
    return FALSE;
}

Bool IsFunc(char* s)
//判断是否是函数名
{
    for (int i = 0;i < func.num;i++)
        if (strcmp(func.funclist[i], s) == 0)
            return TRUE;
    return FALSE;
}

void InitStack(Stack* p)
//初始化栈
{
    p->ptr = (ASTree*)malloc(sizeof(ASTree) * INIT_SIZE);
    p->top = 0;   p->size = INIT_SIZE;
}

ASTree Gettop(const Stack s)
//获取栈顶元素
{
    if (s.top == 0)  return NULL;
    return s.ptr[s.top - 1];
}

Bool Pop(Stack* p, ASTree* t)
//出栈
{
    if (p->top == 0)   return FALSE;
    (*t) = p->ptr[--(p->top)];
    return TRUE;
}

void Push(Stack* p, ASTree q)
//入栈
{
    if (p->top == p->size)
        p->ptr = (ASTree*)realloc(p->ptr, sizeof(ASTree) *
            (p->size + INCREASEMENT)),
        p->size += INCREASEMENT;
    p->ptr[p->top++] = q;
}

char Precede(int t1, int t2)
//求运算符之间的优先级
{
    switch (t1)
    {
    case PLUS:case SUB:
        switch (t2)
        {
        case PLUS:case SUB:case RP:case LH:case RH:
        case EQ:case UEQ:case WELL:case AND:case OR:
            return '>';
        case MUL:case DIV:case LP: return '<';
        default:return ' ';
        }
    case MUL:case DIV:
        switch (t2)
        {
        case PLUS:case SUB:case RP:case LH:case RH:
        case EQ:case UEQ:case WELL:case MUL:case DIV:
        case AND:case OR:    return '>';
        case LP: return '<';  default:return ' ';
        }
    case LP:
        switch (t2)
        {
        case PLUS:case SUB:case MUL:case DIV:case LP:
        case AND:case OR:   return '<';
        case RH:case LH:case EQ:case UEQ:case WELL:
            return '>';
        case RP: return '=';
        default:return ' ';
        }
    case RP:
        switch (t2)
        {
        case PLUS:case SUB:case DIV:case LH:case RH:
        case EQ:case UEQ:case WELL:case MUL:case LP:
        case AND:case OR:    return '>';
        default:return ' ';
        }
    case ASSIGN:
        switch (t2)
        {
        case PLUS:case SUB:case DIV:case LH:case RH:
        case EQ:case UEQ:case ASSIGN:case MUL:case LP:
        case AND:case OR:    return '<';
        case WELL:case RP:return '>';
        default:return ' ';
        }
    case LH:case RH:
        switch (t2)
        {
        case PLUS:case SUB:case MUL:case DIV:case LP:
            return '<';
        case RH:case LH:case EQ:case UEQ:case WELL:case RP:
        case AND:case OR:    return '>';
        default:return ' ';
        }
    case EQ:case UEQ:
        switch (t2)
        {
        case PLUS:case SUB:case MUL:case DIV:case LP:
        case RH:case LH:   return '<';
        case EQ:case UEQ:case WELL:case RP:
        case AND:case OR:    return '>';
        default:return ' ';
        }
    case WELL:
        switch (t2)
        {
        case PLUS:case SUB:case DIV:case LH:case RH:
        case EQ:case UEQ:case ASSIGN:case MUL:case LP:
        case AND:case OR:    return '<';
        case WELL:return '=';
        default:return ' ';
        }
    case AND:
        switch (t2)
        {
        case PLUS:case SUB:case MUL:case DIV:case LP:
        case ASSIGN:case LH:case RH:case EQ:case UEQ:
            return '<';
        case RP:case WELL:case AND:case OR:
            return '>';
        }
    case OR:
        switch (t2)
        {
        case PLUS:case SUB:case MUL:case DIV:case LP:
        case ASSIGN:case LH:case RH:case EQ:case UEQ:
        case AND:    return '<';
        case RP:case WELL:case OR:
            return '>';
        }
    default:return ' ';
    }
}

void Info(int t)
//输出token_text信息
{
    static Bool begin = FALSE;
    if (begin == FALSE)
    {
        begin = TRUE;
        printf("%-12s单词值\n\n", "单词类别");
    }
    switch (t)
    {
    case INT: printf("%-13sint\n", "关键字");break;
    case FLOAT: printf("%-13sfloat\n", "关键字");break;
    case DOUBLE: printf("%-13sdouble\n", "关键字");break;
    case CHAR: printf("%-13schar\n", "关键字");break;
    case LONG: printf("%-13slong\n", "关键字");break;
    case VOID:printf("%-13svoid\n", "关键字");break;
    case IF: printf("%-13sif\n", "关键字");break;
    case ELSE: printf("%-13selse\n", "关键字");break;
    case WHILE: printf("%-13swhile\n", "关键字");break;
    case FOR: printf("%-13sfor\n", "关键字");break;
    case RETURN: printf("%-13sreturn\n", "关键字");break;
    case BREAK: printf("%-13sbreak\n", "关键字");break;
    case CONTINUE: printf("%-13scontinue\n", "关键字");break;
    case DEFINE:printf("%-13sdefine\n", "关键字");break;
    case INCLUDE:printf("%-13sinclude\n", "关键字");break;
    case CONST:printf("%-13sconst\n", "关键字");break;
    case ARRAY: printf("%-13s%s\n", "数组", token_text);break;
    case IDENT:printf("%-13s%s\n", "标识符", token_text);break;
    case INT_CONST:printf("%-13s%s\n", "整型常量", token_text);break;
    case LONG_CONST:printf("%-13s%s\n", "长整型常量", token_text);break;
    case FLOAT_CONST:printf("%-13s%s\n", "浮点型常量", token_text);break;
    case DOUBLE_CONST:printf("%-13s%s\n", "长双精度浮点型常量", token_text);break;
    case EQ:printf("%-13s==\n", "等号");break;
    case LH:printf("%-13s>\n", "大于号");break;
    case RH:printf("%-13s<\n", "小于号");break;
    case PLUS:printf("%-13s+\n", "加号");break;
    case PLUSPLUS:printf("%-13s++\n", "自增");break;
    case SUB:printf("%-13s-\n", "减号");break;
    case SUBSUB:printf("%-13s--\n", "自减");break;
    case MUL:printf("%-13s*\n", "乘号");break;
    case DIV:printf("%-13s/\n", "除号");break;
    case MOD:printf("%-13s%%\n", "取余");break;
    case ASSIGN:printf("%-13s=\n", "赋值");break;
    case LP:printf("%-13s(\n", "左小括号");break;
    case RP:printf("%-13s)\n", "右小括号");break;
    case LSB:printf("%-13s[\n", "左中括号");break;
    case RSB:printf("%-13s]\n", "右中括号");break;
    case LCB:printf("%-13s{\n", "左大括号");break;
    case RCB:printf("%-13s}\n", "右大括号");break;
    case LAB:printf("%-13s<\n", "左尖括号");break;
    case RAB:printf("%-13s>\n", "右尖括号");break;
    case SEMI:printf("%-13s;\n", "分号");break;
    case COMMA:printf("%-13s,\n", "逗号");break;
    case WELL:printf("%-13s#\n", "井号");break;
    case AND:printf("%-13s&&\n", "与");break;
    case OR:printf("%-13s||\n", "或");break;
    case NOT:printf("%-13s!\n", "非");break;
    case SINGLE_QUOTE:printf("%-13s\'\n", "单引号");break;
    case DOUBLE_QUOTE:printf("%-13s\"\n", "双引号");break;
    case NOTE1:printf("%-13s//\n", "注释符");break;
    case NOTE2:printf("%-13s/*\n", "注释符");break;
    case NOTE3:printf("%-13s*/\n", "注释符");break;
    case NOTETEXT:printf("%-13s%s\n", "注释文本", token_text);break;
    }
}

ASTree New()
//创建新结点并初始化
{
    ASTree p = (ASTree)malloc(sizeof(ASTNode));
    p->child = p->sibling = NULL;
    return p;
}

void Traverse(ASTree p, int dep)
//遍历语法树
{
    if (p == NULL)  return;
    for (int i = 1;i < dep;i++)
        printf("      ");       //输出前置空格
    switch (p->tag)
    //根据结点类型输出相应内容
    {
    case _WHILE_:case _FOR_:case _ASSIGN_:case _IF_:case IFELSE:
    case _EXTDEFLIST_:case _EXTVARDEF_:case _EXTVARLIST_:case _FUNCDEF_:
    case _FORMALPARLIST_:case _COMPOUNDSTATEMENT_:case _LOCVARDEFLIST_:
    case _LOCVARDEF_:case _LOCVARLIST_:case _ACTUALPARLIST_:
    case _STATEMENTLIST_:case IFCLAUSE:case ELSECLAUSE:case LOOPBODY:
    case _FOR_ONE_:case _FOR_TWO_:case _RETURNVAR_:
    case _FUNCCALL_:case _PROGRAMMAR_:case _FILENAME_:case _DEFINE_ONE_:
    case _DEFINE_TWO_:case _NOTE_:  printf("%s：\n", p->data.ele_name);break;
    case CONDITION:case _EXPRESSION_:
        printf("%s：\n", p->data.ele_name);
        for (int i = 1;i < dep + 1;i++)
            printf("      ");
        traverse(p->child); putchar('\n'); return;
    case _BREAK_:case _CONTINUE_:
        printf("%s\n", p->data.ele_name);break;
    case _RETURN_:
        printf("%s：\n", p->data.ele_name);
        for (int i = 1;i < dep + 1;i++)
            printf("      ");
        printf("%s：\n", p->child->data.ele_name);
        for (int i = 1;i < dep + 2;i++)
            printf("      ");
        traverse(p->child->child);
        putchar('\n'); return;
    case _INCLUDE_:
        printf("%s：\n", p->data.ele_name);
        for (int i = 1;i < dep + 1;i++)
            printf("      ");
        printf("%s：%s\n", "文件名", p->child->data.ele_name);
        return;
    case _DEFINE_:
        printf("%s：\n", p->data.ele_name);
        for (int i = 1;i < dep + 1;i++)
            printf("      ");
        printf("%s：%s\n", "宏名", p->child->data.ele_name);
        for (int i = 1;i < dep + 1;i++)
            printf("      ");
        printf("%s：%s\n", "替换词", p->child->sibling->data.ele_name);
        return;
    case TYPE:
        printf("%s：", "类型");
        switch (p->data.var_type)
        {
        case INT:printf("%s\n", "int");break;
        case FLOAT:printf("%s\n", "float");break;
        case DOUBLE:printf("%s\n", "double");break;
        case CHAR:printf("%s\n", "char");break;
        case LONG:printf("%s\n", "long");break;
        case VOID:printf("%s\n", "void");break;
        }
        break;
    case FORPARA:
        printf("%s：", "类型");
        switch (p->child->data.var_type)
        {
        case INT:printf("%s\t", "int");break;
        case FLOAT:printf("%s\t", "float");break;
        case DOUBLE:printf("%s\n", "double");break;
        case CHAR:printf("%s\t", "char");break;
        case LONG:printf("%s\n", "long");break;
        case VOID:printf("%s\t", "void");return;
        }
        printf("%s：%s\n", "参数名", p->child->sibling->data.var_name);
        return;
    case ACTPARA:printf("%s: %s\n", "参数", p->data.var_name);break;
    case VAR:printf("%s：\n", p->data.var_name);break;
    case LOCVAR:
    case EXTVAR:
        printf("%s：%s\n", "变量", p->data.var_name);break;
    case FUNC:printf("%s：%s\n", "函数名", p->data.func_name);break;
    case OPERATOR:traverse(p);putchar('\n');return;
    case NOTE:
        if (p->data.note.type == 1)
            printf("行注释    行数：%d    内容：%s\n",
                p->data.note.row, p->data.note.data);
        else
            printf("块注释    行数：%d    内容：%s\n",
                p->data.note.row, p->data.note.data);
        break;
    }
    Traverse(p->child, dep + 1);       //遍历第一个孩子结点
    ASTree sib = NULL;
    if (p->child)
        sib = p->child->sibling;
    while (sib)     //遍历剩余孩子结点
    {
        Traverse(sib, dep + 1);
        sib = sib->sibling;
    }
}

void traverse(ASTree root)
//先序遍历表达式树
{
    if (root == NULL)  return;
    if (root->child)
    {
        if (root->data.op != ASSIGN)    //变量之间加括号,防止优先级出错
            putchar('(');
        traverse(root->child);
        if (root->data.op != ASSIGN)
            putchar(')');
    }
    if (root->tag == VAR)
        printf("%s ", root->data.var_name);
    else
    {
        switch (root->data.op)
        {
        case EQ:printf("==");break;case UEQ:printf("!=");break;
        case LH:putchar('>');break;case RH:putchar('<');break;
        case PLUS:putchar('+');break;case SUB:putchar('-');break;
        case MUL:putchar('*');break;case DIV:putchar('/');break;
        case AND:printf("&&");break;case OR:printf("||");break;
        case ASSIGN:putchar('=');break;
        }
        putchar(' ');
    }
    if (root->child)
    {
        putchar('(');
        traverse(root->child->sibling);    //遍历剩余孩子
        putchar(')');
    }
}

void ftraverse(ASTree root)
//遍历表达式树输入到文件中（实现同上）
{
    if (root == NULL)  return;
    if (root->child)
    {
        if (root->data.op != ASSIGN)
            fputc('(', fout);
        ftraverse(root->child);
        if (root->data.op != ASSIGN)
            fputc(')', fout);
    }
    if (root->tag == VAR)
        fprintf(fout, "%s ", root->data.var_name);
    else
    {
        switch (root->data.op)
        {
        case EQ:fprintf(fout, "==");break;case UEQ:fprintf(fout, "!=");break;
        case LH:fputc('>', fout);break;case RH:fputc('<', fout);break;
        case PLUS:fputc('+', fout);break;case SUB:fputc('-', fout);break;
        case MUL:fputc('*', fout);break;case DIV:fputc('/', fout);break;
        case AND:fprintf(fout, "&&");break;case OR:fprintf(fout, "||");break;
        case ASSIGN:fputc('=', fout);break;
        }
        fputc(' ', fout);
    }
    if (root->child)
    {
        fputc('(', fout);
        ftraverse(root->child->sibling);
        fputc(')', fout);
    }
}

void Warning(const char* s)
//报错，根据形参内容报出相应错误
{
    error = TRUE;
    printf("\n\nE R R O R !\t第 %d 行 第 %d 列\n%s\n\n", row, col, s);
    system("pause");
    putchar('\n');
}

void End()
//程序结束释放空间
{
    free(notes.notelist);
    for (int i = 0;i < func.num;i++)
        free(func.funclist[i]);
    if (func.num)     free(func.funclist);
    if (token_text)   free(token_text);
    if (token_text0)  free(token_text0);
    Destroy(Root);
}

void Destroy(ASTree root)
//销毁语法树
{
    if (root == NULL)  return;
    ASTree p = NULL, q = NULL;
    if (root->child)  p = root->child->sibling;
    Destroy(root->child);
    free(root);
    while (p)
    {
        q = p->sibling;
        Destroy(p);
        p = q;
    }
}

#endif