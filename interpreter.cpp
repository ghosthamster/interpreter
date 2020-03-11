#include "interpreter.h"

Token::Token(Tag _tag, QString Value)
{
    tag = _tag;
    value = Value;
}

Lexer::Lexer(QString code)
{
    QVector<QString> keywords;
    QVector<QString> type;
    QVector<QString> operatorsb;
    QVector<QString> operatorsu;

    keywords << "for" << "if" << "else";
    type << "natural" << "url" << "file" << "catalogue";
    operatorsb << ">" << "<" << "==" << "=" << "+" << "-" << "copy" << "find" ;
    operatorsu << "delete" << "backup" << "size" ;

    for(int i = 0; i < keywords.size(); ++i){ words.insert(keywords[i],Token(Tag::KEYWORD,keywords[i])); }
    for(int i = 0; i < type.size(); ++i){ words.insert(type[i],Token(Tag::TYPE,type[i])); }
    for(int i = 0; i < operatorsb.size(); ++i){ words.insert(operatorsb[i],Token(Tag::OPERATORB,operatorsb[i])); }
    for(int i = 0; i < operatorsu.size(); ++i){ words.insert(operatorsu[i],Token(Tag::OPERATORU,operatorsu[i])); }

    text = code;
}

QVector<Token> Lexer::get_tokens()
{
    QVector<Token> tokens;
    QString word;
    pos = 0;
    while(pos < text.size())
    {
        word = "";

        if(text[pos].isSpace() || text[pos] == "\n")
        {
            pos++;
            continue;
        }

        if(text[pos].isLetter())
        {
            while (text[pos].isLetterOrNumber())
            {
                word.append(text[pos]);
                pos++;
            }

            tokens.append(words.value(word,Token(Tag::IDENTIFIER,word)));
            continue;
        }

        if(text[pos].isDigit())
        {
            while (text[pos].isDigit())
            {
                word.append(text[pos]);
                pos++;
            }

            tokens.append(Token(Tag::LITERAL,word));
            continue;
        }

        if(text[pos] == "\"")
        {
            do
            {
                word.append(text[pos]);
                pos++;
            }while (text[pos] != "\"" && pos < text.size());

            pos++;
            word.remove(0,1);

            tokens.append(Token(Tag::LITERAL,word));
            continue;
        }

        if(text[pos] == ";" || text[pos] == "{" || text[pos] == "}")
        {
            word.append(text[pos]);
            pos++;
            tokens.append(Token(Tag::SEPARATORS,word));
            continue;
        }

        if(text[pos] == ">" || text[pos] == "<" || text[pos] == "=" || text[pos] == "+" || text[pos] == "-")
        {
            if(text[pos] == '=' && text[pos+1] == '=') { word.append('='); pos++;}
            word.append(text[pos]);
            tokens.append(Token(Tag::OPERATORB,word));
            pos++;
            continue;
        }
        else
        {
            InterpreterException exception("Unknown symbol has been found!\nPosition: " + QString::number(pos) + "\nSymbol: " + text[pos]);
            exception.raise();
        }
    }
    return tokens;
}

QList<AST::Node*> Parser::createTree(bool isBody)
{
    QList<AST::Node*> tree;
    tree.clear();
    while(tokenstream.size())
    {
        current = tokenstream.takeFirst();
        if(current.tag == Tag::TYPE)
        {
            if((tokenstream.size()) > 2 && tokenstream[1].value == '=') tree.push_back(parseInit());
            else tree.push_back(parseVarDecl());
            NotEmptyAssign("Expected ';' at the end of stmt");
            if(current.value != ';') throw InterpreterException("Expected ';' at the end of stmt");
        }
        else if(current.tag == Tag::IDENTIFIER)
        {
            tree.push_back(parseVarAssign());
            NotEmptyAssign("Expected ';' at the end of stmt");
            if(current.value != ';') throw InterpreterException("Expected ';' at the end of stmt");
        }
        else if(current.value == "if")
        {
            tree.push_back(parseIf());
        }
        else if(current.value == "for")
        {
            tree.push_back(parseFor());
        }
        else if(current.value == "}")
        {
            break;
        }
        else if (current.tag == Tag::OPERATORU)
        {
            tree.push_back(parseUnOp());
            if(current.value != ';') throw InterpreterException("Expected ';' at the end of stmt");
        }

    }

    return tree;
}

