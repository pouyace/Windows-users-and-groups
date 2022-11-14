#include "nethandler.h"
#include <sddl.h>
#include <minwindef.h>
#ifndef UNICODE
#define UNICODE
#endif
#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "advapi32.lib")
#include <lm.h>
#include <winnetwk.h>
#include <QMetaEnum>

class NetHandlerPrivate {
    friend class NetHandler;
public:
    NetHandlerPrivate(QHostAddress serverAddress):_serverAddress(serverAddress){}
    QHostAddress _serverAddress;
    NetHandler::Error _error = NetHandler::NoError;
    DWORD unknownErrorCode = 0;
    bool checkOperationResult(DWORD result);
    void setError(NetHandler::Error);
};

wchar_t *getWChar(const QString &str){
    wchar_t *retval = (wchar_t *)malloc((str.length()+1)*sizeof(wchar_t));
    int mLen = str.toWCharArray(retval);
    *(retval + mLen) = 0;
    return retval;
}

NetHandler::NetHandler(QHostAddress serverAddress, QObject *parent)
    : QObject(parent),d(new NetHandlerPrivate(serverAddress))
{

}

NetHandler::~NetHandler()
{
    delete d;
}

bool NetHandler::registerUser(const QString &username, const QString &password, const AccessLevel &level)
{
    if(!(username.length() && password.length())){
        this->d->setError(NullInputError);
        return false;
    }
    QMetaEnum metaEnum = QMetaEnum::fromType<AccessLevel>();
    USER_INFO_1 userInfo;
    DWORD dwLevel = 1;
    DWORD dwError = 0;
    NET_API_STATUS nStatus;
    userInfo.usri1_name = getWChar(username);
    userInfo.usri1_password = getWChar(password);
    userInfo.usri1_priv = USER_PRIV_USER;
    userInfo.usri1_home_dir = NULL;
    userInfo.usri1_comment = getWChar(metaEnum.valueToKey(level));
    userInfo.usri1_flags = UF_SCRIPT;
    userInfo.usri1_script_path = NULL;
    nStatus = NetUserAdd(getWChar(d->_serverAddress.toString()), dwLevel, (LPBYTE)&userInfo, &dwError);
    return d->checkOperationResult(nStatus);
}

bool NetHandler::deleteUser(const QString &username)
{
    NET_API_STATUS nStatus;
    nStatus = NetUserDel(getWChar(d->_serverAddress.toString()), getWChar(username));
    return d->checkOperationResult(nStatus);
}

NetHandler::AccessLevel NetHandler::getUserAccessLevel(const QString& username)
{
    DWORD dwLevel = 20;
    AccessLevel userLevel = InvalidLevel;
    LPUSER_INFO_20 pBuf = NULL;
    NET_API_STATUS nStatus;
    QMetaEnum metaEnum = QMetaEnum::fromType<AccessLevel>();
    nStatus = NetUserGetInfo(getWChar(d->_serverAddress.toString()), getWChar(username), dwLevel, (LPBYTE *) & pBuf);
    if (nStatus == NERR_Success)
    {
        if (pBuf != NULL)
        {
            const char* data = QString::fromWCharArray(pBuf->usri20_comment).toStdString().data();
            userLevel =  AccessLevel(metaEnum.keyToValue(data));
        }
    }
    d->checkOperationResult(nStatus);
    if (pBuf != NULL)
        NetApiBufferFree(pBuf);
   return userLevel;
}

