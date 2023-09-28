#include "EventoNetworkClient.h"

#include <json_deserialise.hpp>

#include <QtConcurrent>
#include <QSslConfiguration>

static auto API_GATEWAY = QStringLiteral("https://evento.sast.fun/api");
constexpr const char USER_AGENT[] = "SAST-Evento-Desktop/1";
constexpr const char MIME_FORM_URL_ENCODED[] = "application/x-www-form-urlencoded";
constexpr const char MIME_JSON[] = "application/json";

static void setSsl(QNetworkRequest& request) {
    QSslConfiguration ssl_config(request.sslConfiguration());
    ssl_config.setPeerVerifyMode(QSslSocket::VerifyNone);
    ssl_config.setProtocol(QSsl::AnyProtocol);
}

static EventoResult<QJsonValue> handleNetworkReply(QNetworkReply* reply) {
    auto content = reply->readAll();
    auto networkError = reply->error();

    qDebug() << content << networkError;

    if (networkError != QNetworkReply::NoError) {
        return EventoException(EventoExceptionCode::NetworkError, "network error");
    }
    QJsonParseError jsonError;
    auto result = QJsonDocument::fromJson(content, &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        return EventoException(EventoExceptionCode::JsonError,
                               QString("json error: %1 (offest = %2)")
                                   .arg(jsonError.errorString())
                                   .arg(jsonError.offset));
    }
    reply->deleteLater();
    if (!result.isObject()) {
        return EventoException(EventoExceptionCode::JsonError,
                               QStringLiteral("expect object but got other"));
    }
    QJsonObject jsonObject = result.object();
    auto successItem = jsonObject.constFind("success");
    if (successItem != jsonObject.constEnd() && successItem.value().toBool()) {
        // successful
        auto dataItem = jsonObject.constFind("data");
        if (dataItem != jsonObject.constEnd()) {
            return QJsonValue(dataItem.value());
        }
        return QJsonValue{};
    } else {
        // error
        auto errCodeItem = jsonObject.constFind("errCode");
        auto errMsgItem = jsonObject.constFind("errMsg");
        EventoExceptionCode errCode = EventoExceptionCode::UnexpectedError;
        QString errMsg = QStringLiteral("no error message");
        if (errCodeItem != jsonObject.constEnd()) {
            errCode = static_cast<EventoExceptionCode>(
                errCodeItem.value().toInt(static_cast<int>(EventoExceptionCode::UnexpectedError)));
        }
        if (errMsgItem != jsonObject.constEnd()) {
            errMsg = errMsgItem.value().toString(errMsg);
        }
        return EventoException(errCode, errMsg);
    }
}

QFuture<EventoResult<QJsonValue>> EventoNetworkClient::get(const QUrl& url) {
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, USER_AGENT);
    request.setRawHeader("Accept", MIME_JSON);
    if (!this->tokenBytes.isEmpty()) {
        request.setRawHeader("TOKEN", this->tokenBytes);
    }
    setSsl(request);
    auto reply = manager.get(request);
    return QtConcurrent::run([=]() {
        QtFuture::connect(reply, &QNetworkReply::finished).waitForFinished();
        return handleNetworkReply(reply);
    });
}

QUrl EventoNetworkClient::endpoint(const QString& endpoint) {
    return QUrl(API_GATEWAY + endpoint);
}

QUrl EventoNetworkClient::endpoint(const QString& endpoint, const QUrlQuery& params) {
    auto r = QUrl(API_GATEWAY + endpoint);
    r.setQuery(params);
    return r;
}

QFuture<EventoResult<QJsonValue>> EventoNetworkClient::post(const QUrl& url,
                                                            const QString& contentType,
                                                            const QByteArray& requestData) {
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, USER_AGENT);
    request.setHeader(QNetworkRequest::ContentTypeHeader, contentType);
    request.setRawHeader("Accept", MIME_JSON);
    if (!this->tokenBytes.isEmpty()) {
        request.setRawHeader("TOKEN", this->tokenBytes);
    }
    setSsl(request);
    auto reply = manager.post(request, requestData);
    return QtConcurrent::run([=]() {
        QtFuture::connect(reply, &QNetworkReply::finished).waitForFinished();
        return handleNetworkReply(reply);
    });
}

