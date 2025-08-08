#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "trie.h"
#include "ui_mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnKey1_clicked();
    void on_btnKey2_clicked();
    void on_btnKey3_clicked();
    void on_btnKey4_clicked();
    void on_btnKey5_clicked();
    void on_btnKey6_clicked();
    void on_btnKey7_clicked();
    void on_btnKey8_clicked();
    void on_btnKey9_clicked();
    void on_btnExec_clicked();
    void on_cbOp_currentIndexChanged(int idx);



private:
    Ui::MainWindow *ui;
    Trie trie;
    const std::array<QString,10> digitMap = {
        "", "", "abc","def","ghi","jkl","mno","pqrs","tuv","wxyz"
    };
    void appendResult(const QString &s){
        ui->lwResult->addItem(s);
    }
    void clearResult(){
        ui->lwResult->clear();
    }
};
#endif // MAINWINDOW_H





