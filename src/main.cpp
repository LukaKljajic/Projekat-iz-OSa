#include "global.h"

extern int userMain(int argc, char* argv[]);

int main(int argc, char* argv[]){
    Global::initialize();
    int ret = userMain(argc, argv);
    Global::finalize();
    return ret;
}