AST::VarDecl* Parser::parseVarDecl()
{
    AST::VarDecl* node = new AST::VarDecl();
    node->type = convertType(current.value);
    NotEmptyAssign("Missing ID after TYPE");
    if(current.tag == Tag::IDENTIFIER)
    {
        node->id = current.value;
        return node;
    }
    throw InterpreterException("Missing ID after Type");
}

AST::VarAssign* Parser::parseVarAssign()
{
    AST::VarAssign* node = new AST::VarAssign();
    node->var = new AST::Variable(current.value);
    NotEmptyAssign("Missing '=' after ID");
    if(current.value == "=")
    {
        node->rval = parseExpr();
        return node;
    }
    throw InterpreterException("Missing '=' after ID");
}

AST::VarInit *Parser::parseInit()
{
    AST::VarInit* node = new AST::VarInit();
    node->decl = parseVarDecl();
    node->assign = parseVarAssign();
    return node;
}

AST::Expr *Parser::parseExpr()
{
    NotEmptyAssign("Expected expression");
    if(!tokenstream.isEmpty() && (tokenstream.first().tag == Tag::OPERATORB))
    {
        AST::Expr* expr = nullptr;
        QString op = tokenstream.takeFirst().value;
        tokenstream.push_front(current);
        expr = parseBinOp(op);
        return expr;
    }
    else if(current.tag == Tag::IDENTIFIER) return new AST::Variable(current.value);
    else if(current.tag == Tag::LITERAL) return  parseConst();
    else if(current.tag == Tag::OPERATORU) return  parseUnOp();

    throw InterpreterException("Expected expression");
}

AST::BinOp *Parser::parseBinOp(QString oper)
{
    AST::BinOp *node = new AST::BinOp();
    node->left = parseExpr();
    node->op = oper;
    node->right = parseExpr();
    return node;
}

AST::Body *Parser::parseBody()
{
    AST::Body* node = new AST::Body();
    NotEmptyAssign("Expected '{' at body start");
    if(current.value == '{')
    {
        node->subtree = createTree(true);
        if(current.value == '}') return node;
        throw InterpreterException("Expected '}' at body end");
    }
    throw InterpreterException("Expected '{' at body start");
}

AST::IfStmt *Parser::parseIf()
{
    AST::IfStmt* node = new AST::IfStmt();
    node->expr = parseExpr();
    node->body = parseBody();

    if(!tokenstream.isEmpty() && tokenstream.front().value == "else")
    {
        tokenstream.pop_front();
        node->elsestmt = new AST::ElseStmt();
        node->elsestmt->elsebd = parseBody();
    }
    return node;
}

AST::ForStmt *Parser::parseFor()
{
    AST::ForStmt* node = new AST::ForStmt();
    NotEmptyAssign("Expected Initialization or Assigment in FOR stmt");
    if(current.tag == Tag::TYPE) node->init = parseInit();
    else if(current.tag == Tag::IDENTIFIER) node->init = parseVarAssign();
    node->test = parseExpr();
    node->update = parseExpr();
    node->stmts = parseBody();
    return node;
}

AST::Constant *Parser::parseConst()
{
    QRegExp re("\d");
    if(re.exactMatch(current.value)) return new AST::NaturalConst(current.value.toInt());
    return new AST::UrlConst(current.value);
}

AST::UnOp *Parser::parseUnOp()
{
    AST::UnOp* node = new AST::UnOp();
    node->op = convertUnOp(current.value);
    node->expr = parseExpr();
    return node;
}

void Parser::printTree(QList<AST::Node*> tree,QTreeWidget *trwdg)
{
    trwdg->clear();
    while(!tree.isEmpty())
    {
        trwdg->addTopLevelItem(tree.first()->print());
        tree.pop_front();
    }
}

AST::Type *Parser::convertType(QString qtype)
{
    if(qtype == "natural") return new AST::Natural();
    else if(qtype == "file") return  new AST::File();
    else if(qtype == "catalogue") return new AST::Catalogue;
    else if(qtype == "url") return new AST::Url();
}

AST::UNOP Parser::convertUnOp(QString qtype)
{
    if(qtype == "backup") return AST::UNOP::BACKUP;
    else if(qtype == "delete") return  AST::UNOP::DELETE;
    else if(qtype == "size") return AST::UNOP::SIZE;
}

void Parser::NotEmptyAssign(QString err)
{
    if (tokenstream.isEmpty()) throw InterpreterException(err);
    current = tokenstream.takeFirst();
}

Parser::Parser(QVector<Token> tokens)
{
    tokenstream = tokens;
}


void Interpreter::Evaluate()
{
    while(!tree.isEmpty())
    {
        tree.takeFirst()->eval(symbtable);
    }

}
