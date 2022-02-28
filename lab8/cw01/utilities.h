#ifndef ___UTILITIES___
#define ___UTILITIES___

enum MODE
{
    value, block
};

struct Argument
{
    int lower;
    int upper;
    int** pgmAccessor;
    int** copyAccessor;
    int width;
    int height;
};

void reversePixel(int* value);
void valueSelecting(struct Argument* arg);
void blockSelecting(struct Argument* arg);

#endif