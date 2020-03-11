#include "ast.h"
#include <QDebug>

QString AST::Type::convertType(AST::TYPE type)
{
    if(type == AST::TYPE::NATURAL) return "natural";
    else if(type == AST::TYPE::FILE) return "file";
    else if(type == AST::TYPE::CATALOGUE) return "catalogue";
    else if(type == AST::TYPE::URL) return "url";
}

AST::TYPE AST::Type::convertTypeQ(QString name)
{
    if(name == "natural") return AST::TYPE::NATURAL;
    else if(name == "file") return AST::TYPE::FILE;
    else if(name == "catalogue") return AST::TYPE::CATALOGUE;
    else if(name == "url") return AST::TYPE::URL;
}

QTreeWidgetItem* AST::VarDecl::print()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    QTreeWidgetItem* ident = new QTreeWidgetItem();
    item->setText(0,"VarDecl");
    ident->setText(0,id);
    item->addChild(type->print());
    item->addChild(ident);
    return item;
}

QString AST::VarDecl::eval(QMap<QString, QMap<QString, AST::TYPE>> &symbt)
{
    if(!symbt.contains(id))
    {
        QMap<QString, AST::TYPE> info;
        info.insert("",AST::Type::convertTypeQ(type->eval(symbt)));
        symbt.insert(id,info);
        return "";
    }

    throw InterpreterException("Variable is already declared!");
}

QTreeWidgetItem *AST::File::print()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,convertType(type));
    return item;
}

QString AST::File::eval(QMap<QString, QMap<QString, AST::TYPE> > &symbt)
{
    return AST::Type::convertType(type);
}

QTreeWidgetItem *AST::Natural::print()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,convertType(type));
    return item;
}

QString AST::Natural::eval(QMap<QString, QMap<QString, AST::TYPE> > &symbt)
{
    return AST::Type::convertType(type);
}

QTreeWidgetItem *AST::Catalogue::print()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,convertType(type));
    return item;
}

QString AST::Catalogue::eval(QMap<QString, QMap<QString, AST::TYPE> > &symbt)
{
    return AST::Type::convertType(type);
}

QTreeWidgetItem *AST::Url::print()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,convertType(type));
    return item;
}

QString AST::Url::eval(QMap<QString, QMap<QString, AST::TYPE> > &symbt)
{
    return AST::Type::convertType(type);
}


QTreeWidgetItem *AST::VarAssign::print()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,"=");
    item->addChild(var->print());
    item->addChild(rval->print());
    return item;
}

QString AST::VarAssign::eval(QMap<QString, QMap<QString, AST::TYPE> > &symbt)
{
    QString id = var->id;
    QMap<QString,AST::TYPE> info;
    if(symbt.contains(id))
    {
        AST::TYPE vartype = symbt[id].last();
        QString value = rval->eval(symbt);
        info.insert(value,vartype);

        bool checkForNumber;
        value.toInt(&checkForNumber);

        if(checkForNumber)
        {
            if(AST::TYPE::NATURAL != vartype) throw InterpreterException("Expected NATURAL but got URL!");
            symbt.insert(id,info);
            return "";
        }
        else
        {
            if(AST::TYPE::URL == vartype)
            {
                symbt.insert(id,info);
            }
            else
            {
                if(value.endsWith("/"))
                {
                    if(AST::TYPE::CATALOGUE != vartype) throw InterpreterException("Expected CATALOGUE but got FILE!");

                }
                else
                {
                    if(AST::TYPE::FILE != vartype) throw InterpreterException("Expected FILE but got CATALOGUE!");
                }

                symbt.insert(id,info);
            }

            return "";
        }
    }

    throw InterpreterException("Variable is not declarated!");
}

QTreeWidgetItem *AST::Variable::print()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,id);
    return item;
}

QString AST::Variable::eval(QMap<QString, QMap<QString, AST::TYPE> > &symbt)
{
    if(symbt.contains(id))
        return symbt[id].keys()[0];
    else{
        throw InterpreterException("Variable is not declarated!");
    }
}

QTreeWidgetItem *AST::UrlConst::print()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,value);
    return item;
}

