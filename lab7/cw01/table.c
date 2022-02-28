#include "table.h"
#include <stdio.h>
#include <stdlib.h>

void putPizza(struct Table * tab, int pizza)
{
    if (tab->elNo >= MAXSIZE )
    {
        fprintf(stderr,"Table overload!\n");
        exit(-1);
    }
    tab->tab[tab->right]=pizza;
    tab->right=(tab->right+1)%MAXSIZE;
    tab->elNo++;
}

int getPizza(struct Table* tab)
{
    if (tab->elNo == 0 )
    {
        fprintf(stderr,"Getting pizza from empty table!\n");
        exit(-1);
    }
    int answer = tab->tab[tab->left];
    tab->tab[tab->left]=-1;
    tab->elNo--;
    tab->left=(tab->left+1)%MAXSIZE;
    return answer;
}

void initialize(struct Table* tab)
{
    tab->elNo=0;
    tab->left=0;
    tab->right=0;
    for (int i=0;i<MAXSIZE;i++) tab->tab[i]=-1;
}