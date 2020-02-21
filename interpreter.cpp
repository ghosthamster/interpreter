#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "interpreter.h"

QVector<Token> lexer::getToken(){
    pos = 0;
    QString word;
    QVector<Token> tokens;
    while(pos<text.length())
    {
        word = "";
        if(text[pos].isSpace())
        {
            pos++;
        }
        if(text[pos].isLetter())
        {
            while(text[pos].isLetterOrNumber())
            {
                word.append(text[pos]);
                pos++;
            }
            tokens.append(tags.value(word,Token(word,Tag::IDENTIFIER)));
            continue;
        }
        if(text[pos].isDigit())
        {
            while(text[pos].isDigit())
            {
                word.append(text[pos]);
                pos++;
            }
            tokens.append(Token(word,Tag::LITERAL));
            continue;
        }
        if(text[pos] == "\"")
        {
            do
            {
                word.append(text[pos]);
                pos++;
            }while(text[pos] != "\"" && pos<text.length());
            word.append("\"");
            tokens.append(Token(word,Tag::LITERAL));
            continue;
        }
        if(text[pos] == ";")
        {
            pos++;
            tokens.append(Token(";",Tag::SEPARATOR));
            continue;
        }
        if(text[pos] == "#")
        {
            while(text[pos] != "\n" && pos<text.length())
            {
                word.append(text[pos]);
                pos++;
            }
            tokens.append(Token(word,Tag::LITERAL));
            continue;
        }
    }

}
