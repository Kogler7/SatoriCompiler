/**
 * @file nfa_test.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Test NFA
 * @date 2023-05-06
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "test.h"
#include "lexer/lexer.h"
#include "lexer/regexp/parser.h"

void nfaTest()
{
    string raw = "";
    cin >> raw;
    cout << "input raw: " << raw << endl;
    RegexpParser convertor = RegexpParser(raw);
    convertor.regexpPreproc();
    convertor.tmpReg2postfix();
    convertor.postfix2FA();
    FiniteAutomaton nfa = convertor.getNFA();
    nfa.printStates();
    nfa.printTransitions();
    string word;
    while (true)
    {
        cout << "input word: ";
        cin >> word;
        cout << "inputted word: " << word << endl;
        Viewer vWord = Viewer(word);
        string result;
        bool res = nfa.accepts(vWord, result);
        cout << "match result: " << (res ? "yes" : "no") << endl;
        cout << "matched word: " << result << endl;
    }
}