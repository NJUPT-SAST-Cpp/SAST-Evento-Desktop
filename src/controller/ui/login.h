#ifndef LOGIN_CONTROLLER_H
#define LOGIN_CONTROLLER_H

#include <QtQml>

class Repository;

enum class LoginStatus : int
{
    Start = 1,
    Loading,
    Success,
    Failed
};

class LoginController : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_NAMED_ELEMENT(LoginController)
    Q_PROPERTY(int loginStatus MEMBER status NOTIFY loginStatusChanged)

private:
    int status = int(LoginStatus::Start);

signals:
    void loginStatusChanged();

public:
    static LoginController *create(QQmlEngine *, QJSEngine *)
    {
        static LoginController instance;
        return &instance;
    }

    Q_INVOKABLE void login(const QString &username, const QString &password);

    LoginController() = default;
    ~LoginController() = default;
};

#endif // LOGIN_CONTROLLER_H
