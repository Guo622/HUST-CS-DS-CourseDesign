#include "def.h"
#include "assist.h"
#include "func.h"

int main()
{
    printf("请输入.c源文件名：");
    scanf("%s", sourcefile);
    printf("\n");
    fin = fopen(sourcefile, "r");
    if (fin == NULL)
    {
        printf("打开文件失败！\n");
        system("pause");
        return 0;
    }
    Root = Program();
    if (Root == NULL)     //生成失败
        return 0;
    fout = fopen("temp.c", "w");
    Format(Root, 0);                    //生成格式化文件
    ASTree p, q, r;
    if (notes.num > 0)    //生成注释结点
    {
        q = New(); q->tag = NOTE;
        q->data.note = notes.notelist[0];
        r = New(); r->tag = _NOTE_;
        strcpy(r->data.ele_name, "注释");
        r->child = p = q;
    }
    for (int i = 1;i < notes.num;i++)
    {
        p->sibling = New();
        p->sibling->tag = NOTE;
        p->sibling->data.note = notes.notelist[i];
        p = p->sibling;
    }
    if (notes.num > 0)
    {//将注释结点加入语法树
        if (Root->child == NULL) Root->child = r;
        else
        {
            p = Root->child;
            while (p->sibling)   p = p->sibling;
            p->sibling = r;
        }
    }
    printf("\n\n");
    Traverse(Root, 1);                  //遍历语法树
    Writenote();                        //写入注释
    End();                              //结束操作
    system("pause");
    return 0;
}
