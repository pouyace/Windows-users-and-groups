#ifndef WIDGET_H
#define WIDGET_H
#include <stdio.h>
#include <windows.h>
#include <lm.h>
#pragma comment(lib, "mpr.lib")
#include <QString>
#include <intsafe.h>
#include <minwindef.h>
#include <windows.h>
#include <QWidget>
#include <winnetwk.h>
#include <tchar.h>
#include <QTextStream>
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    QString serverName ;
    QString outputString = "";
    Ui::Widget *ui;

    //Methods
    void setupConnections();
    bool addUser(QString username, QString password);
    bool deleteUser(QString username);
    bool getInfo(QString username, int level);
    bool getGroups(bool showComment);
    bool getGroupMembers(QString groupName);
    bool getEnum();
    bool addMemberToGroup(QString username, QString groupName);
    void resetOutput();
    void setupProperties();
    void dastiSetUset(QString username);

private slots:
    void onsetUserInfo();
    void onAddUser();
    void onDeleteUser();
    void onGetUserInfo();
    void onShowAllUsers();
    void onGetGroups();
    void onGetGroupMembers();
    void onAddMemberToAGroup();


signals:
    void sigsig(bool);
};
#endif // WIDGET_H
