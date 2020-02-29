#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <QString>
#include <QVector>
#include <QMap>
#include <QList>
#include <QTreeWidgetItem>

enum class Tag {
                IDENTIFIER,
                KEYWORD,
                SEPARATORS,
                OPERATOR,
                LITERAL,
                COMMENTS,
                TYPE,
                BODY
};

enum type{
  file,
  natural,
  url,
  catalog
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
                     tags.insert("{",Token("{",Tag::SEPARATORS));
                     tags.insert("}",Token("}",Tag::SEPARATORS));
                     tags.insert("#",Token("#",Tag::COMMENTS));
                    }
};


 class Node{
 public:
     virtual QTreeWidgetItem* print();
     virtual ~Node(){}
 };
 class Type:public Node{
   public:
     QString convertType(type ttype);
 };
 class File:public Type{
 public:
     QTreeWidgetItem* print();
     type type = type::file;
     ~File(){}
 };
 class Natural:public Type{
 public:
     type type = type::natural;
     QTreeWidgetItem* print();
     ~Natural(){}
 };
 class Catalog:public Type{
 public:
     type type = type::catalog;
     QTreeWidgetItem* print();
     ~Catalog(){}
 };
 class Url:public Type{
 public:
     type type = type::url;
     QTreeWidgetItem* print();
     ~Url(){}
 };
 class Exp:public Node{

 };
 class Bin:public Exp{
 public:
     QTreeWidgetItem* print();
     ~Bin(){}
 };
 class Un:public Exp{
 public:
     QTreeWidgetItem* print();
     ~Un(){}
 };
 class stmt:public Node{

 };
 class ifstmt:public stmt{
 public:
     QTreeWidgetItem* print();
     ~ifstmt(){}
 };
 class forstmt:public stmt{
 public:
     QTreeWidgetItem* print();
     ~forstmt(){}
 };
 class Variable:public Exp{
    public:
     QTreeWidgetItem* print();
     Type type;
     QString value;
     ~Variable(){}
 };
 class constanta:public Variable{
 public:
     ~constanta(){}
 };
 class vardecl:public Node{
 public:
     QTreeWidgetItem *print();
     QString id;
     Type* type;
     ~vardecl(){}
 };
 class varasign:public Node{
    public:
    QTreeWidgetItem* print();
    Exp rvalue;
    Variable id;
    ~varasign(){}
 };
 class body:public Node{
 public:
     QTreeWidgetItem* print();
     ~body(){}
 };



class Parser{
public:
    Parser(QVector<Token> token){tokens = token; i = 0;}
    void CreateTree();
    vardecl* parseVarDecl();
    varasign* parseVarAsing();
    QString parseId();
    Type* convertType(QString type);
    QVector<Token> tokens;
    int i;
    QList<Node*> tree;
    Node* parseIdent();
};

#endif // INTERPRETER_H
