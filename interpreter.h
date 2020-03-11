#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <QString>
#include <QMap>
#include <QVector>
#include "ast.h"

enum class Tag {IDENTIFIER,KEYWORD,TYPE,LITERAL,OPERATORB,SEPARATORS,OPERATORU};

class Token
{
public:
    Tag tag;
    QString value;
    Token(Tag _tag, QString Value);
    Token(){}
    ~Token(){}
};

class Lexer
{
private:
    QString text;
    QMap<QString,Token> words;
    char current_sym;
    int pos;

public:
    Lexer(QString code);
    QVector<Token> get_tokens();
};

class Parser
{
public:
    QVector<Token> tokenstream;
    Token current;

    QList<AST::Node*> createTree(bool isBody);
    AST::VarDecl* parseVarDecl();
    AST::VarAssign* parseVarAssign();
    QString parseId();
    AST::VarInit* parseInit();
    AST::Expr* parseExpr();
    AST::Constant *parseConst();
    AST::UnOp* parseUnOp();
    static AST::Type* convertType(QString qtype);
    static AST::UNOP convertUnOp(QString qtype);
    AST::BinOp* parseBinOp(QString oper);
    AST::Body* parseBody();
    AST::IfStmt* parseIf();
    AST::ForStmt* parseFor();
    void NotEmptyAssign(QString err);
    void printTree(QList<AST::Node*> tree,QTreeWidget *trwdg);
    Parser(QVector<Token> tokens);
};

class Interpreter
{
public:
    QList<AST::Node*> tree;
    QMap<QString ,QMap<QString , AST::TYPE>> symbtable;
    void Evaluate();
    Interpreter(QList<AST::Node*> Tree):tree{Tree}{}
    ~Interpreter(){}
};

#endif // INTERPRETER_H
