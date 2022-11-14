#include "widget.h"
#include "ui_widget.h"
#include <QTextBrowser>
#include <lmaccess.h>
#include <sddl.h>
#include <stdio.h>

#include <minwindef.h>
#include <QDebug>
#include <QFile>
#ifndef UNICODE
#define UNICODE
#endif
#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "advapi32.lib")
#ifndef UNICODE
#define UNICODE
#endif
#pragma comment(lib, "netapi32.lib")

#include <stdio.h>
#include <windows.h>
#include <lm.h>

wchar_t *getWChar(const QString &str){
    wchar_t *retval = (wchar_t *)malloc((str.length()+1)*sizeof(wchar_t));
    int mLen = str.toWCharArray(retval);
    *(retval + mLen) = 0;
    return retval;
}
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setupConnections();
    setupProperties();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::setupConnections()
{
    connect(ui->getAllUsersPushButton,      &QPushButton::clicked, this, &Widget::onShowAllUsers);
    connect(ui->addUser_pushButton,         &QPushButton::clicked, this, &Widget::onAddUser);
    connect(ui->deleteUser_pushButton,      &QPushButton::clicked, this, &Widget::onDeleteUser);
    connect(ui->getUserInfo_pushButton,     &QPushButton::clicked, this, &Widget::onGetUserInfo);
    connect(ui->getGroups_PushButton,       &QPushButton::clicked, this, &Widget::onGetGroups);
    connect(ui->getGroupMembers_pushButton, &QPushButton::clicked, this, &Widget::onGetGroupMembers);
    connect(ui->addMemberToGroup_pushButton,&QPushButton::clicked, this, &Widget::onAddMemberToAGroup);
    connect(ui->setInfoPushButton,          &QPushButton::clicked, this, &Widget::onsetUserInfo);
}

bool Widget::addUser(QString username, QString password)
{
    USER_INFO_1 ui;
    DWORD dwLevel = 1;
    DWORD dwError = 0;
    NET_API_STATUS nStatus;
    ui.usri1_name = getWChar(username);
    ui.usri1_password = /*NULL*/getWChar(password);
    ui.usri1_priv = USER_PRIV_USER;
    ui.usri1_home_dir = NULL;
    ui.usri1_comment = NULL;
    ui.usri1_flags = UF_SCRIPT;
    ui.usri1_script_path = NULL;
    nStatus = NetUserAdd(getWChar(serverName),
                         dwLevel,
                         (LPBYTE)&ui,
                         &dwError);
    if (nStatus == NERR_Success){
        outputString = "User " + username +" has been successfully added on " + serverName + ".\n";
        return true;
    }
    else
        outputString =  "A system error has occurred: "+ QString::number(nStatus)+ ".\n" +QString::number(dwError)+ ".\n";
    return false;
}

bool Widget::deleteUser(QString username)
{
#ifndef UNICODE
#define UNICODE
#endif
#pragma comment(lib, "netapi32.lib")

#include <stdio.h>
#include <windows.h>
#include <lm.h>
#include <lmaccess.h>

    DWORD dwError = 0;
    NET_API_STATUS nStatus;

    nStatus = NetUserDel(getWChar(/*"10.10.10.139"*/serverName),getWChar(username));
    if (nStatus == NERR_Success){
        outputString = "User "+ username +" has been successfully deleted";
        return true;
    }
    else
        outputString = "A system error has occurred: " + QString::number(nStatus)+"\n";
    return false;
}

bool Widget::getInfo(QString username, int level)
{
#ifndef UNICODE
#define UNICODE
#endif
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "netapi32.lib")