QFuture<EventoResult<QJsonValue>> EventoNetworkClient::post(const QUrl& url,
                                                            const QUrlQuery& requestData) {
    return this->post(url, MIME_FORM_URL_ENCODED, requestData.toString().toUtf8());
}

QFuture<EventoResult<QJsonValue>> EventoNetworkClient::post(const QUrl& url,
                                                            const QJsonDocument& requestData) {
    return this->post(url, MIME_JSON, requestData.toJson(QJsonDocument::Compact));
}

QFuture<EventoResult<QJsonValue>> EventoNetworkClient::put(const QUrl& url,
                                                           const QString& contentType,
                                                           const QByteArray& requestData) {
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, USER_AGENT);
    request.setHeader(QNetworkRequest::ContentTypeHeader, contentType);
    request.setRawHeader("Accept", MIME_JSON);
    if (!this->tokenBytes.isEmpty()) {
        request.setRawHeader("TOKEN", this->tokenBytes);
    }
    setSsl(request);
    auto reply = manager.put(request, requestData);
    return QtConcurrent::run([=]() {
        QtFuture::connect(reply, &QNetworkReply::finished).waitForFinished();
        return handleNetworkReply(reply);
    });
}

QFuture<EventoResult<QJsonValue>> EventoNetworkClient::put(const QUrl& url,
                                                           const QUrlQuery& requestData) {
    return this->put(url, MIME_FORM_URL_ENCODED, requestData.toString().toUtf8());
}

QFuture<EventoResult<QJsonValue>> EventoNetworkClient::put(const QUrl& url,
                                                           const QJsonDocument& requestData) {
    return this->put(url, MIME_JSON, requestData.toJson(QJsonDocument::Compact));
}

QFuture<EventoResult<QJsonValue>> EventoNetworkClient::patch(const QUrl& url,
                                                             const QString& contentType,
                                                             const QByteArray& requestData) {
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, USER_AGENT);
    request.setHeader(QNetworkRequest::ContentTypeHeader, contentType);
    request.setRawHeader("Accept", MIME_JSON);
    if (!this->tokenBytes.isEmpty()) {
        request.setRawHeader("TOKEN", this->tokenBytes);
    }
    setSsl(request);
    auto reply = manager.sendCustomRequest(request, "PATCH", requestData);
    return QtConcurrent::run([=]() {
        QtFuture::connect(reply, &QNetworkReply::finished).waitForFinished();
        return handleNetworkReply(reply);
    });
}

QFuture<EventoResult<QJsonValue>> EventoNetworkClient::patch(const QUrl& url,
                                                             const QUrlQuery& requestData) {
    return this->patch(url, MIME_FORM_URL_ENCODED, requestData.toString().toUtf8());
}

QFuture<EventoResult<QJsonValue>> EventoNetworkClient::patch(const QUrl& url,
                                                             const QJsonDocument& requestData) {
    return this->patch(url, MIME_JSON, requestData.toJson(QJsonDocument::Compact));
}

QFuture<EventoResult<QJsonValue>> EventoNetworkClient::deleteResource(const QUrl& url) {
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, USER_AGENT);
    request.setRawHeader("Accept", MIME_JSON);
    if (!this->tokenBytes.isEmpty()) {
        request.setRawHeader("TOKEN", this->tokenBytes);
    }
    setSsl(request);
    auto reply = manager.deleteResource(request);
    return QtConcurrent::run([=]() {
        QtFuture::connect(reply, &QNetworkReply::finished).waitForFinished();
        return handleNetworkReply(reply);
    });
}

static QStringList asStringList(const QJsonValue& value) {
    auto array = value.toArray();
    QStringList result;
    result.reserve(array.size());
    for (auto v : array) {
        result.push_back(v.toString());
    }
    return result;
}

