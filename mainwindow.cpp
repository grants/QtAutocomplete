#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QElapsedTimer>
#include <QMessageBox>
//#include "windows.h"
//#pragma comment(lib,"user32.lib")

HHOOK hHook;
HWND hwnd;
bool wasEnter;
LRESULT CALLBACK MyLowLevelKeyBoardProc(int nCode, WPARAM wParam, LPARAM lParam);
//HINSTANCE hModule;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    searchTimer = new QTimer(this);
    counter = 0;
    connect(searchTimer,SIGNAL(timeout()),this,SLOT(searchTimerOver()));
    searchTimer->start(2500);



/*//    hModule = NULL;
//    hModule = LoadLibraryA("kernel32.dll");
//    if(hModule == NULL)
//        QMessageBox::information(0, "Information", "Failed with code "+QString::number(GetLastError()));
//    else
//        QMessageBox::information(0, "Information", "DLL loaded");

//    HINSTANCE hinst = GetModuleHandle( NULL );

//    hHook = NULL;
//    hHook = SetWindowsHookEx(WH_KEYBOARD_LL,MyLowLevelKeyBoardProc,hinst,0);
//    if(hHook == NULL)
//        QMessageBox::information(0, "Information", "Failed with code "+QString::number(GetLastError()));*/

    wasEnter = false;


}

MainWindow::~MainWindow()
{
    delete ui;
    if(hHook != NULL)
    {
        if(UnhookWindowsHookEx(hHook))
            qDebug() << "destroy";
    }
}

void MainWindow::searchTimerOver()
{
    counter += 2.5;
    //get the descriptor of current window
    char title[256];
    HWND h = GetForegroundWindow();
    GetWindowTextA(GetForegroundWindow(),title,sizeof(title));
    QString string(title);
    //qDebug() << title << h << string.contains("Qt Creator",Qt::CaseSensitive);


    //very important!
    //install all things
    if(string.contains("Qt Creator",Qt::CaseSensitive))
    {
        qDebug() << "I FIND!";

        hwnd = h;//save in memory our descriptor

        //install hook
        HINSTANCE hinst = GetModuleHandle( NULL );
        hHook = NULL;
        hHook = SetWindowsHookEx(WH_KEYBOARD_LL,MyLowLevelKeyBoardProc,hinst,0);
        if(hHook == NULL)//if not succesfull, exit
        {
            QMessageBox::information(0, "Error", "Hook cannot be installed. Failed with code "+QString::number(GetLastError()));
            QCoreApplication::quit();
        }


        //start timer which should close app when we don't use Qt Creator
        killerTimer = new QTimer(this);
        connect(killerTimer,SIGNAL(timeout()),this,SLOT(killerTimerOver()));
        killerTimer->start(300 * 1000);//every 300 sec = 5 min we will check existing of our window

        counter = 0;
        //delete this timer
        searchTimer->stop();
        searchTimer->deleteLater();
    }



    if(counter == 600)//600 seconds equals 10 minutes, we close app and report user
    {
        QApplication::beep();
        searchTimer->stop();
        //QMessageBox::information(this, tr("Closing"), tr("Quit"));
        //it is commented, because I think,that application should close without MessageBox
        //user will hear 1 beep and application closing automaticaly

        QCoreApplication::quit();

    }
}

void MainWindow::killerTimerOver()
{
    if(!IsWindow(hwnd))//if descriptor of Qt Creator not exist, exit app
    {
        QApplication::beep();
        QCoreApplication::quit();
    }
}



LRESULT CALLBACK MyLowLevelKeyBoardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
//    QElapsedTimer timer;
//    timer.start();
if(hwnd == GetForegroundWindow())
{
    if(wParam == WM_KEYDOWN)
    {
        //qDebug() << "bsd";
        //QApplication::beep();
        KBDLLHOOKSTRUCT* key = (KBDLLHOOKSTRUCT*)lParam;
        int vkCode = MapVirtualKey(key->scanCode,1);

        if(vkCode != VK_SPACE)//we ignore SPACE
    {
        if(wasEnter)
        {
            if(vkCode >= 0x41 && vkCode <= 0x5A)//from A to Z, only this keys
            {
            //qDebug() << "press" << vkCode;
                INPUT ip;
                ip.type = INPUT_KEYBOARD;
                ip.ki.wScan = 0;
                ip.ki.time = 0;
                ip.ki.dwExtraInfo = 0;
                    // Press the "Ctrl" key
                    ip.ki.wVk = VK_CONTROL;
                    ip.ki.dwFlags = 0; // 0 for key press
                    SendInput(1, &ip, sizeof(INPUT));

                    // Press the "V" key
                    ip.ki.wVk = VK_SPACE;
                    ip.ki.dwFlags = 0; // 0 for key press
                    SendInput(1, &ip, sizeof(INPUT));

                    // Release the "V" key
                    ip.ki.wVk = VK_SPACE;
                    ip.ki.dwFlags = KEYEVENTF_KEYUP;
                    SendInput(1, &ip, sizeof(INPUT));

                    // Release the "Ctrl" key
                    ip.ki.wVk = VK_CONTROL;
                    ip.ki.dwFlags = KEYEVENTF_KEYUP;
                    SendInput(1, &ip, sizeof(INPUT));

                    qDebug() << "ctrl space";

            }
            wasEnter = false;
        }
                if(vkCode == VK_RETURN || vkCode == VK_OEM_COMMA || vkCode == VK_OEM_PERIOD || vkCode == 57)
                {
                    wasEnter = true;
                }
        }


   qDebug() << wasEnter << vkCode;
    }
}

   //qDebug() << "Time:" << timer.elapsed() << "ms";

int varible;

    return CallNextHookEx(hHook,nCode,wParam,lParam);
}