#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <lm.h>
#include <sddl.h>               /* for ConvertSidToStringSid function */

    if (!(level ==0 || level == 1 || level==2 || level ==20)){
        outputString += "invalid level(allowed leves= 0, 1, 2, 20)";
        return true;
    }
    DWORD dwLevel = 0;
    LPUSER_INFO_0 pBuf = NULL;
    LPUSER_INFO_1 pBuf1 = NULL;
    LPUSER_INFO_2 pBuf2 = NULL;
    LPUSER_INFO_20 pBuf20 = NULL;
    NET_API_STATUS nStatus;
    LPTSTR sStringSid = NULL;
    dwLevel = level;
    nStatus = NetUserGetInfo(getWChar(serverName), getWChar(username), dwLevel, (LPBYTE *) & pBuf);
    if (nStatus == NERR_Success)
    {
        if (pBuf != NULL)
        {

            switch (level)
            {
            case 0:
                outputString += "\tUser account name: "+ QString::fromWCharArray(pBuf->usri0_name )+ "\n";
                if (pBuf != NULL)
                    NetApiBufferFree(pBuf);
                return true;
            case 1:
                pBuf1 = (LPUSER_INFO_1) pBuf;
                outputString += "\tUser account name: " + QString::fromWCharArray(pBuf1->usri1_name ) +"\n";
                outputString += "\tPassword: "+ QString::fromWCharArray(pBuf1->usri1_password)+"\n" ;
                outputString += "\tPassword age (seconds): "+ QString::number(pBuf1->usri1_password_age) +"\n";
                outputString += "\tPrivilege level: "+ QString::number(pBuf1->usri1_priv) + "\n";
                outputString += "\tHome directory: "+ QString::fromWCharArray(pBuf1->usri1_home_dir)+"\n";
                outputString += "\tUser comment: "+ QString::fromWCharArray(pBuf1->usri1_comment)+"\n";
                outputString += "\tFlags (in hex): "+ QString::number(pBuf1->usri1_flags)+"\n";
                outputString += "\tScript path: "+ QString::fromWCharArray(pBuf1->usri1_script_path)+"\n";
                if (pBuf != NULL)
                    NetApiBufferFree(pBuf);
                return true;
            case 2:
                pBuf2 = (LPUSER_INFO_2) pBuf;
                outputString += "\tUser account name: "+ QString::fromWCharArray(pBuf2->usri2_name)+"\n";
                outputString += "\tPassword: "+ QString::fromWCharArray(pBuf2->usri2_password)+"\n";
                outputString += "\tPassword age (seconds): "+ QString::number(pBuf2->usri2_password_age)+"\n";
                outputString += "\tPrivilege level: "+ QString::number(pBuf2->usri2_priv)+"\n";
                outputString += "\tHome directory: "+ QString::fromWCharArray(pBuf2->usri2_home_dir)+"\n";
                outputString += "\tComment: "+ QString::fromWCharArray(pBuf2->usri2_comment)+"\n";
                outputString += "\tFlags (in hex): "+ QString::number(pBuf2->usri2_flags)+"\n";
                outputString += "\tScript path: "+ QString::fromWCharArray(pBuf2->usri2_script_path)+"\n";
                outputString += "\tAuth flags (in hex): "+ QString::number(pBuf2->usri2_auth_flags)+"\n";
                outputString += "\tFull name: "+ QString::fromWCharArray(pBuf2->usri2_full_name)+"\n";
                outputString += "\tUser comment: "+ QString::fromWCharArray(pBuf2->usri2_usr_comment)+"\n";
                outputString += "\tParameters: "+ QString::fromWCharArray(pBuf2->usri2_parms)+"\n";
                outputString += "\tWorkstations: "+ QString::fromWCharArray(pBuf2->usri2_workstations)+"\n";
                outputString += "\tLast logon (seconds since January 1, 1970 GMT): "+ QString::number(pBuf2->usri2_last_logon)+"\n";
                outputString += "\tLast logoff (seconds since January 1, 1970 GMT): "+ QString::number(pBuf2->usri2_last_logoff)+"\n";
                outputString += "\tAccount expires (seconds since January 1, 1970 GMT): "+ QString::number(pBuf2->usri2_acct_expires)+"\n";
                outputString += "\tMax storage: "+ QString::number(pBuf2->usri2_max_storage)+"\n" ;
                outputString += "\tUnits per week: "+ QString::number(pBuf2->usri2_units_per_week)+"\n";
                outputString += "\tBad password count: "+ QString::number(pBuf2->usri2_bad_pw_count)+"\n";
                outputString += "\tNumber of logons: "+ QString::number(pBuf2->usri2_num_logons)+"\n";
                outputString += "\tLogon server: "+ QString::fromWCharArray(pBuf2->usri2_logon_server)+"\n";
                outputString += "\tCountry code: "+ QString::number(pBuf2->usri2_country_code)+"\n";
                outputString += "\tCode page: "+ QString::number(pBuf2->usri2_code_page)+"\n" ;
                if (pBuf != NULL)
                    NetApiBufferFree(pBuf);
                return true;
            case 20:
                pBuf20 = (LPUSER_INFO_20) pBuf;
                outputString += "\tUser account name: "+QString::fromWCharArray(pBuf20->usri20_name)+"\n";
                outputString += "\tFull name: "+ QString::fromWCharArray(pBuf20->usri20_full_name)+"\n";
                outputString += "\tComment: "+ QString::fromWCharArray(pBuf20->usri20_comment)+"\n";
                outputString += "\tFlags (in hex): "+ QString::number(pBuf20->usri20_flags)+"\n";
                outputString += "\tUser ID: "+ QString::number(pBuf20->usri20_user_id)+"\n";
                if (pBuf != NULL)
                    NetApiBufferFree(pBuf);
                return true;
            default:
                return false;
            }
        }
    }
    else{
        outputString = "NetUserGetinfo failed with error: "+ QString::number(nStatus) + "\n";
        if (pBuf != NULL)
            NetApiBufferFree(pBuf);
        return false;
    }
    if (pBuf != NULL)
        NetApiBufferFree(pBuf);
    return false;
}