// userFetch
QFuture<EventoResult<QStringList>> EventoNetworkClient::getAdminPermission() {
    auto url = endpoint(QStringLiteral("/permission/admin/self"));
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<QStringList> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            return asStringList(result.take());
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<QStringList>> EventoNetworkClient::getManagerPermission(EventoID eventId) {
    auto url = endpoint(QStringLiteral("/permission/event/manager/self"));
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<QStringList> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            return asStringList(result.take());
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<QStringList>> EventoNetworkClient::getPermittedEvent() {
    auto url = endpoint(QStringLiteral("/permission/manager/events"));
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<QStringList> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            return asStringList(result.take());
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<DTO_Permission>> EventoNetworkClient::getEventPermission(EventoID event) {
    // TODO: implement
    return {};
}

QFuture<EventoResult<DTO_User>> EventoNetworkClient::getUserInfo(const UserID& id) {
    auto url = endpoint(QStringLiteral("/user/info"),
                        [&](QUrlQuery& params) { params.addQueryItem("userId", id); });
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<DTO_User> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            // FIXME
            return DTO_User{};
        } else {
            return {result.code(), result.message()};
        }
    });
}

register_object_member(ParticipationStatus, "isRegistration", isRegistrated);
register_object_member(ParticipationStatus, "isParticipate", isParticipated);
register_object_member(ParticipationStatus, "isSubscribe", isSubscribed);
declare_object(ParticipationStatus, object_member(ParticipationStatus, isRegistrated),
               object_member(ParticipationStatus, isParticipated),
               object_member(ParticipationStatus, isSubscribed));

QFuture<EventoResult<ParticipationStatus>> EventoNetworkClient::getUserParticipate(EventoID event) {
    auto url = endpoint(QStringLiteral("/user/participate"), [=](QUrlQuery& params) {
        params.addQueryItem("eventId", QString::number(event));
    });
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<ParticipationStatus> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            auto rootValue = result.take();
            ParticipationStatus result{};
            if (rootValue.isObject()) {
                declare_top_deserialiser(result, deserialiser_holder);
                JsonDeserialise::JsonDeserialiser deserialiser(deserialiser_holder);
                deserialiser.deserialise(rootValue.toObject());
            } else if (rootValue.isNull()) {
                // okey
            } else {
                return EventoException(EventoExceptionCode::JsonError,
                                       QStringLiteral("expect object or null but got other"));
            }
            return result;
        } else {
            return {result.code(), result.message()};
        }
    });
}

register_object_member(DTO_Feedback, "id", id);
register_object_member(DTO_Feedback, "content", content);
register_object_member(DTO_Feedback, "score", score);
register_object_member(DTO_Feedback, "eventId", eventId);
declare_object(DTO_Feedback, object_member(DTO_Feedback, id), object_member(DTO_Feedback, content),
               object_member(DTO_Feedback, score), object_member(DTO_Feedback, eventId));

QFuture<EventoResult<DTO_Feedback>> EventoNetworkClient::getFeedbackInfo(EventoID eventoId) {
    auto url = endpoint(QStringLiteral("/feedback/user/info"), [=](QUrlQuery& params) {
        params.addQueryItem("eventId", QString::number(eventoId));
    });
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<DTO_Feedback> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            auto rootValue = result.take();
            DTO_Feedback result{};
            if (rootValue.isObject()) {
                declare_top_deserialiser(result, deserialiser_holder);
                JsonDeserialise::JsonDeserialiser deserialiser(deserialiser_holder);
                deserialiser.deserialise(rootValue.toObject());
            }
            return result;
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<bool>> EventoNetworkClient::subscribeDepartment(int departmentId,
                                                                     bool unsubscribe) {
    auto url = endpoint(QStringLiteral("/user/subscribe/department"), [=](QUrlQuery& params) {
        params.addQueryItem("departmentId", QString::number(departmentId));
        params.addQueryItem("isSubscribe", unsubscribe ? "true" : "false");
    });
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<bool> {
        auto f(future);
        auto result = f.takeResult();
        if (!result)
            return {result.code(), result.message()};
        return {};
    });
}

register_object_member(EventType, "id", id);
register_object_member(EventType, "typeName", name);
register_object_member(EventType, "allowConflict", allowConflict);
declare_object(EventType, object_member(EventType, id), object_member(EventType, name));

register_object_member(Department, "id", id);
register_object_member(Department, "departmentName", name);

declare_object(Department, object_member(Department, id), object_member(Department, name));

static QDateTime str2date(const QString& text) {
    return QDateTime::fromString(text, "yyyy-MM-dd HH:mm:ss");
};
static QString date2str(const QDateTime& time) {
    return time.toString("yyyy-MM-dd HH:mm:ss");
};
declare_global_extension(QDateTime, QString, str2date, date2str);

