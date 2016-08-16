#include "widget.h"
#include "ui_widget.h"
#include <QStringList>
#include "time.h"
#include <QtAlgorithms>
#include <QFile>
#include "qdebug.h"

#define Acolor QColor(250, 191, 143)
#define Bcolor QColor(149, 179, 215)
#define Ncolor QColor(196, 215, 155)
#define Xcolor QColor(191, 191, 191)

const int WORDS = 666;
int trans[5][5] = {0, 1, 2, 3, 4,
                  5, 6, 7, 8, 9,
                  10, 11, 12, 13, 14,
                  15, 16, 17, 18, 19,
                  20, 21, 22, 23, 24};

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    QStringList header;
    header << "A" << "B" << "C" << "D" << "E";

    ui->gameTable->setRowCount(5);
    ui->gameTable->setColumnCount(5);
    ui->gameTable->setHorizontalHeaderLabels(header);
    ui->gameTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->gameTable->setSelectionMode(QAbstractItemView::SingleSelection);
    //ui->gameTable->setStyleSheet("QTableWidget::item{selection-background-color: transparent}");

    ui->spymasterTable->setRowCount(5);
    ui->spymasterTable->setColumnCount(5);
    ui->spymasterTable->setHorizontalHeaderLabels(header);
    ui->spymasterTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->spymasterTable->setSelectionMode(QAbstractItemView::NoSelection);

    truth[0][0] = truth[0][1] = truth[0][2] = truth[0][3] = truth[0][4] = 0;
    truth[1][0] = truth[1][1] = truth[1][2] = truth[1][3] = 0; // 0 for teamA
    truth[1][4] = truth[2][0] = truth[2][1] = truth[2][2] = truth[2][3] = truth[2][4] = 1;
    truth[3][0] = truth[3][1] = 1; // 1 for teamB
    truth[3][2] = truth[3][3] = truth[3][4] = 2;
    truth[4][0] = truth[4][1] = truth[4][2] = truth[4][3] = 2; // 2 for teamN
    truth[4][4] = 3; // 3 for X

    this->setWindowTitle("Codenames");
    this->setFixedSize(this->width(), this->height());
    srand((int)time(0));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::shuffle()
{
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++) {
            int x = rand() % 5;
            int y = rand() % 5;
            int temp = truth[i][j];
            truth[i][j] = truth[x][y];
            truth[x][y] = temp;
        }
}

void Widget::chooseWords()
{
    int i = 0;
    while (i < 25) {
        bool flag = false; // not having two same number
        wordsNumber[i] = rand() % WORDS;
        for (int j = 0; j < i; j++)
            if (wordsNumber[j] == wordsNumber[i]) {
                flag = true;
                break;
            }
        if (!flag) i++;
    }
    qSort(wordsNumber, wordsNumber + 25);

    QFile *file = new QFile(":/words/words");
    if (!file->open(QIODevice::ReadOnly|QIODevice::Text))
        qDebug() << "Can't open the file!" << endl;
    i = 0;
    int count = 0;
    while(i < 25) {
        QByteArray line = file->readLine();
        if (count == wordsNumber[i]) {
            words[i] = QString(line);
            words[i] = words[i].left(words[i].length() - 1);
            i++;
        }
        count++;
    }
    for (i = 0; i < 25; i++) {
        int j = rand() % 25;
        QString temp = words[i];
        words[i] = words[j];
        words[j] = temp;
    }
}

void Widget::on_newGameButton_clicked()
{
    shuffle();
    chooseWords();
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++) {
            QTableWidgetItem *gameItem = new QTableWidgetItem;
            QTableWidgetItem *spymasterItem = new QTableWidgetItem;
            if(truth[i][j] == 0) {
                spymasterItem->setBackgroundColor(Acolor);
                spymasterItem->setText("A ");
            }
            else if (truth[i][j] == 1) {
                spymasterItem->setBackgroundColor(Bcolor);
                spymasterItem->setText("B ");
            }
            else if (truth[i][j] == 2) {
                spymasterItem->setBackgroundColor(Ncolor);
                spymasterItem->setText("N ");
            }
            else {
                spymasterItem->setBackgroundColor(Xcolor);
                spymasterItem->setText("X ");
            }
            gameItem->setText(words[trans[i][j]]);
            spymasterItem->setText(spymasterItem->text() + words[trans[i][j]]);
            gameItem->setTextAlignment(Qt::AlignCenter);
            spymasterItem->setTextAlignment(Qt::AlignCenter);
            ui->gameTable->setItem(i, j, gameItem);
            ui->spymasterTable->setItem(i, j, spymasterItem);
        }
}

void Widget::on_markButton_clicked()
{
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
            if (ui->gameTable->item(i, j)->isSelected()) {
                QTableWidgetItem *item = ui->spymasterTable->item(i, j)->clone();
                ui->gameTable->setItem(i, j, item);
                return;
            }
}

void Widget::on_cleanButton_clicked()
{
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
            if (ui->gameTable->item(i, j)->isSelected()) {
                QTableWidgetItem *item = ui->gameTable->item(i, j)->clone();
                QString str = item->text();
                if (str[0] == 'A' || str[0] == 'B' || str[0] == 'N' || str[0] == 'X') {
                    item->setBackgroundColor(QColor(255, 255, 255));
                    item->setText(item->text().mid(2));
                    ui->gameTable->setItem(i, j, item);
                }
                return;
            }
}

void Widget::on_cleanAllButton_clicked()
{
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++) {
            QString str = ui->gameTable->item(i, j)->text();
            if (str[0] == 'A' || str[0] == 'B' || str[0] == 'N' || str[0] == 'X') {
                QTableWidgetItem *item = ui->gameTable->item(i, j)->clone();
                item->setBackgroundColor(QColor(255, 255, 255));
                item->setText(item->text().mid(2));
                ui->gameTable->setItem(i, j, item);
            }
        }
}

void Widget::on_outFocusButton_clicked()
{
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
            if (ui->gameTable->item(i, j)->isSelected()) {
                ui->gameTable->item(i, j)->setSelected(false);
                return;
            }
}
