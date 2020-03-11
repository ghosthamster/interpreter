#ifndef AST_H
#define AST_H

#include <QTreeWidgetItem>
#include <QException>
#include <QDir>
#include <QFile>
#include <QDirIterator>
#include <QMessageBox>

class InterpreterException:public QException
{
private:
    QString Msg;
public:
    void raise() const override {throw *this;}
    InterpreterException* clone() const override {return new InterpreterException(*this);}
    InterpreterException(QString msg):Msg{msg}{}
    QString What(){return Msg;}

};

namespace AST {
    enum class TYPE {FILE,CATALOGUE,URL,NATURAL};
    enum class UNOP {DELETE,BACKUP, SIZE};

    class Node
    {
    public:
        virtual QTreeWidgetItem* print() = 0;
        virtual QString eval(QMap<QString ,QMap<QString , AST::TYPE>>& symbt) = 0;
        virtual ~Node(){}
    };

    class Expr:public Node
    {
    public:
        QTreeWidgetItem * print(){}
        QString eval(QMap<QString ,QMap<QString , AST::TYPE>>& symbt){}
        ~Expr(){}
    };

    class Stmt:public Node
    {
    public:
        QTreeWidgetItem* print(){}
        QString eval(QMap<QString ,QMap<QString , AST::TYPE>>& symbt){}
        ~Stmt(){}
    };

    class Body:public Node
    { 
    public:
        QList<Node*> subtree;
        QTreeWidgetItem* print();
        QString eval(QMap<QString ,QMap<QString , AST::TYPE>>& symbt);
        ~Body(){}
    };

    class Type:public Node
    {
    public:
        static QString convertType(AST::TYPE type);
        static AST::TYPE convertTypeQ(QString);
        QTreeWidgetItem* print(){}
        QString eval(QMap<QString ,QMap<QString , AST::TYPE>>& symbt){}
        ~Type(){}
    };

    class Variable:public Expr
    {
    public:
        QString id;
        QTreeWidgetItem* print();
        QString eval(QMap<QString ,QMap<QString , AST::TYPE>>& symbt);
        Variable(QString ident):id{ident}{}
        Variable(){}
        ~Variable(){}
    };

    class VarDecl:public Node
    {
    public:
        QTreeWidgetItem* print();
        QString id;
        Type* type;
        QString eval(QMap<QString ,QMap<QString , AST::TYPE>>& symbt);
        VarDecl(QString ID, Type* TYpe):id{ID},type{TYpe}{}
        VarDecl(){}
        ~VarDecl(){}
    };

    class VarAssign:public Stmt
    {
    public:
        Variable* var;
        Expr* rval;
        QTreeWidgetItem *print();
        QString eval(QMap<QString ,QMap<QString , AST::TYPE>>& symbt);
        VarAssign(Variable* Var, Expr* Rval):var{Var},rval{Rval}{}
        VarAssign(){}
        ~VarAssign(){}
    };

    class VarInit:public Node
    {
    public:
        VarDecl* decl;
        VarAssign* assign;
        QTreeWidgetItem * print();
        QString eval(QMap<QString ,QMap<QString , AST::TYPE>>& symbt);
        VarInit(VarDecl* Decl, VarAssign* Assign):decl{Decl},assign{Assign}{}
        VarInit(){}
        ~VarInit(){}
    };

    class BinOp:public Expr
    {
    public:
        Expr* left;
        QString op;
        Expr* right;
        QTreeWidgetItem * print();
        QString eval(QMap<QString ,QMap<QString , AST::TYPE>>& symbt);
        BinOp(Expr* Left, QString Op, Expr* Right):left{Left},op{Op},right{Right}{}
        BinOp(){}
        ~BinOp(){}
    };

    class UnOp:public Expr
    {
    public:
        AST::UNOP op;
        Expr* expr;
        QTreeWidgetItem * print();
        QString eval(QMap<QString ,QMap<QString , AST::TYPE>>& symbt);
        QString convertUnOpRev(AST::UNOP qtype);
        UnOp(AST::UNOP Op, Expr* Expression):op{Op},expr{Expression}{}
        UnOp(){}
        ~UnOp(){}
    };

    class Constant:public Expr
    {
    public:
        QTreeWidgetItem* print(){}
        QString eval(QMap<QString ,QMap<QString , AST::TYPE>>& symbt){}
        ~Constant(){}
    };

    class ElseStmt:public Stmt
    {
    public:
        Body* elsebd;
        QTreeWidgetItem * print();
        QString eval(QMap<QString ,QMap<QString , AST::TYPE>>& symbt);
        ElseStmt(Body* ElseBD):elsebd{ElseBD}{}
        ElseStmt(){}
        ~ElseStmt(){}
    };

    class IfStmt:public Stmt
    {
    public:
        Expr* expr;
        Body* body;
        ElseStmt* elsestmt = nullptr;
        QTreeWidgetItem * print();
        QString eval(QMap<QString ,QMap<QString , AST::TYPE>>& symbt);
        IfStmt(Expr* expression,Body* Body):expr{expression},body{Body}{}
        IfStmt(){}
        ~IfStmt(){}
    };

    class ForStmt:public Stmt
    {
    public:
        Node* init;
        Expr* test;
        Expr* update;
        Body* stmts;
        QString eval(QMap<QString ,QMap<QString , AST::TYPE>>& symbt);
        QTreeWidgetItem * print();
        ForStmt(){}
        ~ForStmt(){}
    };

    class File:public Type
    {
    public:
        TYPE type = TYPE::FILE;
        QTreeWidgetItem* print();
        QString eval(QMap<QString ,QMap<QString , AST::TYPE>>& symbt);
        ~File(){}
    };

    class Catalogue:public Type
    {
    public:
        TYPE type = TYPE::CATALOGUE;
        QTreeWidgetItem* print();
        QString eval(QMap<QString ,QMap<QString , AST::TYPE>>& symbt);
        ~Catalogue(){}
    };

    class Natural:public Type
    {
    public:
        TYPE type = TYPE::NATURAL;
        QTreeWidgetItem* print();
        QString eval(QMap<QString ,QMap<QString , AST::TYPE>>& symbt);
        ~Natural(){}
    };

    class Url:public Type
    {
    public:
        TYPE type = TYPE::URL;
        QTreeWidgetItem* print();
        QString eval(QMap<QString ,QMap<QString , AST::TYPE>>& symbt);
        ~Url(){}
    };

    class NaturalConst:public Constant
    {
    public:
        int Value;
        TYPE type = TYPE::NATURAL;
        QTreeWidgetItem* print();
        QString eval(QMap<QString ,QMap<QString , AST::TYPE>>& symbt);
        NaturalConst(int val):Value{val}{}
        NaturalConst(){}
        ~NaturalConst(){}
    };

    class UrlConst:public Constant
    {
    public:
        QString value;
        TYPE type = TYPE::URL;
        QTreeWidgetItem* print();
        QString eval(QMap<QString ,QMap<QString , AST::TYPE>>& symbt);
        UrlConst(QString val):value{val}{}
        UrlConst(){}
        ~UrlConst(){}
    };
}

#endif // AST_H
