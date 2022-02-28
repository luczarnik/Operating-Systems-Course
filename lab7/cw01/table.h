#ifndef __TABLE__
#define __TABLE

#define MAXSIZE 5

struct Table
{
    int tab[MAXSIZE];
    int left;
    int right;
    int elNo;
};

void putPizza(struct Table* table,int pizza);
int getPizza(struct Table* table);
void initialize(struct Table* table);

#endif