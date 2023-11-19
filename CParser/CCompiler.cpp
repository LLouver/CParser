
#include "include/compiler.h"

int main(int argc, char* argv[])
{
    //中文
    Compiler compiler;
    compiler.init();
    if(compiler.compile()==State::OK)
    {
        compiler.output();
    }
    return 0;
}
