#include "login.h"
#include "repository.h"
#include "user_helper.h"
#include <QDesktopServices>
#include <QHttpServerResponse>
#include <QString>
#include <QtConcurrent>

constexpr const char* AUTH_SERVER_URL = "https://link.sast.fun/auth";
constexpr const char* AUTH_CLIENT_ID = "93f24f17-0423-4baf-9a98-b0ad418a68b8";

static QString genCodeChallengeS256(QStringView code_verifier) {
    auto sha256 = QCryptographicHash::hash(code_verifier.toUtf8(), QCryptographicHash::Sha256);
    return QString::fromLatin1(
        sha256.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals));
}

[[maybe_unused]] static QString generateCryptoRandomString(quint8 length) {
    // FIXME
    // !!! SEVERE SECURITY WARNING !!!
    // 当前实现的熵池可能不满足密码学的要求

    const char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    static std::random_device randomEngine;
    std::uniform_int_distribution<int> distribution(0, sizeof(characters) - 2);
    QByteArray data;
    data.reserve(length);
    for (quint8 i = 0; i < length; ++i)
        data.append(characters[distribution(randomEngine)]);
    return data;
}

LoginController::LoginController() {
    login_redirect_server.route("/", [this](const QHttpServerRequest& request) {
        // OAuth 2.0 Redirect Uri
        auto status_code = QHttpServerResponder::StatusCode::Ok;
        switch (request.method()) {
        case QHttpServerRequest::Method::Options:
            goto finished;
        case QHttpServerRequest::Method::Get:
            break;
        default:
            status_code = QHttpServerResponder::StatusCode::BadRequest;
            goto finished;
        }

        {
            auto query = request.query();
            if (!query.hasQueryItem("state")) {
                status_code = QHttpServerResponder::StatusCode::BadRequest;
                goto finished;
            }

            auto state = query.queryItemValue("state");
            if (state != this->state) {
                status_code = QHttpServerResponder::StatusCode::BadRequest;
                goto finished;
            }
            this->state = nullptr; // Clear State, as it should be used only once

            if (query.hasQueryItem("code")) {
                auto code = query.queryItemValue("code");
                auto future = getRepo()->loginViaSastLink(code).then([=](EventoResult<DTO_User> result) {
                    if (!result) {
                        emit loginFailed(result.message());
                        return;
                    }
                    UserHelper::getInstance()->updateUser(result.take());
                    emit loginSuccess();
                });
            } else if (query.hasQueryItem("error")) {
                auto errorDescription = query.hasQueryItem("error_description")
                                            ? query.queryItemValue("error_description")
                                            : query.queryItemValue("error");
                emit loginFailed(errorDescription);
            } else {
                status_code = QHttpServerResponder::StatusCode::BadRequest;
            }
        }

    finished:
        QHttpServerResponse resp(QHttpServerResponder::StatusCode::InternalServerError);
        if (status_code == QHttpServerResponder::StatusCode::Ok) {
            resp = QHttpServerResponse(
                "text/html",
                "<!DOCTYPE html><html><head><title>Completed</title></head><body><script>window.close();</script><p>Authentication completed. Please close this window. </p></body></html>",
                status_code);
        } else {
            resp = QHttpServerResponse("text/plain",
                                       QStringLiteral("Error %1").arg((int)status_code).toUtf8(),
                                       status_code);
        }
        resp.setHeader("Access-Control-Allow-Origin", "https://link.sast.fun");
        resp.setHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
        resp.setHeader("Access-Control-Allow-Headers",
                       "Origin, Content-Type, Accept, Authorization");
        return resp;
    });
    login_redirect_server.listen(QHostAddress::LocalHost, 1919);
}

LoginController* LoginController::create(QQmlEngine*, QJSEngine*) {
    return new LoginController();
}

void LoginController::beginLoginViaSastLink() {
    // FIXME
    // !!! SEVERE SECURITY WARNING !!!
    // !!! THIS MUST BE FIXED BEFORE RELEASED !!!
    // Evento 后端目前只支持固定为 sast_forever，这导致 code 可以不经过 native 程序处理便被劫持使用
    // 来自浏览器端的攻击可以直接拦截到账号权限，而不需要对 native 程序进行攻击。
    // 潜在的 code 泄露漏洞。
    this->code_verifier = QStringLiteral("sast_forever");
    // 根据 RFC 规定：
    // with a minimum length of 43 characters and a maximum length of 128 characters.
    // https://datatracker.ietf.org/doc/html/rfc7636#section-4.1
    // this->code_verifier = generateCryptoRandomString(64);

    // FIXME
    // !!! SEVERE SECURITY WARNING !!!
    // !!! THIS MUST BE FIXED BEFORE RELEASED !!!
    // SASTLink 目前只支持固定为 xyz，这导致我们无法检查会话 ID，
    // 这可能导致预期之外的请求被 native 程序处理，潜在的 RCE 漏洞利用源。
    this->state = QStringLiteral("xyz");
    // this->state = generateCryptoRandomString(8);

    QUrl url(AUTH_SERVER_URL);
    QUrlQuery query;
    query.addQueryItem("client_id", AUTH_CLIENT_ID);
    query.addQueryItem("code_challenge", genCodeChallengeS256(code_verifier));
    query.addQueryItem("code_challenge_method", "S256");
    query.addQueryItem("redirect_uri", "http://localhost:1919/");
    query.addQueryItem("response_type", "code");
    query.addQueryItem("scope", "all");
    query.addQueryItem("state", state);
    url.setQuery(query);

    QDesktopServices::openUrl(url);

    emit loginProcessing();
}

void LoginController::loadPermissionList() {
    auto future = getRepo()->getAdminPermission().then([this](EventoResult<QStringList> result) {
        if (!result) {
            loadPermissionErrorEvent(result.message());
            return;
        }
        auto permissionList = result.take();
        QMetaObject::invokeMethod(this, [=]() {
            if (permissionList.isEmpty())
                UserHelper::getInstance()->setProperty("permission",
                                                       UserHelper::Permission::UserPermission);
            else
                UserHelper::getInstance()->setProperty("permission",
                                                       UserHelper::Permission::AdminPermisson);
            loadPermissionSuccessEvent();
        });
    });
    QtConcurrent::run([=] {
        auto f(future);
        f.waitForFinished();
    });
}
