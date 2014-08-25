#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMimeData>
#include <QMainWindow>
#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include <QApplication>
#include <QClipboard>
#include "windows.h"
//#pragma comment(lib,"user32.lib")

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void searchTimerOver();

    void killerTimerOver();


private:
    Ui::MainWindow *ui;

    QTimer *searchTimer;//timer for searching QtCreator window
    QTimer *killerTimer;//timer for auto closing this app
    double counter;     //after 10 minutes, we close app, if program didn't find QtCreator window
    QString originalText;
};

#endif // MAINWINDOW_H