QString AST::UrlConst::eval(QMap<QString, QMap<QString, AST::TYPE> > &symbt)
{
    return value;
}

QTreeWidgetItem *AST::NaturalConst::print()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,QString::number(Value));
    return item;
}

QString AST::NaturalConst::eval(QMap<QString, QMap<QString, AST::TYPE> > &symbt)
{
    return QString::number(Value);
}

QTreeWidgetItem *AST::UnOp::print()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,convertUnOpRev(op));
    item->addChild(expr->print());
    return item;
}

qint64 dirSize(QString dirPath)
{
    qint64 size = 0;
    QDir dir(dirPath);

    QDir::Filters fileFilters = QDir::Files|QDir::System|QDir::Hidden;
    for(QString filePath : dir.entryList(fileFilters)) {
        QFileInfo fi(dir, filePath);
        size+= fi.size();
    }

    QDir::Filters dirFilters = QDir::Dirs|QDir::NoDotAndDotDot|QDir::System|QDir::Hidden;
    const auto &list = dir.entryInfoList(dirFilters);
    for(const QFileInfo &dir : list) {
      size += dir.size();
      size+= dirSize(dirPath + "/" + dir.fileName());
    }

    return size;
}

QString AST::UnOp::eval(QMap<QString, QMap<QString, AST::TYPE> > &symbt){
    QString val = expr->eval(symbt);
    bool checkForNumber;
    val.toInt(&checkForNumber);
    if(checkForNumber)
    {
        throw InterpreterException("Invalid argument for UnOp");
    }
    bool isCatal = val.endsWith("/");
    if(op == AST::UNOP::DELETE)
    {
        if(isCatal){
            QDir dir(val);
            if(dir.exists())
            {
                dir.removeRecursively();
                return "1";
            }
            else{
                return "0";
            }
        }
        else{
            QFile file(val);
            if(file.exists())
            {
                file.remove();
                return "1";
            }
            else{
                return "0";
            }
        }
    }
    if(op == AST::UNOP::BACKUP)
    {
        if(isCatal)
            throw InterpreterException("Expected FILE in backup but get CATALOGUE");
        QFile file(val);
        if(file.exists())
        {
            QDir b(QDir::current());
            b.mkdir("backup");

            QFileInfo fileInfo(file.fileName());
            QString filename(fileInfo.fileName());
            qDebug() << QDir::currentPath() + "/backup/" + filename;
            file.copy(QDir::currentPath() + "/backup/" + filename);
            return "1";
        }
        else
            return "0";
    }
    if(op == AST::UNOP::SIZE)
    {
        if(isCatal){
            QDir dir(val);
            if(dir.exists())
            {
                return QString::number(dirSize(val));
            }
            else{
                return "0";
            }
        }
        else{
            QFile file(val);
            if(file.exists())
            {
                return QString::number(file.size());
            }
            else{
                return "0";
            }
        }
    }
}

QString AST::UnOp::convertUnOpRev(AST::UNOP qtype)
{
    if(qtype == AST::UNOP::BACKUP) return "backup";
    else if(qtype == AST::UNOP::DELETE) return "delete";
    else if(qtype == AST::UNOP::SIZE) return "size";
}

QTreeWidgetItem *AST::VarInit::print()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,"VarInit");
    item->addChild(decl->print());
    item->addChild(assign->print());
    return item;
}

QString AST::VarInit::eval(QMap<QString, QMap<QString, AST::TYPE> > &symbt)
{
    decl->eval(symbt);
    assign->eval(symbt);
    return "";
}

QTreeWidgetItem *AST::BinOp::print()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,op);
    item->addChild(left->print());
    item->addChild(right->print());
    return item;
}

