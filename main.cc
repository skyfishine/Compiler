#include "lexer.h"
#include "parser.h"

int main() {
    // Lexer lex("test.pas");
    // lex.LexicalAnalyze();

    Parser parser("test.pas");
    parser.analyze();

}