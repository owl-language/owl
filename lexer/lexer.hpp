#ifndef lexer_hpp
#define lexer_hpp
#include <iostream>
#include <vector>
#include <unordered_map>
#include "../tokens/tokens.hpp"
#include "filebuffer.hpp"
using namespace std;

class OwlLexer {
    private:
        SourceBuffer sb;
        TokenType handleKeywordOrId(string word);
        Token handleSpecials();
        Token extractFullNumber();
        string extractFullWord();
        vector<Token> tokenize();
        bool inComment;
        bool inString;
        void initReserved();
        void processString(vector<Token>& tokenList);
        unordered_map<string, TokenType> reserved;
    public:
        OwlLexer() {
            inComment = false;
            initReserved();
        }
        vector<Token> tokenize(string filename) {
            sb.loadFile(filename);
            return tokenize();
        }
        
};

void OwlLexer::initReserved() {
    reserved["begin"] = BEGIN;
    reserved["end"]  = END;
    reserved["read"] = READ;
    reserved["print"] = PRINT;
    reserved["input"] = READ;
    reserved["if"] = IF;
    reserved["then"] = THEN;
    reserved["else"] = ELSE;
    reserved["func"] = FUNC;
    reserved["while"] = WHILE;
    reserved["let"] = LET;
    reserved["int"] = INT;
    reserved["char"] = CHAR;
    reserved["program"] = PROG;
    reserved["return"] = RETURN;
    reserved["array"] = ARRAY;
    reserved["string"] = STR;
    reserved["rand"] = RAND;
    reserved["real"] = REALT;
}

TokenType OwlLexer::handleKeywordOrId(string word) {
    if (reserved.find(word) != reserved.end()) {
        return reserved.at(word);
    } else {
        return ID;
    }
    return ERROR;
}

Token OwlLexer::handleSpecials() {
    if (sb.Char() == ':') {
        sb.GetChar();
        if (sb.Char() == '=') {
            return Token(ASSIGN, ":=", -1, sb.lineNumber());
        }
        sb.UnGetChar();
        return Token(COLON, ":", -1, sb.lineNumber());
    } else if (sb.Char() == '!') {
        sb.GetChar();
        if (sb.Char() == '=') {
            return Token(NOTEQUAL, "!=", -1, sb.lineNumber());
        }
        sb.UnGetChar();
        return Token(NOT, "!", -1, sb.lineNumber());
    } else if (sb.Char() == '<') {
        sb.GetChar();
        if (sb.Char() == '=') {
            return Token(LESSEQ, "<=", -1, sb.lineNumber());
        }
        sb.UnGetChar();
        return Token(LESS, "<", -1, sb.lineNumber());
    } else if (sb.Char() == '>') {
        sb.GetChar();
        if (sb.Char() == '=') {
            return Token(GREATEREQ, ">=", -1, sb.lineNumber());
        }
        sb.UnGetChar();
        return Token(GREATER, ">=", -1, sb.lineNumber());
    } else if (sb.Char() == '{') {
        sb.GetChar();
        if (sb.Char() == '*') {
            return Token(OPENCOMMENT, "{*", -1, sb.lineNumber());
        }
        sb.UnGetChar();
    } else if (sb.Char() == '*') {
        sb.GetChar();
        if (sb.Char() == '}') {
            return Token(CLOSECOMMENT, "*}", -1, sb.lineNumber());
        } 
        sb.UnGetChar();
        return Token(MULT, "*", -1, sb.lineNumber());        
    } else if (sb.Char() == '(') {
        return Token(LPAREN, "(", -1, sb.lineNumber());
    } else if (sb.Char() == ')') {
        return Token(RPAREN, ")", -1, sb.lineNumber());
    } else if (sb.Char() == '[') {
        return Token(LSQBRACKET, "[", -1, sb.lineNumber());
    } else if (sb.Char() == ']') {
        return Token(RSQBRACKET, "]", -1, sb.lineNumber());
    } else if (sb.Char() == '+') {
        return Token(PLUS, "+", -1, sb.lineNumber());
    } else if (sb.Char() == '-') {
        return Token(MINUS, "-", -1, sb.lineNumber());
    } else if (sb.Char() == '/') {
        return Token(DIVD, "/", -1, sb.lineNumber());
    } else if (sb.Char() == '=') {
        return Token(EQUAL, "=", -1, sb.lineNumber());      
    } else if (sb.Char() == '\"') {
        return Token(QUOTE, "\"", -1, sb.lineNumber());
    } else if (sb.Char() == ';') {
        return Token(SEMI, ";", -1, sb.lineNumber());
    } else if (sb.Char() == ',') {
        return Token(COMA, ",", -1, sb.lineNumber());
    } else if (sb.Char() == '.') {
        return Token(PERIOD, ".", -1, sb.lineNumber());
    } else if (sb.Char() == '\'') {
        return Token(SQUOTE, "\'", -1, sb.lineNumber());
    } 
    cout<<"ERROR uknown token: "<<sb.Char()<<"\n";
    sb.GetChar();
    return Token(ERROR, "Error", -1, sb.lineNumber());
}

