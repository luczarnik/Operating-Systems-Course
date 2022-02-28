#include <stdio.h>

#ifdef DLL
#include "dllmerge.h"
#include <dlfcn.h>
#endif

#ifndef DLL
#include "merge.h"
#endif

int main() {//first argument file location with sequence definition
    #ifdef DLL
    initializeFunctionsFromDynamicLibrary();
    #endif
    struct Sequence sequence = createSequence("file1.txt:file2.txt");
    struct Table table =createTable(sequence.size);
    mergeSequence(sequence,table,0);
    printBlocks(table);
    return 0;
    #ifdef DLL
    #endif
}