bool Widget::getGroups(bool showComment)
{
    LOCALGROUP_INFO_1* grpInfo = NULL;
    LOCALGROUP_INFO_1* tempgrpInfo;
    DWORD entriesRead = 0;
    DWORD totalEntries = 0;
    DWORD resume = 0;
    PDWORD_PTR resume_ptr = nullptr;

    int res = NetLocalGroupEnum(getWChar(serverName), 1, (LPBYTE*)&grpInfo, -1, &entriesRead, &totalEntries, resume_ptr);
    if(entriesRead > 0)
    {
        tempgrpInfo = grpInfo;
        for(int i = 0;i < entriesRead;i++)
        {
            outputString += QString::fromWCharArray(tempgrpInfo->lgrpi1_name)+ "\n";
            if(showComment)
                outputString += "comment: "+QString::fromWCharArray(tempgrpInfo->lgrpi1_comment)+ "\n";
            outputString += "------------------------------\n";
            tempgrpInfo++;
        }
    }

    if(grpInfo)
    {
        NetApiBufferFree(grpInfo);
        grpInfo = NULL;
    }
    return true;
}

bool Widget::getGroupMembers(QString groupName)
{
    QString localGroupName = groupName;
    DWORD dwLevel = 1;
    DWORD count = 0;
    DWORD totalEntries = 0;
    LPLOCALGROUP_MEMBERS_INFO_1 buff = 0 ;
    PDWORD_PTR resume_ptr = nullptr;
    int retValue = NetLocalGroupGetMembers(getWChar(serverName), getWChar(localGroupName)
                                           , 1, (LPBYTE*)&buff, MAX_PREFERRED_LENGTH, &count, &totalEntries, 0);



    outputString +="\tusers count = "+QString::number(count) +"\n" + QString::number(retValue);
    for(int i=0;i<count;i++){
        outputString += QString::fromWCharArray(buff->lgrmi1_name)+"\n";
        buff++;
    }
    if (buff != NULL)
    {
        NetApiBufferFree((LPBYTE*)&buff);
        buff = NULL;
    }
    return retValue;
}

bool Widget::getEnum()
{
#ifndef UNICODE
#define UNICODE
#endif
#pragma comment(lib, "netapi32.lib")

#include <stdio.h>
#include <assert.h>
#include <windows.h>
#include <lm.h>
    LPUSER_INFO_0 pBuf = NULL;
    LPUSER_INFO_0 pTmpBuf;
    DWORD dwLevel = 0;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;
    DWORD i;
    DWORD dwTotalCount = 0;
    NET_API_STATUS nStatus;
    LPTSTR pszServerName = NULL;
    pszServerName =  (LPTSTR) getWChar(serverName);
    outputString += "\nUser account on "+QString::fromWCharArray( pszServerName)+": \n";
    do
    {
        nStatus = NetUserEnum((LPCWSTR) pszServerName,
                              dwLevel,
                              FILTER_NORMAL_ACCOUNT, // global users
                              (LPBYTE*)&pBuf,
                              dwPrefMaxLen,
                              &dwEntriesRead,
                              &dwTotalEntries,
                              &dwResumeHandle);

        if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA))
        {
            if ((pTmpBuf = pBuf) != NULL)
            {

                for (i = 0; (i < dwEntriesRead); i++)
                {
                    assert(pTmpBuf != NULL);

                    if (pTmpBuf == NULL)
                    {
                        outputString += "An access violation has occurred\n";
                        break;
                    }

                    outputString += "\t-- "+QString::fromWCharArray( pTmpBuf->usri0_name)+"\n";
                    pTmpBuf++;
                    dwTotalCount++;
                }
            }
        }

        else
            outputString += "A system error has occurred: "+QString::number( nStatus)+"\n";

        if (pBuf != NULL)
        {
            NetApiBufferFree(pBuf);
            pBuf = NULL;
        }
    }
    while (nStatus == ERROR_MORE_DATA);
    if (pBuf != NULL)
        NetApiBufferFree(pBuf);
    outputString += "\nTotal of "+QString::number( dwTotalCount) +" entries enumerated\n";
    return true;

}

