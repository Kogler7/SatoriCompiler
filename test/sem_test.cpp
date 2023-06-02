#include "test.h"
#include "irgen/sem.h"
#include "lexer/lexer.h"
#include "parser/ebnf.h"
#include "parser/slr1/parser.h"
#include "utils/log.h"

using namespace std;

#include "test.sem"

void semTest()
{
    reg_nodes();
    auto node = MAKE_NODE("ExprNode");
    node->act();

    EBNFParser ebnfParser("./assets/lex/syntax.lex");
    Grammar g = ebnfParser.parse("./assets/stx/rsc.estx");
    SLR1Grammar G = SLR1Grammar(g);
    G.printRules();
    G.extractLeftCommonFactor();
    G.printRules();
    G.printTerminals();
    G.printNonTerms();
    G.printFollow();
    // SLR1Parser slr1(G);
    // Lexer lexer("./assets/lex/rsc.lex");
    // auto tokens = lexer.tokenize("./assets/src/test.rsc");
    // lexer.printTokens(tokens);
    // tokens = G.transferTokens(tokens);
    // lexer.printTokens(tokens);
    // G.printItems();
    // G.printClusters();
    // G.printGoTrans();
    // G.printSLR1Table();
    // info << "result: \n"
    //      << slr1.parse(tokens) << endl;
    // slr1.getTree()->print();
}