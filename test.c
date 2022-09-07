#include <stdio.h>
#include <stdlib.h>
#define A 10000
#define MAXN 100
int a, b, g[10];        /* test example */
long c, h[10];
float d, i[10];
double f, j[10];
char k;
a = 123;
b = 0x123;
c = 123L;
f = 0.123F;
int func_dec(int a, float b); // function declaration
void func_def()              //function definition
{
    int i, j, k;
    float p, q, r;
}
int main()
{
    func_dec(a, d);
    c = a + b;
    if (c > 1)
        a = a + 1;
    if (a > b && c || d < f)
    {
        a = a > b && c || d < f && (a* b / c - 1);
        b = b + 1;
    }
    d = d - 1;
    if (a > 1)
    {
        int i;
        if (b > 1)
        {
            if (c > 1)
                a = a + 1;
            else
                b = b + 1;
        }
        else
            c = c + 1;
    }
    while (a)
    {
        int i, j;
        for (i = 1;i < 10;i = i + 1)
        {
            a = a + 1;
        }
        if (a > 1)
            break;
        else
            continue;
    }
    for (a = 1;a;a = a - 1)
    {
        if (a > 1)
            continue;
        for (b = 1;b > 1;b = b + 1)
        {
            a = a + 1;
            b = b - 1;
            c = c + 1;
        }
    }
    return 0;
}