static const char* strings[] = {"ERROR",       "NOT_STARTED", "CHECKING_IN",
                                "IN_PROGRESS", "CANCELED",    "ENDED"};
static EventState str2eventstate(const QString& str) {
    for (int i = 0; i < 6; i++)
        if (str == strings[i])
            return static_cast<EventState>(i);
    return EventState::Uninitialised;
}
static auto eventstate2str(EventState state) {
    return strings[int(state)];
}
declare_global_extension(EventState, QString, str2eventstate, eventstate2str);

register_object_member(DTO_Evento, "id", id);
register_object_member(DTO_Evento, "title", title);
register_object_member(DTO_Evento, "description", description);
register_object_member(DTO_Evento, "gmtEventStart", gmtEventStart);
register_object_member(DTO_Evento, "gmtEventEnd", gmtEventEnd);
register_object_member(DTO_Evento, "gmtRegistrationStart", gmtRegistrationStart);
register_object_member(DTO_Evento, "gmtRegistrationEnd", gmtRegistrationEnd);
register_object_member(DTO_Evento, "eventType", type);
register_object_member(DTO_Evento, "location", location);
register_object_member(DTO_Evento, "tag", tag);
register_object_member(DTO_Evento, "state", state);
register_object_member(DTO_Evento, "departments", departments);
declare_object(DTO_Evento, object_member(DTO_Evento, id), object_member(DTO_Evento, title),
               object_member(DTO_Evento, description), object_member(DTO_Evento, gmtEventStart),
               object_member(DTO_Evento, gmtEventEnd),
               object_member(DTO_Evento, gmtRegistrationStart),
               object_member(DTO_Evento, gmtRegistrationEnd), object_member(DTO_Evento, type),
               object_member(DTO_Evento, location), object_member(DTO_Evento, tag),
               object_member(DTO_Evento, state), object_member(DTO_Evento, departments));

std::vector<DTO_Evento> asEventoDTOArray(const QJsonValue& rootValue) {
    std::vector<DTO_Evento> result;

    if (rootValue.isArray()) {
        declare_top_deserialiser(result, deserialiser);
        deserialiser.assign(rootValue);
    }
    return result;
}

