#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    try
    {
        Lexer lexer(ui->textEdit->toPlainText());
        QVector<Token> tokens = lexer.get_tokens();
        QString text;
        for (int i = 0; i < tokens.size();i++)
        {
            text.append("<");
            if(tokens[i].tag == Tag::TYPE) text.append("TYPE,");
            else if(tokens[i].tag == Tag::KEYWORD) text.append("KEYWORD,");
            else if(tokens[i].tag == Tag::LITERAL) text.append("LITERAL,");
            else if(tokens[i].tag == Tag::OPERATORU) text.append("OPERATORU,");
            else if(tokens[i].tag == Tag::OPERATORB) text.append("OPERATORB,");
            else if(tokens[i].tag == Tag::IDENTIFIER) text.append("IDENTIFIER,");
            else if(tokens[i].tag == Tag::SEPARATORS) text.append("SEPARATORS,");
            text.append(tokens[i].value + "> ");
        }
        ui->textEdit_2->setText(text);

        Parser parser(tokens);
        QList<AST::Node*> tree = parser.createTree(false);

        parser.printTree(tree,ui->treeWidget);

        Interpreter inter(tree);
        inter.Evaluate();

        ui->tableWidget->setRowCount(inter.symbtable.size());

        for (int i = 0; i < inter.symbtable.size();++i)
        {
            for (int j = 0;j < 3;++j)
            {
                ui->tableWidget->setItem(i,j,new QTableWidgetItem(""));
            }
        }

        QList<QString> keys = inter.symbtable.keys();

        for (int i = 0; i < inter.symbtable.size();++i)
        {
            ui->tableWidget->item(i,0)->setText(keys[i]);
            ui->tableWidget->item(i,1)->setText(inter.symbtable[keys[i]].lastKey());
            ui->tableWidget->item(i,2)->setText(AST::Type::convertType(inter.symbtable[keys[i]].last()));
        }

    }
    catch (InterpreterException &tokenexcept)
    {
        QMessageBox box;
        box.setText(tokenexcept.What());
        box.exec();
    }

}
