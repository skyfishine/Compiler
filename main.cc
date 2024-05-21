#include "lexer.h"
#include "parser.h"
#include <cassert>
#include <iostream>

int main(int argc, const char **argv)
{
    // Lexer lex("test.pas");
    // lex.LexicalAnalyze();
    if (argc < 2)
    {
        Parser parser("test.pas");
        parser.analyze();
    }
    else {
        Parser Parser(argv[1]);
        Parser.analyze();
    }
    
}