#ifndef QUIAPPLICATION_H
#define QUIAPPLICATION_H
#include <QApplication>
#include <functional>

using UIRunable = std::function<void()>;

class QUIApplication : public QApplication
{
    Q_OBJECT
public:
    QUIApplication(int &argc, char **argv);
    virtual ~QUIApplication();

Q_SIGNALS:
    //同步UI执行 (当前UI线程则直接执行)
    void syncExecSignal(UIRunable runable);
    //异步UI执行 (当前UI线程则直接执行)
    void asyncExecSignal(UIRunable runable);

public:
    //获取应用实例
    static QUIApplication* getApp();
    //在UI线程同步执行
    static void syncExec(UIRunable runable);
    //在UI线程异步执行
    static void asyncExec(UIRunable runable);
};

Q_DECLARE_METATYPE(UIRunable)

#endif // QUIAPPLICATION_H
