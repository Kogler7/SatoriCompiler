/**
 * @file test.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Test
 * @date 2023-04-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "test.h"
#include "lexer.h"
#include "regexp.h"

set<string> keywords = {
	"auto", "break", "case", "char", "const", "continue", "default",
	"do", "double", "else", "enum", "extern", "float", "for", "goto",
	"if", "int", "long", "register", "return", "short", "signed",
	"sizeof", "static", "struct", "switch", "typedef", "union",
	"unsigned", "void", "volatile", "while"};

set<string> separators = {
	"(", ")", "[", "]", "{", "}", ",", ";", ":", ".",
	"&", "*", "+", "-", "~", "!", "/", "%", "<", ">",
	"^", "|", "=", "?", "#", "\\", "\"", "\`"};

void lexerMain()
{
	Lexer lexer("./assets/cpp.lex");
	// lexer.readCodeFile("./assets/code.cpp");
	lexer.readSrcFile("./assets/error.cpp");
	// lexer.readCodeFile("./assets/test.cpp");
	lexer.tokenize();
	lexer.printTokens();
}

void lexerTest()
{
	Lexer lexer;
	int id = 0;
	for (auto &keyword : keywords)
	{
		lexer.addKeywordId(keyword, id++);
	}
	for (auto &sep : separators)
	{
		lexer.addKeywordId(sep, id++);
	}
	lexer.addTokenType("BLANK", "[\\t\\n\\r\\f\\v ]+");
	lexer.addTokenType("IGNORE", "\"([\\x0a-\\x0d\\x20-x21\\0x23-\\x7e]|\\\\\")*\"");
	lexer.addTokenType("IGNORE", "/\\\*([\\x0a-\\x0d\\x20-\\x29\\x2b-\\x7e]*|\\*[\\x0a-\\x0d\\x20-\\x2e\\x30-\\x7e]*)*\\*/");
	lexer.addTokenType("IDENTIFIER", "[a-zA-Z_][a-zA-Z0-9_]*");
	lexer.addTokenType("SEPARATOR", "[\\+\\-\\*\\\\%=\\(\\){}\\[\\]<>;,.\\|&^!]");
	lexer.addTokenType("SEPARATOR", ">=|<=|!=|==|\\+\\+|\\-\\-|\\|\\||&&|\\*=|/=|\\+=|\\-=|%=|<<|>>");
	lexer.addTokenType("STRING", "\"([\\x0a-\\x0d\\x20-x21\\0x23-\\x7e]|\\\")*\"");
	lexer.addTokenType("MACRO", "#[a-zA-Z][a-zA-Z0-9]*");
	lexer.addTokenType("INCLUDE", "\"[a-zA-Z0-9_.]+\"|<[a-zA-Z0-9_.]+>");
	lexer.addTokenType("INTEGER", "(\\-|\\+|\\e)[0-9]+");
	lexer.addTokenType("REAL", "(\\-|\\+|\\e)[0-9]+\\.[0-9]+");
	lexer.addTokenType("CHARACTER", "\'[\\x0a-\\x0d\\x20-x21\\0x23-\\x7e]*\'");

	lexer.addIgnoredType("BLANK");
	lexer.addIgnoredType("IGNORE");
	lexer.addReservedType("IDENTIFIER");
	lexer.addReservedType("SEPARATOR");

	lexer.tokenize("int main() { int a = 1; int b = 2; int c = a + b; return 0; }");
	lexer.printTokens();
}

void nfaTest() // 实验性质的测试函数
{
	string raw = "";
	cin >> raw;
	cout << "input raw: " << raw << endl;
	Regexp2FA convertor = Regexp2FA(raw);
	convertor.regexpPreproc();
	convertor.tmpReg2postfix();
	convertor.postfix2FA();
	FiniteAutomaton nfa = convertor.getNFA();
	nfa.print();
	string word;
	while (true)
	{
		cout << "input word: ";
		cin >> word;
		cout << "inputted word: " << word << endl;
		viewer vWord = viewer(word);
		string result;
		bool res = nfa.accepts(vWord, result);
		cout << "match result: " << (res ? "yes" : "no") << endl;
		cout << "matched word: " << result << endl;
	}
}