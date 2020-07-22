#include "quiapplication.h"
#include <QThread>

//获取应用实例
QUIApplication* QUIApplication::getApp()
{
    return static_cast<QUIApplication*>(qApp);
}

//在UI线程同步执行
void QUIApplication::syncExec(UIRunable runable)
{
    QUIApplication* app = QUIApplication::getApp();
    if(nullptr == app || nullptr == runable)
        return;
    if(QThread::currentThread() != app->thread())
    {
        emit app->syncExecSignal(runable);
    }else
    {
        runable();
    }
}

//在UI线程异步执行
void QUIApplication::asyncExec(UIRunable runable)
{
    QUIApplication* app = QUIApplication::getApp();
    if(nullptr == app || nullptr == runable)
        return;

    emit app->asyncExecSignal(runable);
}

QUIApplication::QUIApplication(int &argc, char **argv)
: QApplication(argc, argv)
{
    qRegisterMetaType<UIRunable>();

    //通过qApp槽QueuedConnection来实现UI线程异步调用
    auto async = [this](UIRunable runable)->void
    {
        runable();
    };
    connect(this, &QUIApplication::asyncExecSignal, this, async, Qt::QueuedConnection);

    //通过qApp槽BlockingQueuedConnection来实现UI线程同步调用
    auto sync = [this](UIRunable runable)
    {
        runable();
    };
    connect(this, &QUIApplication::syncExecSignal, this, sync, Qt::BlockingQueuedConnection);
}

QUIApplication::~QUIApplication()
{

}

