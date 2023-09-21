#ifndef EVENTO_NETWORK_CLIENT_H
#define EVENTO_NETWORK_CLIENT_H

#include <QByteArray>
#include <QFuture>
#include <QJsonDocument>
#include <QJsonValue>
#include <QNetworkReply>
#include <QString>
#include <QUrlQuery>
#include <dto/evento.h>
#include <dto/feedback.h>
#include <dto/permission.h>
#include <dto/slide.h>
#include <dto/user.h>
#include <dto/user_brief.h>
#include <evento_exception.h>
#include <result.h>

#include <QNetworkAccessManager>

class EventoNetworkClient {
private:
    QByteArray tokenBytes;
    QNetworkAccessManager manager;

protected:
    QUrl endpoint(const QString &endpoint);
    QUrl endpoint(const QString &endpoint, const QUrlQuery &params);

    template <typename TParamsBuilder, typename = std::enable_if_t<std::is_invocable_v<TParamsBuilder, QUrlQuery &>>>
    QUrl endpoint(const QString &endpoint, TParamsBuilder paramsBuilder)
    {
        QUrlQuery query;
        paramsBuilder(query);
        return this->endpoint(endpoint, query);
    }

    QFuture<EventoResult<QJsonValue>> get(const QUrl &url);

    QFuture<EventoResult<QJsonValue>> post(const QUrl &url, const QString &contentType, const QByteArray &requestData);
    QFuture<EventoResult<QJsonValue>> post(const QUrl &url, const QUrlQuery &requestData);
    QFuture<EventoResult<QJsonValue>> post(const QUrl &url, const QJsonDocument &requestData);

    QFuture<EventoResult<QJsonValue>> put(const QUrl &url, const QString &contentType, const QByteArray &requestData);
    QFuture<EventoResult<QJsonValue>> put(const QUrl &url, const QUrlQuery &requestData);
    QFuture<EventoResult<QJsonValue>> put(const QUrl &url, const QJsonDocument &requestData);

    QFuture<EventoResult<QJsonValue>> patch(const QUrl &url, const QString &contentType, const QByteArray &requestData);
    QFuture<EventoResult<QJsonValue>> patch(const QUrl &url, const QUrlQuery &requestData);
    QFuture<EventoResult<QJsonValue>> patch(const QUrl &url, const QJsonDocument &requestData);

    QFuture<EventoResult<QJsonValue>> deleteResource(const QUrl &url);

public:
    // userFetch
    QFuture<EventoResult<QStringList>> getAdminPermission();
    QFuture<EventoResult<QStringList>> getManagerPermission(const EventoID &eventoId);
    QFuture<EventoResult<QStringList>> getPermittedEvent();
    QFuture<EventoResult<DTO_Permission>> getEventPermission(EventoID event);
    QFuture<EventoResult<DTO_User>> getUserInfo(const UserID &id);
    QFuture<EventoResult<ParticipationStatus>> getUserParticipate(const EventoID &eventoId);
    QFuture<EventoResult<DTO_Feedback>> getFeedbackInfo(const EventoID &eventoId);

    // eventFetch
    QFuture<EventoResult<std::vector<DTO_Evento>>> getUndertakingList();
    QFuture<EventoResult<std::vector<DTO_Evento>>> getLatestList();
    QFuture<EventoResult<std::vector<DTO_Evento>>> getRegisteredList();
    QFuture<EventoResult<std::vector<DTO_Evento>>> getSubscribedList();
    QFuture<EventoResult<std::vector<DTO_Evento>>> getHistoryList();
    QFuture<EventoResult<std::vector<DTO_Evento>>> getEventList(int page, int size);
    QFuture<EventoResult<std::vector<DTO_Evento>>> getDepartmentEventList(int departmentId);
    QFuture<EventoResult<std::vector<DTO_Evento>>> getEventByTime(const QString& time);
    QFuture<EventoResult<DTO_Evento>> getEvent(EventoID event);
    QFuture<EventoResult<std::vector<DTO_Feedback>>> getFeedbackList(EventoID eventoId);
    QFuture<EventoResult<std::vector<DTO_Slide>>> getSlideList();
    QFuture<EventoResult<std::vector<DTO_Slide>>> getEventSlideList(EventoID id);
    QFuture<EventoResult<std::vector<DTO_Slide>>> getHomeSlideList(int size);
    QFuture<EventoResult<std::vector<EventType>>> getTypeList();
    QFuture<EventoResult<QString>> getLocationList();
    QFuture<EventoResult<QString>> getDepartmentList();
    QFuture<EventoResult<QString>> getQRCode(EventoID eventId);

    // eventUpload
    QFuture<EventoResult<bool>> checkInEvent(EventoID event, const QString &code);
    QFuture<EventoResult<bool>> feedbackEvent(const DTO_Feedback &feedback);
    QFuture<EventoResult<bool>> subscribEvent(EventoID event, bool targetState);
    QFuture<EventoResult<bool>> isFeedbacked(EventoID event);
    QFuture<EventoResult<bool>> cancelEvent(EventoID event);
    QFuture<EventoResult<bool>> deleteEvent(EventoID event);

    // adminFetch
    QFuture<EventoResult<std::vector<DTO_Evento>>> getQualifiedEvent(int type = -1, const std::vector<int> &dep = std::vector<int>(), const QDate &day = QDate());
    QFuture<EventoResult<QStringList>> getActionStateList();
    QFuture<EventoResult<QStringList>> getActionList();
    QFuture<EventoResult<std::vector<DTO_UserBrief>>> getEventManagerList(const EventoID &eventoId);
    QFuture<EventoResult<std::vector<DTO_UserBrief>>> getAdminUserList();
    QFuture<EventoResult<QString>> getAdminPermissionTreeData();
    QFuture<EventoResult<QString>> getManagerPermissionTreeData();

    EventoNetworkClient() = default;
    ~EventoNetworkClient() = default;
};
#endif // EVENTO_NETWORK_CLIENT_H
