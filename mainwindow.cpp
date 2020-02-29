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


void MainWindow::on_run_clicked()
{
    lexer A(ui->code->toPlainText());
    Parser B(A.getToken());
    ui->result->setText(A.getStringToken());
    //ui->result->setText(ui->result->toPlainText()+B.tree[0].print);
}
