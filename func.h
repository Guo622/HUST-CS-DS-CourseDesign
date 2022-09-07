
/* 本文件内容为程序所需主要函数的定义 */

#ifndef _FUNC_H_
#define _FUNC_H_
#include "def.h"
#include "assist.h"

int Get_token()
//词法分析
{
    if (token_text)  free(token_text);
    token_text = NULL;                      //初始化token_text为空
    int c;
    while ((c = fgetc(fin)) != EOF)
    //过滤空白符号
    {
        col++;                              //更新列数
        if (c == '\n')    col = 1, row++;   //更新列数行数
        if (!isspace(c))    break;
    }
    if (c == '/')
    {
        c = fgetc(fin); col++;
        if (c != '*' && c != '/')
        {
            ungetc(c, fin);col--;
            Info(DIV);      //不是注释，则返回除号
            return DIV;
        }
        if (c == '/')  //行注释
        {
            Info(NOTE1);
            int r0 = row;
            c = fgetc(fin);
            while (c != '\n' && c != EOF)
            {
                Add_char(c);
                c = fgetc(fin);
            }
            if (c == '\n') { row++, col = 1; }
            Addnote(token_text, r0, 1);
            Info(NOTETEXT);
            return Get_token();
        }
        else    //块注释
        {
            Info(NOTE2);
            int c0, r0 = row;
            c0 = c = fgetc(fin);col++;
            while (c != EOF)
            {

                if (c0 == '*' && c == '/')
                {
                    token_text[strlen(token_text) - 1] = '\0';
                    Addnote(token_text, r0, 2);
                    Info(NOTETEXT);
                    Info(NOTE3);
                    return Get_token();
                }
                if (c == '\n')     col = 1, row++;
                Add_char(c);
                c0 = c;
                c = fgetc(fin);
            }
            Warning("应输入'*/'！");
            return ERROR_TOKEN;
        }
    }
    if (c == '.')
    //处理小数点开头
    {
        Add_char(c);
        c = fgetc(fin);
        col++;
        if (!isdigit(c))
        {
            Warning("应输入数字！");
            return ERROR_TOKEN;
        }
        while (isdigit(c))
        {
            Add_char(c);
            c = fgetc(fin);
            col++;
        }
        if (c == 'F' || c == 'f')
        {
            Add_char(c);
            Info(FLOAT_CONST);                //返回浮点常量
            return FLOAT_CONST;
        }
        else if (c == 'L' || c == 'l')
        {
            Add_char(c);
            Info(DOUBLE_CONST);         //长双精度浮点常量
            return DOUBLE_CONST;
        }
        else
        {
            ungetc(c, fin);
            col--;
            Info(FLOAT_CONST);
            return FLOAT_CONST;
        }
    }
    if (isalpha(c) || c == '_')
    //处理字母开头
    {
        do
        {
            Add_char(c);                    //拼接token_text
            c = fgetc(fin);
            col++;
        } while (isalpha(c) || isdigit(c) || c == '_');
        if (c == '[')  //数组类型
        {
            Add_char(c);
            c = fgetc(fin);
            col++;
            while (isdigit(c))
            {
                Add_char(c);
                c = fgetc(fin);
                col++;
            }
            if (c != ']')
            {
                Warning("缺少右中括号！");
                return ERROR_TOKEN;
            }
            Add_char(c);
            Info(ARRAY);
            return ARRAY;
        }
        ungetc(c, fin);   col--;             //退回多读取的字符
        if ((type = Find_keyword()) == -1)
        {
            Info(IDENT);
            return IDENT;                   //非关键字则返回标识符
        }
        Info(type);
        return type;                        //返回关键字种类
    }
    if (isdigit(c))
    //处理数字开头
    {
        do
        {
            Add_char(c);                      //拼接数字串
            c = fgetc(fin);
            col++;
        } while (isdigit(c));
        if (c == '.')
        {
            do
            {
                Add_char(c);
                c = fgetc(fin);
                col++;
            } while (isdigit(c));
            if (c == 'F' || c == 'f')
            {
                Add_char(c);
                Info(FLOAT_CONST);                //返回浮点常量
                return FLOAT_CONST;
            }
            else if (c == 'L' || c == 'l')
            {
                Add_char(c);
                Info(DOUBLE_CONST);
                return DOUBLE_CONST;
            }
            else
            {
                ungetc(c, fin);
                col--;
                Info(FLOAT_CONST);
                return FLOAT_CONST;
            }
        }
        if ((c == 'x' || c == 'X') && strcmp(token_text, "0") == 0)
        {   //16进制数
            do
            {
                Add_char(c);                      //拼接数字串
                c = fgetc(fin);
                col++;
            } while (isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'));
        }
        if (c == 'l' || c == 'L')
        {
            Add_char(c);
            Info(LONG_CONST);         //长整型常量
            return LONG_CONST;
        }
        ungetc(c, fin);  col--;               //退回多读取的字符  
        Info(INT_CONST);
        return INT_CONST;                     //返回整型常量
    }
    switch (c)
    {
    case '+': c = fgetc(fin); col++;
        if (c == '+') { Info(PLUSPLUS); return PLUSPLUS; }  //返回++
        ungetc(c, fin); col--; Info(PLUS);  return PLUS;    //返回+
    case '-': c = fgetc(fin); col++;
        if (c == '-') { Info(SUBSUB); return SUBSUB; }      //返回--
        ungetc(c, fin); col--; Info(SUB); return SUB;       //返回-
    case '&': c = fgetc(fin); col++;
        if (c != '&')  return ERROR_TOKEN;
        Info(AND);     return AND;                          //返回与  
    case '|': c = fgetc(fin); col++;
        if (c != '|')  return ERROR_TOKEN;
        Info(OR);      return OR;                           //返回或
    case '=': c = fgetc(fin); col++;
        if (c == '=') { Info(EQ); return EQ; }              //返回==
        ungetc(c, fin); col--; Info(ASSIGN); return ASSIGN; //返回=
    //返回其他符号
    case '*':Info(MUL); return MUL;  case '/':Info(DIV); return DIV;
    case '%':Info(MOD); return MOD;
    case '<':
        if (type0 == INCLUDE) { Info(LAB); return LAB; }
        else { Info(RH);  return RH; }
    case '>':Info(LH);  return LH;   case '(':Info(LP);  return LP;
    case ')':Info(RP);  return RP;   case '[':Info(LSB); return LSB;
    case ']':Info(RSB); return RSB;  case '{':Info(LCB); return LCB;
    case '}':Info(RCB); return RCB;  case ';':Info(SEMI);return SEMI;
    case ',':Info(COMMA); return COMMA;  case '#':Info(WELL); return WELL;
    case '\'':Info(SINGLE_QUOTE); return SINGLE_QUOTE;
    case '\"':Info(DOUBLE_QUOTE); return DOUBLE_QUOTE;
    case '!':Info(NOT); return NOT;
    default:if (c == EOF)  return EOF;
           else    return ERROR_TOKEN;      //返回错误符号
    }

}

ASTree Program()
//语法单位<程序>,生成整个程序语法树的根结点,子树为外部定义序列或文件包含或宏定义或语句
{
    ASTree root = New();    root->tag = _PROGRAMMAR_;
    strcpy(root->data.ele_name, "程序");
    type = Get_token();
    if (type == EOF)   return root;
    if (IsType(type))
        root->child = ExtDefList();    //外部定义序列
    else if (type == WELL)
    {
        type = Get_token();
        if (type == INCLUDE)
            root->child = FileInclude();   //文件包含
        else if (type == DEFINE)
            root->child = MacroDefine();   //宏定义
        else
        {
            Destroy(root);
            Warning("语法错误！应输入\'define\'或\'include\'！");
            return NULL;
        }
        type = Get_token();
    }
    else
    {
        root->child = Statement();    //语句
        type = Get_token();
    }
    if (error == TRUE)
    {
        Destroy(root);
        return NULL;
    }
    ASTree p = root->child;
    while (p)
    {//生成其它子树
        if (type == EOF)   break;
        if (IsType(type))
            p->sibling = ExtDefList();
        else if (type == WELL)
        {
            type = Get_token();
            if (type == INCLUDE)
                p->sibling = FileInclude();
            else if (type == DEFINE)
                p->sibling = MacroDefine();
            else
            {
                Destroy(root);
                Warning("语法错误！应输入\'define\'或\'include\'！");
                return NULL;
            }
            type = Get_token();
        }
        else
        {
            p->sibling = Statement();
            type = Get_token();
        }
        p = p->sibling;
    }
    if (error == TRUE)
    {
        Destroy(root);
        return NULL;
    }
    return root;
}

ASTree FileInclude()
//处理文件包含
{
    type0 = INCLUDE;
    type = Get_token();
    if (type != LAB)
    {
        Warning("应输入左尖括号！");
        return NULL;
    }
    free(token_text); token_text = NULL;
    int c = fgetc(fin); col++;
    while (c != EOF && c != '\n')   //处理空格
    {
        if (c != ' ')    break;
        c = fgetc(fin);
        col++;
    }
    while (c != EOF && c != '\n')   //拼接文件名
    {
        if (c == '>' || c == ' ') break;
        Add_char(c);
        c = fgetc(fin);
        col++;
    }
    if (c == EOF || c == '\n')
    {
        Warning("缺少右尖括号！");
        return NULL;
    }
    if (c == ' ')   //处理空格
    {
        c = fgetc(fin);col++;
        while (c == ' ')
        {
            if (c == '>')   break;
            c = fgetc(fin);
            col++;
        }
        if (c != '>')
        {
            Warning("缺少右尖括号！");
            return NULL;
        }
    }
    printf("%-13s%s\n", "文件名", token_text);
    Info(RAB);
    ASTree root = New(); root->tag = _INCLUDE_;
    strcpy(root->data.ele_name, "文件包含");
    root->child = New(); root->child->tag = _FILENAME_;   //子孩子存文件名
    strcpy(root->child->data.ele_name, token_text);
    return root;
}

ASTree MacroDefine()
//处理宏定义
{
    ASTree root = New(); root->tag = _DEFINE_;
    strcpy(root->data.ele_name, "宏定义");
    ASTree p = root->child = New(); p->tag = _DEFINE_ONE_;  //第一子孩子存宏名
    ASTree q = p->sibling = New(); q->tag = _DEFINE_TWO_;   //第二子孩子存宏名替换词
    free(token_text); token_text = NULL;
    int c = fgetc(fin); col++;
    while (c != EOF && c != '\n')   //处理空格
    {
        if (c != ' ')    break;
        c = fgetc(fin);
        col++;
    }
    if (c == EOF || c == '\n')
    {
        Destroy(root);
        Warning("应输入宏名！");
        return NULL;
    }
    if (!isalpha(c) && c != '_')
    {
        Destroy(root);
        Warning("宏名不合法，应以字母或下划线开头！");
        return NULL;
    }
    while (c != EOF)   //拼接宏名
    {
        if (c == ' ' || c == '\n') break;
        Add_char(c);
        c = fgetc(fin);
        col++;
    }
    if (c == '\n' || c == EOF)
    {
        if (c == '\n')     col = 1, row++;
        printf("%-13s%s\n", "宏名", token_text);
        strcpy(p->data.ele_name, token_text);
        strcpy(q->data.ele_name, "");
        return root;
    }
    Copy(); free(token_text); token_text = NULL;
    while (c != EOF && c != '\n')   //处理空格
    {
        if (c != ' ')    break;
        c = fgetc(fin);
        col++;
    }
    if (c == '\n' || c == EOF)    //替换词为空
    {
        if (c == '\n')    col = 1, row++;
        printf("%-13s%s\n", "宏名", token_text0);
        strcpy(p->data.ele_name, token_text0);
        strcpy(q->data.ele_name, "");
        return root;
    }
    while (c != EOF)    //拼接宏名替换词
    {
        if (c == ' ' || c == '\n') break;
        Add_char(c);
        c = fgetc(fin);
        col++;
    }
    if (c == '\n')    col = 1, row++;
    printf("%-13s%s\n", "宏名", token_text0);
    printf("%-13s%s\n", "宏名替换词", token_text);
    strcpy(p->data.ele_name, token_text0);
    strcpy(q->data.ele_name, token_text);
    return root;
}

ASTree ExtDefList()
//语法单位<外部定义序列>,处理一系列外部定义,返回所有子树均为外部定义结点
{
    if (type == EOF)    return NULL;
    ASTree root = New();    root->tag = _EXTDEFLIST_;
    strcpy(root->data.ele_name, "外部定义序列");
    ASTree p = root->child = ExtDef();            //第一棵子树
    while (p)
    {
        p->sibling = ExtDef();
        p = p->sibling;
    }
    if (error == TRUE)
    {
        Destroy(root);
        return NULL;
    }
    return root;
}

ASTree ExtDef()
//语法单位<外部定义>,处理一个外部定义
{
    if (type == EOF)     return NULL;
    if (!(IsType(type))) return NULL;
    type0 = type;
    type = Get_token();
    if (type != IDENT && type != ARRAY)
    {
        Warning("应输入标识符！");
        return NULL;
    }
    Copy();
    type = Get_token();
    if (type != LP)    return ExtVarDef();   //外部变量定义
    else        return FuncDef();            //函数定义
}

ASTree ExtVarDef()
//语法成分<外部变量定义>,处理外部变量定义
{
    ASTree root = New(); root->tag = _EXTVARDEF_;
    root->child = New(); root->child->tag = TYPE;
    strcpy(root->data.ele_name, "外部变量定义");
    root->child->data.var_type = type0;     //第一子孩子存类型
    root->child->sibling = ExtVarList();    //第二子孩子存变量序列
    if (error == TRUE)
    {
        Destroy(root);
        return NULL;
    }
    return root;
}

ASTree ExtVarList()
//语法成分<变量序列>,处理变量序列,每个孩子均为一个外部变量
{
    ASTree root = New(); root->tag = _EXTVARLIST_;
    strcpy(root->data.ele_name, "外部变量序列");
    root->child = ExtVar();
    ASTree p = root->child;
    while (p)
    {
        if (type != COMMA && type != SEMI)
        {
            Destroy(root);
            Warning("应输入逗号或分号！");
            return NULL;
        }
        if (type == SEMI)
        {
            type = Get_token();
            return root;
        }
        type = Get_token();
        if (type != IDENT && type != ARRAY)
        {
            Destroy(root);
            Warning("应输入标识符！");
            return NULL;
        }
        Copy();
        p->sibling = ExtVar();
        type = Get_token();
        p = p->sibling;
    }
    if (error == TRUE)
    {
        Destroy(root);
        return NULL;
    }
    return root;
}

ASTree ExtVar()
//语法成分<外部变量>,处理一个外部变量
{
    ASTree root;
    root = New(); root->tag = EXTVAR;
    strcpy(root->data.var_name, token_text0);
    return root;
}

ASTree FuncDef()
//语法单位<函数定义>,处理函数定义
{
    ASTree root = New(); root->tag = _FUNCDEF_;
    root->child = New(); root->child->tag = TYPE;   //第一子孩子存类型
    root->child->data.var_type = type0;
    ASTree sec = root->child->sibling = New();
    sec->tag = FUNC;
    strcpy(sec->data.func_name, token_text0);   //第二子孩子存函数名
    Addfunc(token_text0);
    type = Get_token();
    ASTree third = sec->sibling = FormalParList();  //第三子孩子存形参序列
    if (error == TRUE)
    {
        Destroy(root);
        return NULL;
    }
    type = Get_token();
    if (type == LCB)
    {
        strcpy(root->data.ele_name, "函数定义");
        third->sibling = CompoundStatement();   //第四子孩子存函数体
        if (error == TRUE)
        {
            Destroy(root);
            return NULL;
        }
        strcpy(third->sibling->data.ele_name, "函数体");
    }
    else if (type == SEMI)
        strcpy(root->data.ele_name, "函数声明");
    else if (type != EOF)
    {
        Destroy(root);
        Warning("应输入分号！");
        return NULL;
    }
    type = Get_token();
    return root;
}

ASTree FormalParList()
//语法成分<形参序列>,处理形参序列,所有子孩子均为一个形参
{
    ASTree root = New(); root->tag = _FORMALPARLIST_;
    strcpy(root->data.ele_name, "形参序列");
    ASTree p = root->child = FormalPara();
    while (p)
    {
        type = Get_token();
        p->sibling = FormalPara();
        p = p->sibling;
    }
    if (error == TRUE)
    {
        Destroy(root);
        return NULL;
    }
    return root;
}

ASTree FormalPara()
//语法成分<形参>,处理一个形参
{
    if (type == RP || type == VOID)    return NULL;
    if (type == COMMA)      type = Get_token();
    if (!IsType(type)) { Warning("应输入类型！");return NULL; }
    ASTree p = New(); p->tag = FORPARA;
    strcpy(p->data.ele_name, "形参");
    ASTree fir = p->child = New(); fir->tag = TYPE;   //第一子孩子存类型
    fir->data.var_type = type;
    type = Get_token();
    if (type == IDENT || type == ARRAY)
    {
        fir->sibling = New();
        fir->sibling->tag = VAR;    //第二子孩子存变量名
        strcpy(fir->sibling->data.var_name, token_text);
    }
    else if (type != COMMA && type != RP)
    {
        Destroy(p);
        Warning("应输入逗号或右括号！");
        return NULL;
    }
    return p;
}

ASTree CompoundStatement()
//语法单位<复合语句>,处理复合语句,子孩子为局部变量定义序列或语句序列
{
    ASTree root = New(); root->tag = _COMPOUNDSTATEMENT_;
    strcpy(root->data.ele_name, "复合语句");
    type = Get_token();
    if (IsType(type))
        root->child = LocVarDefList();
    else
        root->child = StatementList();
    ASTree p = root->child;
    while (p)
    {

        if (IsType(type))
            p->sibling = LocVarDefList();
        else
            p->sibling = StatementList();
        p = p->sibling;
    }
    if (error == TRUE)
    {
        Destroy(root);
        return NULL;
    }
    if (type != RCB)
    {
        Destroy(root);
        Warning("应输入右大括号！");
        return NULL;
    }
    return root;
}

ASTree LocVarDefList()
//语法单位<局部变量定义序列>,处理一系列局部变量定义序列,每个孩子均为局部变量定义
{
    ASTree root = New(); root->tag = _LOCVARDEFLIST_;
    strcpy(root->data.ele_name, "局部变量定义序列");
    ASTree p = root->child = LocVarDef();
    while (p)
    {
        if (!IsType(type))
            break;
        p->sibling = LocVarDef();
        p = p->sibling;
    }
    if (error == TRUE)
    {
        Destroy(root);
        return NULL;
    }
    return root;
}

ASTree LocVarDef()
//语法单位<局部变量定义>,处理一个局部变量定义
{
    ASTree root = New(); root->tag = _LOCVARDEF_;
    strcpy(root->data.ele_name, "局部变量定义");
    root->child = New(); root->child->tag = TYPE;  //第一子孩子存类型
    root->child->data.var_type = type;
    type = Get_token();
    if (type != IDENT && type != ARRAY)
    {
        Destroy(root);
        Warning("应输入标识符！");
        return NULL;
    }
    root->child->sibling = LocVarList();   //第二子孩子存变量序列
    if (error == TRUE)
    {
        Destroy(root);
        return NULL;
    }
    return root;
}

ASTree LocVarList()
//语法成分<局部变量序列>,处理局部变量序列,每个子孩子均为一个局部变量
{
    ASTree root = New(); root->tag = _LOCVARLIST_;
    strcpy(root->data.ele_name, "局部变量序列");
    ASTree p = root->child = LocVar();
    while (p)
    {
        type = Get_token();
        if (type != COMMA && type != SEMI)
        {
            Destroy(root);
            Warning("应输入逗号或分号！");
            return NULL;
        }
        if (type == SEMI)
        {
            type = Get_token();
            return root;
        }
        type = Get_token();
        if (type != IDENT && type != ARRAY)
        {
            Destroy(root);
            Warning("应输入标识符！");
            return NULL;
        }
        p->sibling = LocVar();
        p = p->sibling;
    }
    if (error == TRUE)
    {
        Destroy(root);
        return NULL;
    }
    return root;
}

ASTree LocVar()
//语法成分<局部变量>,处理一个局部变量
{
    ASTree root = New(); root->tag = LOCVAR;
    strcpy(root->data.var_name, token_text);
    return root;
}

ASTree ActualParList()
//处理实参序列,每个子孩子均为一个实参
{
    ASTree root = New(); root->tag = _ACTUALPARLIST_;
    strcpy(root->data.ele_name, "实参序列");
    if (type == RP)
    {
        type = Get_token();
        if (type != SEMI)
        {
            Destroy(root);
            Warning("应输入分号！");
            return NULL;
        }
        return root;
    }
    if (type != IDENT && type != INT_CONST && type != FLOAT_CONST)
    {
        Destroy(root);
        Warning("应输入标识符或常量！");
        return NULL;
    }
    ASTree p = root->child = New(); p->tag = ACTPARA;
    strcpy(p->data.var_name, token_text);
    while (TRUE)
    {
        type = Get_token();
        if (type == RP)       break;
        else if (type == COMMA)    continue;
        else if (type == IDENT || type == INT_CONST || type == FLOAT_CONST)
        {
            p->sibling = New();
            p->sibling->tag = ACTPARA;
            strcpy(p->sibling->data.var_name, token_text);
        }
        else
        {
            Destroy(root);
            Warning("语法错误！应输入右括号或逗号或标识符或常量！");
            return NULL;
        }
        p = p->sibling;
    }
    type = Get_token();
    if (type != SEMI)
    {
        Destroy(root);
        Warning("应输入分号！");
        return NULL;
    }
    if (error == TRUE)
    {
        Destroy(root);
        return NULL;
    }
    return root;
}

ASTree StatementList()
//语法单位<语句序列>,处理一系列语句,每个子孩子均为一个语句
{
    if (type == RCB)   return NULL;
    ASTree root = New(); root->tag = _STATEMENTLIST_;
    strcpy(root->data.ele_name, "语句序列");
    if (type1 == IF)   type1 = -1;
    ASTree p = root->child = Statement();
    while (p)
    {
        if (type1 == IF)    type1 = -1;
        else   type = Get_token();
        if (IsType(type))    return root;
        p->sibling = Statement();
        p = p->sibling;
    }
    if (error == TRUE)
    {
        Destroy(root);
        return NULL;
    }
    return root;
}

ASTree Statement()
//语法单位<语句>,处理一个语句
{
    ASTree root, p, q, r;
    int t;
    if (IsType(type))    return LocVarDef();  //局部变量定义
    switch (type)
    {
    case IF:   //if语句
        root = New();
        type = Get_token();
        if (type != LP)
        {
            free(root);
            Warning("应输入左小括号！");
            return NULL;
        }
        type = Get_token();
        p = root->child = New();p->tag = CONDITION;  //第一子孩子存条件
        strcpy(p->data.ele_name, "条件");
        p->child = Expression(RP);
        if (error == TRUE)
        {
            Destroy(root);
            return NULL;
        }
        q = p->sibling = New(); q->tag = IFCLAUSE;  //第二子孩子存if子句
        strcpy(q->data.ele_name, "if子句");
        type = Get_token();t = type;
        q->child = Statement();
        if (error == TRUE)
        {
            Destroy(root);
            return NULL;
        }
        if (q->child == NULL && t != SEMI)
        {
            Destroy(root);
            Warning("应输入一个语句！");
            return NULL;
        }
        type = Get_token();
        type1 = IF;
        if (type == ELSE)
        {
            type1 = -1;
            q->sibling = New();
            q->sibling->tag = ELSECLAUSE;      //第三子孩子存else子句
            strcpy(q->sibling->data.ele_name, "else子句");
            type = Get_token(); t = type;
            q->sibling->child = Statement();
            if (error == TRUE)
            {
                Destroy(root);
                return NULL;
            }
            if (q->sibling->child == NULL && t != SEMI)
            {
                Destroy(root);
                Warning("应输入一个语句！");
                return NULL;
            }
            root->tag = IFELSE;
            strcpy(root->data.ele_name, "if_else 条件语句");
        }
        else
        {
            root->tag = _IF_;
            strcpy(root->data.ele_name, "if 条件语句");
        }
        break;
    case LCB:
        root = CompoundStatement(); break;
    case WHILE:    //while语句
        type0 = WHILE;
        root = New(); root->tag = _WHILE_;
        strcpy(root->data.ele_name, "while 循环");
        type = Get_token();
        if (type != LP)
        {
            Destroy(root);
            Warning("应输入左小括号！");
            return NULL;
        }
        type = Get_token();
        p = root->child = New(); p->tag = CONDITION;   //第一子孩子存条件
        strcpy(p->data.ele_name, "条件");
        p->child = Expression(RP);
        if (error == TRUE)
        {
            Destroy(root);
            return NULL;
        }
        q = p->sibling = New(); q->tag = LOOPBODY;  //第二子孩子存循环体
        strcpy(q->data.ele_name, "循环体");
        type = Get_token();t = type;
        q->child = Statement();
        if (error == TRUE)
        {
            Destroy(root);
            return NULL;
        }
        if (q->child == NULL && t != SEMI)
        {
            Destroy(root);
            Warning("应输入一个语句！");
            return NULL;
        }
        break;
    case FOR:    //for语句
        type0 = FOR;
        root = New(); root->tag = _FOR_;
        strcpy(root->data.ele_name, "for 循环");
        type = Get_token();
        if (type != LP)
        {
            Destroy(root);
            Warning("应输入左小括号！");
            return NULL;
        }
        type = Get_token();
        p = root->child = New(); p->tag = _FOR_ONE_;  //第一子孩子存初始语句
        strcpy(p->data.ele_name, "初始语句");
        p->child = Statement();
        if (error == TRUE)
        {
            Destroy(root);
            return NULL;
        }
        if (p->child->tag != _LOCVARDEF_)
            type = Get_token();
        q = p->sibling = New(); q->tag = CONDITION;   //第二子孩子存循环条件
        strcpy(q->data.ele_name, "条件");
        q->child = Expression(SEMI);
        if (error == TRUE)
        {
            Destroy(root);
            return NULL;
        }
        r = q->sibling = New(); r->tag = _FOR_TWO_;   //第三子孩子存更新语句
        strcpy(r->data.ele_name, "更新语句");
        type = Get_token();
        r->child = Expression(RP);
        if (error == TRUE)
        {
            Destroy(root);
            return NULL;
        }
        r->sibling = New(); r->sibling->tag = LOOPBODY;  //第四子孩子存循环体
        strcpy(r->sibling->data.ele_name, "循环体");
        type = Get_token(); t = type;
        r->sibling->child = Statement();
        if (error == TRUE)
        {
            Destroy(root);
            return NULL;
        }
        if (r->sibling->child == NULL && t != SEMI)
        {
            Destroy(root);
            Warning("应输入一个语句！");
            return NULL;
        }
        break;
    case BREAK:   //break语句
        if (type0 != FOR && type0 != WHILE)
        {
            Warning("break语句只能出现在for循环或while循环中！");
            return NULL;
        }
        root = New(); root->tag = _BREAK_;
        strcpy(root->data.ele_name, "break 语句");
        type = Get_token();
        if (type != SEMI)
        {
            Destroy(root);
            Warning("应输入分号！");
            return NULL;
        }
        break;
    case CONTINUE:   //continue语句
        if (type0 != FOR && type0 != WHILE)
        {
            Warning("continue语句只能出现在for循环或while循环中！");
            return NULL;
        }
        root = New(); root->tag = _CONTINUE_;
        strcpy(root->data.ele_name, "continue 语句");
        type = Get_token();
        if (type != SEMI)
        {
            Destroy(root);
            Warning("应输入分号！");
            return NULL;
        }
        break;
    case RETURN:    //return语句
        root = New(); root->tag = _RETURN_;
        strcpy(root->data.ele_name, "return 语句");
        root->child = New(); root->child->tag = _RETURNVAR_;
        strcpy(root->child->data.ele_name, "返回值");   //子孩子存返回值
        type = Get_token();
        root->child->child = Expression(SEMI);
        break;
    case LP:
        type = Get_token();
        root = Expression(SEMI); break;
    case INT_CONST:case FLOAT_CONST:
        root = New(); root->tag = _EXPRESSION_;
        strcpy(root->data.ele_name, "表达式语句");
        root->child = Expression(SEMI);
        break;
    case IDENT:
        root = New();
        if (IsFunc(token_text))
        {   //函数调用语句
            Copy();
            type = Get_token();
            if (type != LP)
            {
                Destroy(root);
                Warning("应输入左括号！");
                return NULL;
            }
            root->tag = _FUNCCALL_;
            strcpy(root->data.ele_name, "函数调用");
            p = root->child = New(); p->tag = FUNC;
            strcpy(p->data.func_name, token_text0);  //第一子孩子存函数名
            type = Get_token();
            p->sibling = ActualParList();    //第二子孩子存实参序列
        }
        else
        {
            root->tag = _EXPRESSION_;
            strcpy(root->data.ele_name, "表达式语句");
            root->child = Expression(SEMI);
        }
        break;
    case RCB: root = NULL;break;
    case SEMI: root = NULL;break;
    case EOF:root = NULL;break;
    default: { Warning("非法语句！");return NULL;}
    }
    if (error == TRUE)
    {
        Destroy(root);
        return NULL;
    }
    return root;
}

ASTree Expression(Token_kind endsym)
//语法单位<表达式>,处理一个表达式
{
    if (type == endsym)    return NULL;
    Stack ops, ods;
    InitStack(&ops);InitStack(&ods);     //初始化操作符栈和操作数栈
    ASTree q = New();q->tag = OPERATOR;
    q->data.op = WELL; Push(&ops, q);
    ASTree top = Gettop(ops), t, t1, t2;
    while ((type != WELL || top->data.op != WELL) && error == FALSE)
    {
        if (type >= IDENT && type <= DOUBLE_CONST)
        {   //操作数入栈
            q = New(); q->tag = VAR;
            strcpy(q->data.var_name, token_text);
            Push(&ods, q);
            type = Get_token();
        }
        else if (IsOperater(type))
        {
            top = Gettop(ops);
            switch (Precede(top->data.op, type))
            {
            case '<':q = New(); q->tag = OPERATOR;q->data.op = type;
                Push(&ops, q); type = Get_token(); break;  //优先级较小则入栈
            case '=':if (!Pop(&ops, &t)) error = TRUE;
                type = Get_token();break;       //去括号
            case '>':if (!Pop(&ods, &t2)) error = TRUE;
                if (!Pop(&ods, &t1))  error = TRUE;
                if (!Pop(&ops, &t))   error = TRUE;
                t->child = t1;t1->sibling = t2;
                Push(&ods, t);break;    //优先级较大则出栈两个操作数一个操作符并将运算结果入栈
            default:
                if (endsym == RP && type == RP)  break;
                error = TRUE;break;
            }
            if (type == endsym && ops.top == 1)    type = WELL;
        }
        else if (type == endsym)
        {
            type = WELL;
            if (endsym == SEMI && ops.top != 1)
            {
                if (!Pop(&ods, &t2)) error = TRUE;
                if (!Pop(&ods, &t1))  error = TRUE;
                if (!Pop(&ops, &t))   error = TRUE;
                t->child = t1;t1->sibling = t2;
                Push(&ods, t);
            }
        }
        else error = TRUE;
        top = Gettop(ops);
    }
    top = Gettop(ops);
    if (ods.top == 1 && top->data.op == WELL && error == FALSE)
    {
        free(top);  top = Gettop(ods);
        free(ops.ptr); free(ods.ptr);
        return top;
    }
    else  //运算错误则销毁两个栈并报错
    {
        for (int i = 0;i < ods.top;i++)
            free(ods.ptr[i]);
        for (int i = 0;i < ops.top;i++)
            free(ops.ptr[i]);
        free(ops.ptr);free(ods.ptr);
        Warning("表达式不合法！");
        return NULL;
    }
    return NULL;
}

void Format(ASTree root, int dep)
//先序遍历语法树
{
    ASTree p, q, r;
    if (root == NULL)  return;
    switch (root->tag)
    {//根据结点类型输出相应内容
    case _PROGRAMMAR_:case _EXTDEFLIST_:case _LOCVARDEFLIST_:
    case _STATEMENTLIST_:
        break;
    case _INCLUDE_:
    {
        fprintf(fout, "#include <%s>\n", root->child->data.ele_name);
        row0++;return;
    }
    case _DEFINE_:
    {
        fprintf(fout, "#define %s %s\n",
            root->child->data.ele_name, root->child->sibling->data.ele_name);
        row0++;return;
    }
    case _EXTVARDEF_:
    {//外部变量定义
        p = root->child;
        fprintf(fout, "%s ", keyword[p->data.var_type]);
        p = p->sibling->child;
        while (p)
        {
            fprintf(fout, "%s", p->data.var_name);
            p = p->sibling;
            if (p == NULL) { fprintf(fout, ";\n"); row0++; }
            else fprintf(fout, ", ");
        }
        return;
    }
    case _FUNCDEF_:
    {//函数定义
        p = root->child;
        fprintf(fout, "%s ", keyword[p->data.var_type]);
        p = p->sibling;
        fprintf(fout, "%s(", p->data.func_name);
        p = p->sibling; q = p->child;
        if (q == NULL) fprintf(fout, ")");
        while (q)
        {
            fprintf(fout, "%s %s", keyword[q->child->data.var_type]
                , q->child->sibling->data.var_name);
            q = q->sibling;
            if (q == NULL) fprintf(fout, ")");
            else fprintf(fout, ", ");
        }
        p = p->sibling;
        if (p == NULL) { fprintf(fout, ";\n");row0++; }
        else
        {
            fprintf(fout, "\n");
            row0++;
            Format(p, dep);
        }
        return;
    }
    case _FUNCCALL_:
    {//函数调用
        for (int i = 0;i < dep;i++)
            fprintf(fout, "    ");
        p = root->child;
        fprintf(fout, "%s(", p->data.func_name);
        p = p->sibling->child;
        if (p == NULL)
        {
            fprintf(fout, ");\n");
            row0++;
        }
        while (p)
        {
            fprintf(fout, "%s", p->data.var_name);
            p = p->sibling;
            if (p)  fprintf(fout, ", ");
            else { fprintf(fout, ");\n");row0++; }
        }
        return;
    }
    case _COMPOUNDSTATEMENT_:
    {//复合语句
        for (int i = 0;i < dep;i++)
            fprintf(fout, "    ");
        fprintf(fout, "{\n");row0++;
        p = root->child;
        if (p == NULL)
        {
            for (int i = 0;i < dep;i++)
                fprintf(fout, "    ");
            fprintf(fout, "}\n");row0++;
        }
        while (p)
        {
            Format(p, dep + 1);
            p = p->sibling;
            if (p == NULL)
            {
                for (int i = 0;i < dep;i++)
                    fprintf(fout, "    ");
                fprintf(fout, "}\n");row0++;
            }
        }
        return;
    }
    case _LOCVARDEF_:
    {//局部变量定义
        for (int i = 0;i < dep;i++)
            fprintf(fout, "    ");
        p = root->child;
        fprintf(fout, "%s ", keyword[p->data.var_type]);
        p = p->sibling->child;
        while (p)
        {
            fprintf(fout, "%s", p->data.var_name);
            p = p->sibling;
            if (p == NULL) { fprintf(fout, ";\n");row0++; }
            else fprintf(fout, ", ");
        }
        return;
    }
    case _IF_:
    {//if语句
        for (int i = 0;i < dep;i++)
            fprintf(fout, "    ");
        fprintf(fout, "if (");
        p = root->child;
        ftraverse(p->child);
        fprintf(fout, ")\n");row0++;
        p = p->sibling;
        if (p->child->tag == _COMPOUNDSTATEMENT_)
            Format(p->child, dep);
        else
            Format(p->child, dep + 1);
        return;
    }
    case IFELSE:
    {//ifelse语句
        for (int i = 0;i < dep;i++)
            fprintf(fout, "    ");
        fprintf(fout, "if (");
        p = root->child;
        ftraverse(p->child);
        fprintf(fout, ")\n");row0++;
        p = p->sibling;
        if (p->child->tag == _COMPOUNDSTATEMENT_)
            Format(p->child, dep);
        else
            Format(p->child, dep + 1);
        p = p->sibling;
        for (int i = 0;i < dep;i++)
            fprintf(fout, "    ");
        fprintf(fout, "else\n");row0++;
        if (p->child->tag == _COMPOUNDSTATEMENT_)
            Format(p->child, dep);
        else
            Format(p->child, dep + 1);
        return;
    }
    case _WHILE_:
    {//while语句
        for (int i = 0;i < dep;i++)
            fprintf(fout, "    ");
        fprintf(fout, "while (");
        p = root->child;
        ftraverse(p->child);
        fprintf(fout, ")\n");row0++;
        p = p->sibling;
        if (p->child->tag == _COMPOUNDSTATEMENT_)
            Format(p->child, dep);
        else
            Format(p->child, dep + 1);
        return;
    }
    case _FOR_:
    {//for语句
        for (int i = 0;i < dep;i++)
            fprintf(fout, "    ");
        fprintf(fout, "for (");
        p = root->child;q = p->child;
        if (q->tag == _LOCVARDEF_)
        {
            r = q->child;
            fprintf(fout, "%s ", keyword[r->data.var_type]);
            r = r->sibling->child;
            while (r)
            {
                fprintf(fout, "%s", r->data.var_name);
                r = r->sibling;
                if (r == NULL) fprintf(fout, ";");
                else fprintf(fout, ", ");
            }
        }
        else
        {
            ftraverse(q->child);
            fprintf(fout, ";");
        }
        p = p->sibling;
        ftraverse(p->child); fprintf(fout, ";");
        p = p->sibling;
        ftraverse(p->child); fprintf(fout, ")\n"); row0++;
        p = p->sibling;
        if (p->child->tag == _COMPOUNDSTATEMENT_)
            Format(p->child, dep);
        else
            Format(p->child, dep + 1);
        return;
    }
    case _BREAK_:
    {//break语句
        for (int i = 0;i < dep;i++)
            fprintf(fout, "    ");
        fprintf(fout, "break;\n"); row0++;
        return;
    }
    case _CONTINUE_:
    {//continue语句
        for (int i = 0;i < dep;i++)
            fprintf(fout, "    ");
        fprintf(fout, "continue;\n"); row0++;
        return;
    }
    case _RETURN_:
    {//return语句
        for (int i = 0;i < dep;i++)
            fprintf(fout, "    ");
        fprintf(fout, "return ");
        ftraverse(root->child->child);
        fprintf(fout, ";\n"); row0++;
        return;
    }
    case OPERATOR:case VAR:
    {//变量或操作符
        for (int i = 0;i < dep;i++)
            fprintf(fout, "    ");
        ftraverse(root);
        fprintf(fout, ";\n"); row0++;
        return;
    }
    case _EXPRESSION_:
    {//表达式
        for (int i = 0;i < dep;i++)
            fprintf(fout, "    ");
        ftraverse(root->child);
        fprintf(fout, ";\n"); row0++;
        return;
    }
    }
    p = root->child;
    while (p)
    {//遍历输出子孩子
        Format(p, dep);
        p = p->sibling;
    }
}

void Writenote()
//将注释写入格式化文件
{
    char filename[100];
    strncpy(filename, sourcefile, strlen(sourcefile) - 2);
    strcat(filename, "_formated.c");    //生成格式化文件名
    if (notes.num == 0)
    {
        char command[100] = "rename temp.c ";
        strcat(command, filename);
        fclose(fin); fclose(fout);
        system(command);
        printf("\n格式化源程序名：%s\n\n", filename);
        return;
    }
    row0--;
    fclose(fin);fclose(fout);
    fin = fopen("temp.c", "r");
    fout = fopen(filename, "w");
    char s[100];
    int r = 1, num = 0, d = row - row0;
    for (int i = 0;i < notes.num; i++)
        notes.notelist[i].row -= d;
    while (fgets(s, 100, fin) != NULL)
    {
        s[strlen(s) - 1] = '\0';
        fputs(s, fout);
        if (notes.notelist[num].row == r)  //在对应行输出注释
        {
            if (notes.notelist[num].type == 1)
                fprintf(fout, "\t\t//%s\n", notes.notelist[num].data);
            else
                fprintf(fout, "\t\t/*%s*/\n", notes.notelist[num].data);
            num++;
        }
        else fputc('\n', fout);
        r++;
    }
    fclose(fin);    fclose(fout);
    system("del temp.c");
    printf("\n格式化源程序名：%s\n\n", filename);
}

#endif