// eventFetch
QFuture<EventoResult<std::vector<DTO_Evento>>> EventoNetworkClient::getUndertakingList() {
    auto url = endpoint(QStringLiteral("/event/conducting"));
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<std::vector<DTO_Evento>> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            return asEventoDTOArray(result.take());
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<std::vector<DTO_Evento>>> EventoNetworkClient::getLatestList() {
    auto url = endpoint(QStringLiteral("/event/newest"));
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<std::vector<DTO_Evento>> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            return asEventoDTOArray(result.take());
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<std::vector<DTO_Evento>>> EventoNetworkClient::getRegisteredList() {
    auto url = endpoint(QStringLiteral("/user/registered"));
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<std::vector<DTO_Evento>> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            return asEventoDTOArray(result.take());
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<std::vector<DTO_Evento>>> EventoNetworkClient::getSubscribedList() {
    auto url = endpoint(QStringLiteral("/user/subscribed"));
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<std::vector<DTO_Evento>> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            return asEventoDTOArray(result.take());
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<std::vector<DTO_Evento>>> EventoNetworkClient::getHistoryList() {
    auto url = endpoint(QStringLiteral("/event/history"));
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<std::vector<DTO_Evento>> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            return asEventoDTOArray(result.take());
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<std::vector<DTO_Evento>>> EventoNetworkClient::getEventListInPage(int page,
                                                                                       int size) {
    auto url = endpoint(QStringLiteral("/event/list"), [&](QUrlQuery& params) {
        params.addQueryItem("page", QString::number(page));
        params.addQueryItem("size", QString::number(size));
    });
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<std::vector<DTO_Evento>> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            return asEventoDTOArray(result.take());
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<std::vector<DTO_Evento>>>
    EventoNetworkClient::getDepartmentEventList(int departmentId) {
    QUrlQuery params;
    params.addQueryItem("departmentId", QString::number(departmentId));
    params.addQueryItem("typeId", "");
    params.addQueryItem("time", "");
    auto url = endpoint(QStringLiteral("/event/list"));
    auto future = this->post(url, params);
    return QtConcurrent::run([=]() -> EventoResult<std::vector<DTO_Evento>> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            return asEventoDTOArray(result.take());
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<std::vector<DTO_Evento>>>
    EventoNetworkClient::getEventListByTime(const QString& time) {
    QUrlQuery params;
    params.addQueryItem("time", time);
    params.addQueryItem("typeId", "");
    params.addQueryItem("departmentId", "");
    auto url = endpoint(QStringLiteral("/event/list"));
    auto future = this->post(url, params);
    return QtConcurrent::run([=]() -> EventoResult<std::vector<DTO_Evento>> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            return asEventoDTOArray(result.take());
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<DTO_Evento>> EventoNetworkClient::getEventById(EventoID event) {
    auto url = endpoint(QStringLiteral("/event/info"), [=](QUrlQuery& params) {
        params.addQueryItem("eventId", QString::number(event));
    });
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<DTO_Evento> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            auto rootValue = result.take();
            if (rootValue.isObject()) {
                DTO_Evento dto;
                declare_top_deserialiser(dto, deserialiser);
                deserialiser.assign(rootValue);
                return dto;
            }
            return EventoException(EventoExceptionCode::JsonError, "Format Error!");
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<std::vector<DTO_Feedback>>>
    EventoNetworkClient::getFeedbackList(EventoID eventoId) {
    auto url = endpoint(QStringLiteral("/feedback/list"), [&](QUrlQuery params) {
        params.addQueryItem("eventId", QString::number(eventoId));
    });
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<std::vector<DTO_Feedback>> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            auto rootValue = result.take();
            std::vector<DTO_Feedback> result;
            if (rootValue.isArray()) {
                declare_top_deserialiser(result, deserialiser);
                deserialiser.assign(rootValue);
                return result;
            } else if (rootValue.isNull())
                return result;
            return EventoException(EventoExceptionCode::JsonError, "Format Error!");
        } else {
            return {result.code(), result.message()};
        }
    });
}

register_object_member(DTO_FeedbackSummary, "eventId", eventId);
register_object_member(DTO_FeedbackSummary, "average", aveScore);
register_object_member(DTO_FeedbackSummary, "subscribeNum", subscribedNum);
register_object_member(DTO_FeedbackSummary, "registrationNum", registeredNum);
register_object_member(DTO_FeedbackSummary, "participantNum", checkedNum);
register_object_member(DTO_FeedbackSummary, "feedbacks", feedbacks);
declare_object(DTO_FeedbackSummary, object_member(DTO_FeedbackSummary, eventId),
               object_member(DTO_FeedbackSummary, aveScore),
               object_member(DTO_FeedbackSummary, subscribedNum),
               object_member(DTO_FeedbackSummary, registeredNum),
               object_member(DTO_FeedbackSummary, checkedNum),
               object_member(DTO_FeedbackSummary, feedbacks));

QFuture<EventoResult<DTO_FeedbackSummary>>
    EventoNetworkClient::getFeedbackSummary(EventoID eventoId) {
    auto url = endpoint(QStringLiteral("/feedback/event"), [=](QUrlQuery& params) {
        params.addQueryItem("eventId", QString::number(eventoId));
    });
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<DTO_FeedbackSummary> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            auto rootValue = result.take();
            if (rootValue.isObject()) {
                DTO_FeedbackSummary dto;
                declare_top_deserialiser(dto, deserialiser);
                deserialiser.assign(rootValue);
                return dto;
            }
            return EventoException(EventoExceptionCode::JsonError, "Format Error!");
        } else {
            return {result.code(), result.message()};
        }
    });
}

register_object_member(FeedbackNum, "eventId", eventId);
register_object_member(FeedbackNum, "title", title);
register_object_member(FeedbackNum, "feedbackCount", feedbackCount);
declare_object(FeedbackNum, object_member(FeedbackNum, eventId), object_member(FeedbackNum, title),
               object_member(FeedbackNum, feedbackCount));

QFuture<EventoResult<std::pair<int, std::vector<FeedbackNum>>>>
    EventoNetworkClient::getFeedbackSummaryListInPage(int page) {
    auto url = endpoint(QStringLiteral("/feedback/num"), [=](QUrlQuery& params) {
        params.addQueryItem("page", QString::number(page));
        params.addQueryItem("size", QStringLiteral("10"));
    });
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<std::pair<int, std::vector<FeedbackNum>>> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            auto rootValue = result.take();
            if (rootValue.isObject()) {
                std::vector<FeedbackNum> dto;
                int total;
                declare_deserialiser("result", dto, dto_);
                declare_deserialiser("total", total, total_);
                JsonDeserialise::JsonDeserialiser res(dto_, total_);
                res.deserialise(rootValue.toObject());
                return std::make_pair(total, dto);
            }
            return EventoException(EventoExceptionCode::JsonError, "Format Error!");
        } else {
            return {result.code(), result.message()};
        }
    });
}

register_object_member(DTO_Slide, "id", id);
register_object_member(DTO_Slide, "title", title);
register_object_member(DTO_Slide, "link", link);
register_object_member(DTO_Slide, "url", url);
declare_object(DTO_Slide, object_member(DTO_Slide, id), object_member(DTO_Slide, title),
               object_member(DTO_Slide, link), object_member(DTO_Slide, url));

QFuture<EventoResult<std::vector<DTO_Slide>>> EventoNetworkClient::getSlideList() {
    // TODO: implement
    return {};
}

QFuture<EventoResult<std::vector<DTO_Slide>>> EventoNetworkClient::getEventSlideList(EventoID id) {
    auto url = endpoint(QStringLiteral("/slide/event/list"), [=](QUrlQuery& params) {
        params.addQueryItem("eventId", QString::number(id));
    });
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<std::vector<DTO_Slide>> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            auto rootValue = result.take();
            std::vector<DTO_Slide> result;
            if (rootValue.isArray()) {
                declare_top_deserialiser(result, deserialiser);
                deserialiser.assign(rootValue);
                return result;
            } else if (rootValue.isNull())
                return result;
            return EventoException(EventoExceptionCode::JsonError, "Format Error!");
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<std::vector<DTO_Slide>>> EventoNetworkClient::getHomeSlideList(int size) {
    auto url = endpoint(QStringLiteral("/slide/home/list"), [=](QUrlQuery& params) {
        params.addQueryItem("size", QString::number(size));
    });
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<std::vector<DTO_Slide>> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            auto rootValue = result.take();
            std::vector<DTO_Slide> result;
            if (rootValue.isArray()) {
                declare_top_deserialiser(result, deserialiser);
                deserialiser.assign(rootValue);
                return result;
            } else if (rootValue.isNull())
                return result;
            return EventoException(EventoExceptionCode::JsonError, "Format Error!");
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<std::vector<EventType>>> EventoNetworkClient::getTypeList() {
    auto url = endpoint(QStringLiteral("/admin/types"));
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<std::vector<EventType>> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            auto rootValue = result.take();
            std::vector<EventType> result;
            if (rootValue.isArray()) {
                declare_top_deserialiser(result, deserialiser);
                deserialiser.assign(rootValue);
                return result;
            } else if (rootValue.isNull())
                return result;
            return EventoException(EventoExceptionCode::JsonError, "Format Error!");
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<QString>> EventoNetworkClient::getLocationList() {
    auto url = endpoint(QStringLiteral("/admin/locations"));
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<QString> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            return QString(QJsonDocument(result.take().toArray())
                               .toJson(QJsonDocument::Compact)
                               .toStdString()
                               .c_str());
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<QString>> EventoNetworkClient::getDepartmentList() {
    auto url = endpoint(QStringLiteral("/admin/departments"));
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<QString> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            return QString(QJsonDocument(result.take().toArray())
                               .toJson(QJsonDocument::Compact)
                               .toStdString()
                               .c_str());
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<QString>> EventoNetworkClient::getSubscribedDepartmentList() {
    auto url = endpoint("/user/subscribe/departments");
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<QString> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            auto arr = result.take().toArray();
            QString str = "[";
            for (const auto& i : arr) {
                str += (QString::number(i.toObject()["id"].toInt()) + ", ");
            }
            if (!arr.isEmpty())
                str.remove(str.size() - 2, 2);
            str += "]";
            return str;
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<QString>> EventoNetworkClient::getQRCode(EventoID eventId) {
    auto url = endpoint(QStringLiteral("/event/authcode"), [=](QUrlQuery& params) {
        params.addQueryItem("eventId", QString::number(eventId));
    });
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<QString> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            return result.take().toString();
        } else {
            return {result.code(), result.message()};
        }
    });
}

// eventUpload
QFuture<EventoResult<bool>> EventoNetworkClient::checkIn(EventoID event, const QString& code) {
    auto url = endpoint(QStringLiteral("/event/authcode"), [=, &code](QUrlQuery& params) {
        params.addQueryItem("eventId", QString::number(event));
        params.addQueryItem("code", code);
    });
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<bool> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            return {};
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<bool>> EventoNetworkClient::feedbackEvent(const DTO_Feedback& feedback) {
    QUrlQuery params;
    params.addQueryItem("content", feedback.content);
    params.addQueryItem("score", QString::number(feedback.score));
    params.addQueryItem("eventId", QString::number(feedback.eventId));
    auto url = endpoint(QStringLiteral("/feedback/info"));
    auto future = this->post(url, params);
    return QtConcurrent::run([=]() -> EventoResult<bool> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            return {};
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<bool>> EventoNetworkClient::registerEvent(EventoID event, bool selection) {
    auto url = endpoint(QStringLiteral("/user/register"), [=](QUrlQuery& params) {
        params.addQueryItem("eventId", QString::number(event));
        params.addQueryItem("isRegister",
                            selection ? QStringLiteral("true") : QStringLiteral("false"));
    });
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<bool> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            return {};
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<bool>> EventoNetworkClient::subscribeEvent(EventoID event, bool selection) {
    auto url = endpoint(QStringLiteral("/user/subscribe"), [=](QUrlQuery& params) {
        params.addQueryItem("eventId", QString::number(event));
        params.addQueryItem("isSubscribe",
                            selection ? QStringLiteral("true") : QStringLiteral("false"));
    });
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<bool> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            return {};
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<int>> EventoNetworkClient::hasFeedbacked(EventoID event) {
    auto url = endpoint(QStringLiteral("/feedback/user/info"), [=](QUrlQuery& params) {
        params.addQueryItem("eventId", QString::number(event));
    });
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<int> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            return result.take()["data"].isNull();
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<bool>> EventoNetworkClient::deleteEvent(EventoID event) {
    auto url = endpoint(QStringLiteral("/event/info"), [=](QUrlQuery& params) {
        params.addQueryItem("eventId", QString::number(event));
    });
    auto future = this->deleteResource(url);
    return QtConcurrent::run([=]() -> EventoResult<bool> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            return {};
        } else {
            return {result.code(), result.message()};
        }
    });
}

struct Request_Evento {
    QString title;
    QString description;
    QString eventStart;
    QString eventEnd;
    QString registerStart;
    QString registerEnd;
    int typeId;
    int locationId;
    QVariantList departmentIds;
    QString tag;

    QJsonValue serialise() {
        declare_serialiser("title", title, title_);
        declare_serialiser("description", description, description_);
        declare_serialiser("gmtEventStart", eventStart, eventStart_);
        declare_serialiser("gmtEventEnd", eventEnd, eventEnd_);
        declare_serialiser("gmtRegistrationStart", registerStart, registerStart_);
        declare_serialiser("gmtRegistrationEnd", registerEnd, registerEnd_);
        declare_serialiser("typeId", typeId, typeId_);
        declare_serialiser("locationId", locationId, locationId_);
        declare_serialiser("tag", tag, tag_);
        auto convertor = [](const QVariantList& src) -> QJsonValue {
            QJsonArray arr;
            for (const auto& i : src) {
                QJsonObject obj;
                obj.insert("id", QString::number(i.toInt()));
                arr.append(obj);
            }
            return arr;
        };
        declare_one_direction_extension_serialiser("departments", departmentIds, departmentIds_,
                                                   convertor);
        JsonDeserialise::JsonSerialiser serialiser(title_, description_, eventStart_, eventEnd_,
                                                   registerStart_, registerEnd_, typeId_,
                                                   locationId_, tag_, departmentIds_);
        return serialiser.serialise_to_json();
    }
};

QFuture<EventoResult<bool>> EventoNetworkClient::createEvent(
    const QString& title, const QString& description, const QString& eventStart,
    const QString& eventEnd, const QString& registerStart, const QString& registerEnd, int typeId,
    int locationId, const QVariantList& departmentIds, const QString& tag) {

    auto url = endpoint(QStringLiteral("/event/info"));
    auto future = this->post(
        url, QJsonDocument(Request_Evento{title, description, eventStart, eventEnd, registerStart,
                                          registerEnd, typeId, locationId, departmentIds, tag}
                               .serialise()
                               .toObject()));
    return QtConcurrent::run([=]() -> EventoResult<bool> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            return {};
        } else {
            return {result.code(), result.message()};
        }
    });
}

struct Request_EventoPatch : public Request_Evento {
    EventoID id;

    QJsonValue serialise() {
        auto obj = Request_Evento::serialise().toObject();
        obj.insert("id", id);
        return obj;
    }
};

QFuture<EventoResult<bool>> EventoNetworkClient::editEvent(
    EventoID event, const QString& title, const QString& description, const QString& eventStart,
    const QString& eventEnd, const QString& registerStart, const QString& registerEnd, int typeId,
    int locationId, const QVariantList& departmentIds, const QString& tag) {
    auto url = endpoint(QStringLiteral("/event/info"));
    auto future = this->put(
        url, QJsonDocument(Request_Evento{title, description, eventStart, eventEnd, registerStart,
                                          registerEnd, typeId, locationId, departmentIds, tag}
                               .serialise()
                               .toObject()));
    return QtConcurrent::run([=]() -> EventoResult<bool> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            return {};
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<bool>> EventoNetworkClient::cancelEvent(EventoID event) {
    auto url = endpoint(QStringLiteral("/event/info"), [=](QUrlQuery& params) {
        params.addQueryItem("eventId", QString::number(event));
    });
    QUrlQuery params;
    params.addQueryItem("id", QString::number(event));
    auto future = this->patch(url, params);
    return QtConcurrent::run([=]() -> EventoResult<bool> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            return {};
        } else {
            return {result.code(), result.message()};
        }
    });
}

register_object_member(DTO_UserBrief, "userId", userId);
register_object_member(DTO_UserBrief, "email", email);
declare_object(DTO_UserBrief, object_member(DTO_UserBrief, userId),
               object_member(DTO_UserBrief, email));

QFuture<EventoResult<std::vector<DTO_UserBrief>>>
    EventoNetworkClient::getEventManagerList(EventoID eventoId) {
    auto url = endpoint(QStringLiteral("/permission/event/managers"), [=](QUrlQuery& params) {
        params.addQueryItem("eventId", QString::number(eventoId));
    });
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<std::vector<DTO_UserBrief>> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            auto rootValue = result.take();
            std::vector<DTO_UserBrief> result;
            if (rootValue.isArray()) {
                declare_top_deserialiser(result, deserialiser);
                deserialiser.assign(rootValue);
                return result;
            } else if (rootValue.isNull())
                return result;
            return EventoException(EventoExceptionCode::JsonError, "Format Error!");
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<std::vector<DTO_UserBrief>>> EventoNetworkClient::getAdminUserList() {
    auto url = endpoint(QStringLiteral("/permission/admins"));
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<std::vector<DTO_UserBrief>> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            auto rootValue = result.take();
            std::vector<DTO_UserBrief> result;
            if (rootValue.isArray()) {
                declare_top_deserialiser(result, deserialiser);
                deserialiser.assign(rootValue);
                return result;
            } else if (rootValue.isNull())
                return result;
            return EventoException(EventoExceptionCode::JsonError, "Format Error!");
        } else {
            return {result.code(), result.message()};
        }
    });
}

QFuture<EventoResult<QString>> EventoNetworkClient::getAdminPermissionTreeData() {
    // TODO: implement
    return {};
}

QFuture<EventoResult<QString>> EventoNetworkClient::getManagerPermissionTreeData() {
    // TODO: implement
    return {};
}

QFuture<EventoResult<QVariantList>> EventoNetworkClient::getAdminEvents(const QString& userId) {
    auto url = endpoint(QStringLiteral("/permission/manager/events"), [=](QUrlQuery& params) {
        params.addQueryItem("userId", userId);
    });
    auto future = this->get(url);
    return QtConcurrent::run([=]() -> EventoResult<QVariantList> {
        auto f(future);
        auto result = f.takeResult();
        if (result) {
            auto rootValue = result.take();
            if (rootValue.isArray())
                return rootValue.toArray().toVariantList();
            else
                return QVariantList{};
        } else {
            return {result.code(), result.message()};
        }
    });
}