QStringList NetHandler::getGroups()
{
    LOCALGROUP_INFO_1* grpInfo = NULL;
    LOCALGROUP_INFO_1* tempgrpInfo;
    DWORD entriesRead = 0;
    DWORD totalEntries = 0;
    PDWORD_PTR resume_ptr = nullptr;
    QStringList groups;

    int res = NetLocalGroupEnum(getWChar(d->_serverAddress.toString()), 1, (LPBYTE*)&grpInfo, -1, &entriesRead, &totalEntries, resume_ptr);
    if(entriesRead > 0)
    {
        tempgrpInfo = grpInfo;
        for(int i = 0;i < entriesRead;i++)
        {
            groups.append(QString::fromWCharArray(tempgrpInfo->lgrpi1_name));
            tempgrpInfo++;
        }
    }
    d->checkOperationResult(res);

    if(grpInfo)
    {
        NetApiBufferFree(grpInfo);
        grpInfo = NULL;
    }
    return groups;
}

QStringList NetHandler::getGroupMembers(const QString &groupName)
{
    QStringList members;
    QString localGroupName = groupName;
    DWORD dwLevel = 1;
    DWORD count = 0;
    DWORD totalEntries = 0;
    LPLOCALGROUP_MEMBERS_INFO_1 buff = 0 ;
    DWORD retValue = NetLocalGroupGetMembers(getWChar(d->_serverAddress.toString()), getWChar(localGroupName)
                                           , 1, (LPBYTE*)&buff, MAX_PREFERRED_LENGTH, &count, &totalEntries, 0);

    for(int i=0;i<count;i++){
        members.append(QString::fromWCharArray(buff->lgrmi1_name));
        buff++;
    }
    if (buff != NULL)
    {
        NetApiBufferFree((LPBYTE*)&buff);
        buff = NULL;
    }
    d->checkOperationResult(retValue);
    return members;
}


bool NetHandler::addMemberToGroup(const QString &username, const QString &groupName)
{
    DWORD level = 3;
    LOCALGROUP_MEMBERS_INFO_3 buf;
    buf.lgrmi3_domainandname = getWChar(username);
    DWORD count = 1;
    DWORD result = NetLocalGroupAddMembers(getWChar(d->_serverAddress.toString())
                                         , getWChar(groupName), level, (LPBYTE)&buf, count);
    return d->checkOperationResult(result);
}

bool NetHandler::setUserAccessLevel(const QString &username, const AccessLevel &userLevel)
{
    USER_INFO_1007 userComment;
    QMetaEnum metaEnum = QMetaEnum::fromType<AccessLevel>();
    userComment.usri1007_comment =  getWChar(metaEnum.valueToKey(userLevel));
    DWORD dwlevel = 1;
    DWORD sdf = 0;
    NET_API_STATUS nStatus = NetUserSetInfo(getWChar(d->_serverAddress.toString()),getWChar(username),1007,(LPBYTE)&userComment,&sdf);
    return d->checkOperationResult(nStatus);
}

bool NetHandlerPrivate::checkOperationResult(DWORD result)
{
    switch (result) {
    case NERR_Success: setError(NetHandler::NoError); return true;
    case NERR_UserExists: setError(NetHandler::UsernameAlreadyExists); return false;
    case ERROR_ACCESS_DENIED: setError(NetHandler::AccessDenied); return false;
    case NERR_GroupNotFound: setError(NetHandler::GroupNotFound); return false;
    case NERR_PasswordTooShort: setError(NetHandler::PasswordTooShort); return false;
    case NERR_UserNotFound: setError(NetHandler::UsernameNotFound); return false;
    default: setError(NetHandler::UnknownError); unknownErrorCode = result; return false;
    }
}

void NetHandlerPrivate::setError(NetHandler::Error error)
{
    _error = error;
}

QString NetHandler::errorString() const
{
    switch (d->_error) {
    case NoError: return "No error";
    case UsernameAlreadyExists: return "Username already taken";
    case ServerNameError: return "Server name error";
    case NullInputError: return "Empty input not allowed";
    case AccessDenied: return "Access denied";
    case PasswordTooShort: return "password is too short";
    case UnknownError: return "A system error has occurred: "+ QString::number(d->unknownErrorCode);
    case UsernameNotFound: return "Username not found";
    case GroupNotFound: return "Group not found";
    }
}
