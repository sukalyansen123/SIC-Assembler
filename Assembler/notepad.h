#ifndef NOTEPAD_H
#define NOTEPAD_H

#include <QMainWindow>

namespace Ui {
class Notepad;
}

class Notepad : public QMainWindow
{
    Q_OBJECT

public:
    explicit Notepad(QWidget *parent = 0);
    ~Notepad();

private:
    Ui::Notepad *ui;

private slots:
    bool quit();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();

    void on_pushButton_clicked();

    void on_clear_clicked();
    void on_lineEdit_cursorPositionChanged(int arg1, int arg2);
    void on_lineEdit_1_cursorPositionChanged(int arg1, int arg2);
    void on_pushButton_2_clicked();
    void on_debug_clicked();
    void on_pushButton_3_clicked();
    void on_decimal_clicked();
    void on_pushButton_4_clicked();
};

#endif // NOTEPAD_H
