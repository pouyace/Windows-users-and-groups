#ifndef NETHANDLER
#define NETHANDLER
#include <windows.h>
#include <QObject>
#include <QHostAddress>

class NetHandlerPrivate;
class NetHandler : public QObject
{
    Q_OBJECT
public:
    enum Error{NoError, ServerNameError, UsernameAlreadyExists, NullInputError, AccessDenied, PasswordTooShort, UnknownError,
              UsernameNotFound, GroupNotFound};
    enum AccessLevel{InvalidLevel, Level1, Level2, Level3, Level4, Level5};
    Q_ENUM(AccessLevel)
    NetHandler(QHostAddress serverAddress, QObject *parent = nullptr);
    ~NetHandler();

    bool registerUser(const QString& username, const QString& password, const AccessLevel& level);
    bool deleteUser(const QString& username);
    bool addMemberToGroup(const QString& username, const QString& groupName);
    QStringList getGroupMembers(const QString& groupName);
    QString errorString() const;
    bool setUserAccessLevel(const QString& username, const AccessLevel& userLevel);
    AccessLevel getUserAccessLevel(const QString &username);
    QStringList getGroups();

private:
    NetHandlerPrivate *d = nullptr;
};
#endif