QString AST::BinOp::eval(QMap<QString, QMap<QString, AST::TYPE> > &symbt)
{
    QString lval = left->eval(symbt);
    QString rval = right->eval(symbt);
    bool r,l;
    int ilval = lval.toInt(&l);
    int irval = rval.toInt(&r);
    if(!(l&&r))
    {
        if(op == "copy" && !l && !r){
            QFile file(lval);
            QDir dir(rval);
            if(!rval.endsWith("/") || lval.endsWith("/"))
                throw InterpreterException("Unexpected parameters for copy");
            if(file.exists() && dir.exists()){
                QFileInfo fileInfo(file.fileName());
                QString filename(fileInfo.fileName());
                qDebug() << dir.path()+filename;
                file.copy(dir.path()+"/"+filename);
                return "1";
            }
            else
                return "0";
        }
        else if(op == "find" && !l && !r){
            QFile file(lval);
            QDir dir(rval);
            if(!rval.endsWith("/") || lval.endsWith("/"))
                throw InterpreterException("Unexpected parameters for copy");
            if(file.exists() && dir.exists()){
                QFileInfo fileInfo(file.fileName());
                QString filename(fileInfo.fileName());

                QString targetStr = filename;
                   QFileInfoList hitList;
                   QString directory = rval+"/";
                   QDirIterator it(directory, QDirIterator::Subdirectories);


                   while (it.hasNext()) {
                       QString filename = it.next();
                       QFileInfo file(filename);

                       if (file.isDir()) {
                           continue;
                       }

                       if (file.fileName().contains(targetStr, Qt::CaseInsensitive)) {
                           hitList.append(file);
                       }
                   }
                   QString msg;
                   foreach (QFileInfo hit, hitList) {
                       msg.append(hit.absoluteFilePath()+"\n");
                   }
                   QMessageBox qmsg;
                   qmsg.setText(msg);
                   qmsg.exec();

                return "1";
            }
            else
                return "0";
        }
        else
            throw InterpreterException("Invalid type in BinOP");
    }
    if(op == ">")
    {
       if(ilval>irval)
           return "1";
       else
           return "0";
    }
    if(op == "<")
    {
       if(ilval<irval)
           return "1";
       else
           return "0";
    }
    if(op == "==")
    {
       if(ilval==irval)
           return "1";
       else
           return "0";
    }
    if(op == "+")
    {
       return QString::number(ilval+irval);
    }
    if(op == "-")
    {
       return QString::number(ilval-irval);
    }
    if(op == "="){
       AST::Variable* var = dynamic_cast<AST::Variable*>(left);
       AST::VarAssign assign(var,right);
       assign.eval(symbt);
       return "";
    }

    throw InterpreterException("Unknow operation");
}

QTreeWidgetItem *AST::IfStmt::print()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,"ifstmt");
    item->addChild(expr->print());
    item->addChild(body->print());
    if (elsestmt) item->addChild(elsestmt->print());
    return item;
}

QString AST::IfStmt::eval(QMap<QString, QMap<QString, AST::TYPE> > &symbt)
{
    if(expr->eval(symbt).toInt()){
        body->eval(symbt);
    }
    else if(elsestmt){
        elsestmt->eval(symbt);
    }
    return "";
}

QTreeWidgetItem *AST::Body::print()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,"Body");
    for(int i = 0; i < subtree.size();i++)
    {
        item->addChild(subtree[i]->print());
    }
    return item;
}

QString AST::Body::eval(QMap<QString, QMap<QString, AST::TYPE> > &symbt)
{
    while(!subtree.isEmpty())
    {
        subtree.takeFirst()->eval(symbt);
    }
    return "";
}

QTreeWidgetItem *AST::ElseStmt::print()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,"ElseStmt");
    item->addChild(elsebd->print());
    return item;
}

QString AST::ElseStmt::eval(QMap<QString, QMap<QString, AST::TYPE> > &symbt)
{
    elsebd->eval(symbt);
    return "";
}

QString AST::ForStmt::eval(QMap<QString, QMap<QString, AST::TYPE> > &symbt)
{
    init->eval(symbt);
    while(test->eval(symbt).toInt()){
        AST::Body tmp = *stmts;
        stmts->eval(symbt);
        *stmts = tmp;
        update->eval(symbt);
    }
    return "";
}

QTreeWidgetItem *AST::ForStmt::print()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,"ForStmt");
    item->addChild(init->print());
    item->addChild(test->print());
    item->addChild(update->print());
    item->addChild(stmts->print());

    return item;
}
