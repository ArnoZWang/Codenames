#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_newGameButton_clicked();
    void on_markButton_clicked();
    void on_cleanButton_clicked();
    void on_cleanAllButton_clicked();
    void on_outFocusButton_clicked();

private:
    Ui::Widget *ui;
    int truth[5][5];
    void shuffle();
    void chooseWords();
    int wordsNumber[25];
    QString words[25];
};

#endif // WIDGET_H