bool Widget::addMemberToGroup(QString username, QString groupName)
{
    QString servernamee = serverName;
    QString groupname = groupName;
    DWORD level = 3;
    LOCALGROUP_MEMBERS_INFO_3 buf;
    buf.lgrmi3_domainandname = getWChar(username);
    DWORD count = 1;
    qDebug()<<"boro berim";
    int result = NetLocalGroupAddMembers(getWChar(servernamee)
                                         , getWChar(groupname), level, (LPBYTE)&buf, count);

//    if ( (LPBYTE)&buf != NULL)
//        NetApiBufferFree(&buf);
    if(result == 0)
        outputString += "user '"+ username+ "' has been added to '"+ groupName+ "'\n";
    else
        outputString += "operation failed";
    return result;
}

void Widget::resetOutput()
{
    outputString = "";
    ui->textEdit->clear();
}

void Widget::setupProperties()
{
    ui->serverNameLineEdit->setText("192.168.1.100");
}

void Widget::dastiSetUset(QString username)
{
//    DWORD dwLevel = 0;
//    LPUSER_INFO_1005 pBuf1 = NULL;
//    LPUSER_INFO_0 pBuf = NULL;
//    NET_API_STATUS nStatus;
//    LPTSTR sStringSid = NULL;
//    dwLevel = 1005;
//    nStatus = NetUserGetInfo(getWChar(serverName), getWChar(username), dwLevel, (LPBYTE *) & pBuf);
//    pBuf1 = (LPUSER_INFO_1005) pBuf;
//    QString temp =  "\tPrivilege level: "+ QString::number(pBuf1->usri1005_priv) + "\n";
//    pBuf1->usri1005_priv = 1;
//    temp+= "\tPrivilege level: "+ QString::number(pBuf1->usri1005_priv) + "\n";
    USER_INFO_1007 userpriv;
    userpriv.usri1007_comment = getWChar("vay vay vay delam");
    DWORD dwlevel = 1;
    DWORD sdf = 0;
    NET_API_STATUS nStatus2 = NetUserSetInfo(getWChar(serverName),getWChar(username),1007,(LPBYTE)&userpriv,&sdf);
//    temp+= "\tPrivilege level: "+ QString::number(pBuf1->usri1005_priv) + "\n";
//    temp+= QString::number(nStatus) + "\n" + QString::number(sdf);

    this->ui->textEdit->setText(QString::number(nStatus2) + "\n" + QString::number(sdf));
}

void Widget::onsetUserInfo()
{
    serverName = ui->serverNameLineEdit->text();
    resetOutput();
  this->dastiSetUset(ui->lineEdit->text());
}

void Widget::onAddUser()
{
    serverName = ui->serverNameLineEdit->text();
    resetOutput();
    ui->lastCommandCheckBox->setChecked(addUser(ui->addUser_usernameLineEdit->text(), ui->addUser_passwordLineEdit->text()));
    ui->textEdit->setText(outputString);
}

void Widget::onDeleteUser()
{
    serverName = ui->serverNameLineEdit->text();
    resetOutput();
    ui->lastCommandCheckBox->setChecked(deleteUser(ui->deleteUser_usernameLineEdit->text()));
    ui->textEdit->setText(outputString);
}

void Widget::onGetUserInfo()
{
    serverName = ui->serverNameLineEdit->text();
    resetOutput();
    ui->lastCommandCheckBox->setChecked(getInfo(ui->getUserInfo_usernameLineEdit->text(), ui->getUserInfo_levelSpinBox->value()));
    ui->textEdit->setText(outputString);
}

void Widget::onShowAllUsers()
{
    serverName = ui->serverNameLineEdit->text();
    resetOutput();
    ui->lastCommandCheckBox->setChecked(getEnum());
    ui->textEdit->setText(outputString);
}

void Widget::onGetGroups()
{
    serverName = ui->serverNameLineEdit->text();
    resetOutput();
    ui->lastCommandCheckBox->setChecked(getGroups(ui->getGroups_checkBox->isChecked()));
    ui->textEdit->setText(outputString);
}

void Widget::onGetGroupMembers()
{
    serverName = ui->serverNameLineEdit->text();
    resetOutput();
    ui->lastCommandCheckBox->setChecked(getGroupMembers(ui->getGroupMembers_groupnameLineEdit->text()));
    ui->textEdit->setText(outputString);
}

void Widget::onAddMemberToAGroup()
{
    serverName = ui->serverNameLineEdit->text();
    resetOutput();
    ui->lastCommandCheckBox->setChecked(addMemberToGroup(ui->addMemberToGroup_usernameLineEdit->text(), ui->addMemberToGroup_groupnameLineEdit->text()));
    ui->textEdit->setText(outputString);
}

