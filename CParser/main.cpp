
#include "include/compiler.h"

int main(int argc, char* argv[])
{
    //ÖÐÎÄ
    Compiler compiler;
    if(compiler.init()!=State::OK)
        return 1;
    if(compiler.compile()==State::OK)
    {
        compiler.output();
    }
    else
    {
        return -1;
    }
    return 0;
    /*
    Lexer txysb;
    txysb.set_input("./test2.c");
    txysb.set_result("./result.txt");
    txysb.set_statistics("./statistics.txt");
    txysb.set_table("./table.txt");
    txysb.set_errors("./errors.txt");

    txysb.start_analysis();
    Token token;
    while(!txysb.getNextLexical(token))
    {
        printf("symbol%d at line %d,col %d\n" , token.symbol_id,token.line,token.col);
    }*/
    return 0;
}
