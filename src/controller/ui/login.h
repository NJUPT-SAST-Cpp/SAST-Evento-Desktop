#ifndef LOGIN_CONTROLLER_H
#define LOGIN_CONTROLLER_H

#include <QtQml>

class Repository;

enum class LoginStatus : int {
    Start = 1,
    Loading,
    Success,
    Failed
};

class LoginController : public QObject {
    Q_OBJECT
    QML_NAMED_ELEMENT(LoginController)
    Q_PROPERTY(int loginStatus MEMBER status NOTIFY loginStatusChanged)

private:
    int status = int(LoginStatus::Start);
    std::unique_ptr<Repository> m_repository;

signals:
    void loginStatusChanged();

public:
    Q_INVOKABLE void login(const QString& username, const QString& password);
    LoginController(QObject* parent = nullptr);
    ~LoginController();
};

#endif  // LOGIN_CONTROLLER_H
