#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "interpreter.h"

QString lexer::getStringToken(){
    QVector<Token> tokens = getToken();
      QString text;
      for (int i = 0; i < tokens.size();i++)
      {
          text.append("<");
          if(tokens[i].tag == Tag::TYPE) text.append("TYPE,");
          else if(tokens[i].tag == Tag::KEYWORD) text.append("KEYWORD,");
          else if(tokens[i].tag == Tag::LITERAL) text.append("LITERAL,");
          else if(tokens[i].tag == Tag::COMMENTS) text.append("COMMENTS,");
          else if(tokens[i].tag == Tag::OPERATOR) text.append("OPERATOR,");
          else if(tokens[i].tag == Tag::IDENTIFIER) text.append("IDENTIFIER,");
          else if(tokens[i].tag == Tag::SEPARATORS) text.append("SEPARATORS,");
          text.append(tokens[i].data + "> ");
    }
    return text;
}

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
            pos++;
            continue;
        }
        if(text[pos] == ";" || text[pos] == "{" || text[pos] == "}" )
        {
            word.append(text[pos]);
            pos++;
            tokens.append(Token(word,Tag::SEPARATORS));
            continue;
        }
        if(text[pos] == "#")
        {
            while(text[pos] != "\n" && pos<text.length())
            {
                //word.append(text[pos]);
                pos++;
            }
            //tokens.append(Token(word,Tag::COMMENTS));
            continue;
        }
        if(text[pos] == "=" || text[pos] == ">" || text[pos] == "<")
        {
            if(text[pos] == "=" && text[pos+1] == "=")
            {
                tokens.append(Token("==",Tag::OPERATOR));
            }
            else
            {
                word.append(text[pos]);
                tokens.append(Token(word,Tag::OPERATOR));
            }
            pos++;
            continue;
        }
        else
        {
            //add exeption
            pos++;
        }
    }
    return tokens;
}


void Parser::CreateTree(){
   while(i<tokens.length())
   {
       if(tokens[i].tag==Tag::TYPE)
       {
           parseVarDecl();
       }
       if(tokens[i].tag==Tag::IDENTIFIER)
       {
           parseVarAsing();
       }
       else{
           //eror here
           i++;
       }
   }
}
vardecl* Parser::parseVarDecl(){

    vardecl* node = new vardecl();
    if(tokens[i + 1].tag == Tag::IDENTIFIER)
    {
        node->type = convertType(tokens[i].data);
        i++;
        node->id = parseId();
        if(node->id.isNull())
        {
            //error;
            return nullptr;
        }
        i++;
        return node;
    }
    else ; //error
}

QString Parser::parseId()
{
    if(tokens[i].data==";")
    {
        i++;
        return tokens[i].data;
    }
    return nullptr;
}
varasign* Parser::parseVarAsing(){
    ;
}

Type* Parser::convertType(QString type)
{
    if(type == "natural")
        return new Natural;
    if(type == "file")
        return new File;
    if(type == "catalog")
        return new Catalog;
    if(type == "url")
        return new Url;
}

QTreeWidgetItem *vardecl::print()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,"vardecl");
    QTreeWidgetItem* ch = new QTreeWidgetItem();
    item->setText(0,id);
    item->addChild(ch);

}

QTreeWidgetItem *Natural::print()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0,convertType(type));
        return item;
}

QTreeWidgetItem *Catalog::print()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0,convertType(type));
        return item;
}

QTreeWidgetItem *Url::print()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0,convertType(type));
        return item;
}

QTreeWidgetItem *File::print()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0,convertType(type));
        return item;
}

QString Type::convertType(type ttype)
{
    if(ttype == type::file)
        return "file";
    if(ttype == type::url)
        return "url";
    if(ttype == type::catalog)
        return "catalog";
    if(ttype == type::natural)
        return "natural";
}
