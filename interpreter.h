#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <QString>
#include <QVector>
#include <QMap>
enum class Tag {
                IDENTIFIER,
                KEYWORD,
                SEPARATORS,
                OPERATOR,
                LITERAL,
                COMMENTS,
                TYPE
};

class Token{
public:
    Token(QString DATA,Tag TAG){data = DATA; tag = TAG;}
    QString data;
    Tag tag;
};

class lexer{
    QString text;
    QMap<QString,Token> tags;
    char symb;
    int pos;
public:
    QVector<Token> getToken();
    QString getStringToken();
    lexer(QString c){text = c;
                     QVector<QString> keyword;
                     keyword<<"if"<<"else"<<"step"<<"in"<<"for"<<"to"<<"run"<<"delete"<<"findAllFiles"
                           <<"findAllCatalogues"<<"backup"<<"copy";
                     QVector<QString> Operator;
                     Operator<<"="<<">"<<"<"<<"==";
                     QVector<QString> type;
                     type<<"natural"<<"file"<<"catalogue"<<"url";
                     for(int i = 0; i < keyword.size();++i)
                     {
                        tags.insert(keyword[i],Token(keyword[i],Tag::KEYWORD));
                     }
                     for(int i = 0; i < Operator.size();++i)
                     {
                        tags.insert(Operator[i],Token(Operator[i],Tag::OPERATOR));
                     }
                     for(int i = 0; i < type.size();++i)
                     {
                        tags.insert(type[i],Token(type[i],Tag::TYPE));
                     }
                     tags.insert(";",Token(";",Tag::SEPARATORS));
                     tags.insert("#",Token("#",Tag::COMMENTS));
                    }
};





#endif // INTERPRETER_H
