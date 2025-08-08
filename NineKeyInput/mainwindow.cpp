#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include<QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 启动时从 D:/dict.txt 加载字典
    const QString dictPath = QStringLiteral("D:/dict.txt");
    QFile check(dictPath);
    qDebug() << dictPath << "exists? " << check.exists();
    if (!trie.importFromTxt(dictPath.toStdString())) {
        ui->lwResult->addItem(QStringLiteral("加载字典失败: ") + dictPath);
        qWarning() << "加载字典失败:" << dictPath;
    } else {
        ui->lwResult->addItem(QStringLiteral("加载字典成功: ") + dictPath);
        qDebug() << "加载字典成功:" << dictPath;
    }
    // “添加”“修改”时启用次数输入
    ui->sbCount->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// —— 九键按钮槽 ——
void MainWindow::on_btnKey1_clicked() { ui->leInput->insert(QStringLiteral("1")); }
void MainWindow::on_btnKey2_clicked() { ui->leInput->insert(QStringLiteral("2")); }
void MainWindow::on_btnKey3_clicked() { ui->leInput->insert(QStringLiteral("3")); }
void MainWindow::on_btnKey4_clicked() { ui->leInput->insert(QStringLiteral("4")); }
void MainWindow::on_btnKey5_clicked() { ui->leInput->insert(QStringLiteral("5")); }
void MainWindow::on_btnKey6_clicked() { ui->leInput->insert(QStringLiteral("6")); }
void MainWindow::on_btnKey7_clicked() { ui->leInput->insert(QStringLiteral("7")); }
void MainWindow::on_btnKey8_clicked() { ui->leInput->insert(QStringLiteral("8")); }
void MainWindow::on_btnKey9_clicked() { ui->leInput->insert(QStringLiteral("9")); }

// —— 下拉框切换槽 ——
void MainWindow::on_cbOp_currentIndexChanged(int idx)
{
    ui->sbCount->setEnabled(idx == 0 || idx == 2);
}

// —— 执行按钮槽 ——
void MainWindow::on_btnExec_clicked()
{
    ui->lwResult->clear();
    const QString op = ui->cbOp->currentText();
    const QString in = ui->leInput->text().trimmed();
    const int cnt    = ui->sbCount->value();

    if (op == QStringLiteral("添加")) {
        if (cnt <= 0) {
            ui->lwResult->addItem(QStringLiteral("添加失败，请输入合法字符串对应次数"));
        } else {
            int oldCnt = trie.getCount(in.toStdString());
            if (oldCnt > 0) {
                ui->lwResult->addItem(
                    in + QStringLiteral(" 存在，已输入次数为 ") +
                    QString::number(oldCnt) + QStringLiteral(" 次")
                    );
            } else {
                trie.insert(in.toStdString(), cnt);
                ui->lwResult->addItem(QStringLiteral("添加成功"));
            }
        }
    }
    else if (op == QStringLiteral("删除")) {
        if (trie.remove(in.toStdString()))
            ui->lwResult->addItem(QStringLiteral("删除 ") + in + QStringLiteral(" 成功"));
        else
            ui->lwResult->addItem(in + QStringLiteral("不存在"));
    }
    else if (op == QStringLiteral("修改")) {
        if (cnt <= 0) {
            ui->lwResult->addItem(QStringLiteral("修改失败，请输入合法字符串对应次数"));
        } else {
            int oldCnt = trie.getCount(in.toStdString());
            if (oldCnt == 0) {
                ui->lwResult->addItem(in + QStringLiteral(" 不存在"));
            } else {
                trie.update(in.toStdString(), cnt);
                ui->lwResult->addItem(QStringLiteral("修改成功"));
            }
        }
    }
    else if (op == QStringLiteral("数字查询")) {
        std::vector<std::string> steps;
        trie.digitQuery(in.toStdString(), steps);
        if (steps.empty()) {
            ui->lwResult->addItem(
                QStringLiteral("请重新输入合法字符串或数字序列")
                );
        } else {
            for (const auto &s : steps)
                ui->lwResult->addItem(QString::fromStdString(s));
        }
    }
    else if (op == QStringLiteral("正则查询")) {
        auto res = trie.regexSearch(in.toStdString());
        if (res.empty()) {
            ui->lwResult->addItem(QString());
        } else {
            for (const auto &w : res)
                ui->lwResult->addItem(QString::fromStdString(w));
        }
    }
    else if (op == QStringLiteral("前缀查询")) {
        auto res = trie.prefixSearch(in.toStdString());
        if (res.empty()) {
            ui->lwResult->addItem(QString());
        } else {
            for (const auto &p : res)
                ui->lwResult->addItem(
                    QString::fromStdString(p.first) + QStringLiteral(":") +
                    QString::number(p.second)
                    );
        }
    }
    else if (op == QStringLiteral("自动校正")) {
        auto res = trie.fuzzySearch(in.toStdString());
        if (res.empty()) {
            ui->lwResult->addItem(QStringLiteral("无匹配单词"));
        } else {
            for (auto &p : res) {

                ui->lwResult->addItem(QString::fromStdString(p.first));
            }
        }
    }
}