Token OwlLexer::extractFullNumber() {
    string asStr;
    Token nextToken;
    bool isFloat = false;
    while (sb.Char() != sb.EOFMarker() && (isdigit(sb.Char()) || sb.Char() == '.')) {
        if (sb.Char() == '.') isFloat = true;
        asStr.push_back(sb.Char());
        sb.GetChar();
    }
    nextToken.lineno = sb.lineNumber();
    nextToken.stringval = asStr;
    if (isFloat) {
        nextToken.tokenval = REALNUM;
        nextToken.realval = atof(asStr.c_str());
    } else {
        nextToken.tokenval = NUM;
        nextToken.numval = atoi(asStr.c_str());
    }
    return nextToken;
}

string OwlLexer::extractFullWord() {
    string word;
    while (sb.Char() != sb.EOFMarker() && isalpha(sb.Char())) {
        word.push_back(sb.Char());
        sb.GetChar();
    }
    return word;
}

void OwlLexer::processString(vector<Token>& tokenList) {
    string str;
    Token nextToken;
    while (sb.Char() != sb.EOFMarker() && sb.Char() != '\"') {
        str.push_back(sb.Char());
        sb.GetChar();
    }
    nextToken.tokenval = STRING_LITERAL;
    nextToken.stringval = str;
    nextToken.lineno = sb.lineNumber();
    tokenList.push_back(nextToken);
    nextToken = handleSpecials();
    if (nextToken.tokenval != QUOTE) {
        cout<<"Error processing string."<<endl;
        exit(0);
    }
    tokenList.push_back(nextToken);
    
}

vector<Token> OwlLexer::tokenize() {
    vector<Token> tokenList;
        Token nextToken;
        while (sb.Char() != sb.EOFMarker()) {
            if (isalpha(sb.Char())) {
                    string word = extractFullWord();
                    nextToken.tokenval = handleKeywordOrId(word);
                    nextToken.stringval = word;
                    nextToken.lineno = sb.lineNumber();
                    if (!inComment)
                        tokenList.push_back(nextToken);
                    continue;
                } else if (isdigit(sb.Char())) {
                    Token num = extractFullNumber();
                    if (!inComment)
                        tokenList.push_back(num);
                    continue;
                } else { 
                    if (sb.Char() != ' ') {
                        nextToken = handleSpecials();
                        if (inComment == false && nextToken.tokenval != OPENCOMMENT)
                            tokenList.push_back(nextToken);
                        if (nextToken.tokenval == QUOTE) {
                            sb.GetChar();
                            processString(tokenList);
                        }
                        if (nextToken.tokenval == OPENCOMMENT) {
                            inComment = true;
                            sb.GetChar();
                        }
                        if (nextToken.tokenval == CLOSECOMMENT)
                            inComment = false;
                    }
        }
        sb.GetChar();
    }
    return tokenList;
}

#endif