#include "repositoryimpl.h"

QFuture<EventoResult<std::vector<DTO_Slide>>> repositoryImpl::getHomeSlideList(const int size)
{
    return QtConcurrent::run([this, size]{
        std::vector<DTO_Slide> res;

        int countNum;
        if(size > slide_data_list.size()){
            countNum = slide_data_list.size();
        }
        else{
            countNum = size;
        }
        for(int i = 0; i < countNum; i++){
            slide_data unit = slide_data_list.at(i);
            res.push_back(DTO_Slide{
                unit.id.toInt(),
                unit.event_id.toInt(),
                unit.title,
                unit.link,
                unit.url
            });
        }
        return EventoResult(std::move(res));
    });
}


QFuture<EventoResult<std::vector<DTO_Evento>>> repositoryImpl::getUndertakingList()
{
    return readEventoByState("3");
}

QFuture<EventoResult<std::vector<DTO_Evento>>> repositoryImpl::getLatestList()
{
    return readEventoByState("1");
}

QFuture<EventoResult<std::vector<DTO_Evento>>> repositoryImpl::getRegisteredList()
{
    return QtConcurrent::run([this]{
        std::vector<DTO_Evento> res;

        for (int i = 0; i < participate_data_list.size(); i++) {
            if (!participate_data_list.at(i).user_id.compare("B22041234") && !participate_data_list.at(i).is_registration.compare("true")) {
                res.push_back(readEvento(participate_data_list.at(i).event_id.toInt()));
            }
        }

        return EventoResult(std::move(res));
    });
}

QFuture<EventoResult<std::vector<DTO_Evento>>> repositoryImpl::getSubscribedList()
{
    return QtConcurrent::run([this]{
        std::vector<DTO_Evento> res;

        for(int i = 0; i<participate_data_list.size(); i++) {
            if(!participate_data_list.at(i).user_id.compare("B22041234")
                && !participate_data_list.at(i).is_subscribe.compare("true")) {
                res.push_back(readEvento(participate_data_list.at(i).event_id.toInt()));
            }
        }

        return EventoResult(std::move(res));
    });
}

QFuture<EventoResult<std::vector<DTO_Evento>>> repositoryImpl::getHistoryList()
{
    return QtConcurrent::run([this]{
        std::vector<DTO_Evento> res;

        for(int i = 0; i<participate_data_list.size(); i++) {
            if(!participate_data_list.at(i).is_participate.compare("true")) {
                res.push_back(readEvento(participate_data_list.at(i).event_id.toInt()));
            }
        }

        return EventoResult(std::move(res));
    });
}

QFuture<EventoResult<std::vector<DTO_Evento>>> repositoryImpl::getEventListInPage(int page, int size)
{
    return QtConcurrent::run([=]{
        std::vector<DTO_Evento> res;
        int beginId = (page - 1) * size +1;
        int endId = beginId + (size - 1);

        if(beginId > event_data_list.size()){
            return EventoResult<std::vector<DTO_Evento>>(EventoExceptionCode::UnexpectedError, "count too big");
        }
        if(endId > event_data_list.size()){
            endId = event_data_list.size();
        }
        for(int i = beginId; i <= endId; i++){
            res.push_back(readEvento(event_data_list.at(i-1).id.toInt()));
        }

        return EventoResult(std::move(res));
    });
}

QFuture<EventoResult<DTO_Evento>> repositoryImpl::getEventById(EventoID event)
{
    return QtConcurrent::run([=]{
        return EventoResult(std::move(readEvento(event)));
    });
}

QFuture<EventoResult<std::vector<DTO_Feedback>>> repositoryImpl::getFeedbackList(EventoID eventoId)
{
    return QtConcurrent::run([=] {
        std::vector<DTO_Feedback> res;
        for(int i = 0; i<feedback_data_list.size(); i++) {
            feedback_data unit = feedback_data_list.at(i);
            QString participateId = unit.participate_id;
            std::vector<Participation> participationList = readParticipation(std::pair<QString, QString>("id", participateId));
            if(participationList.at(0).event_id == eventoId){
                res.push_back(DTO_Feedback{
                    i,
                    participationList.at(0).event_id,
                    unit.score.toInt(),
                    unit.content
                });
            }
        }
        return EventoResult(std::move(res));
    });
}

/*
    virtual QFuture<EventoResult<DTO_FeedbackSummary>> getFeedbackSummary(EventoID eventoId) override;
    virtual QFuture<EventoResult<std::pair<int,std::vector<std::pair<int,int>>>>> getFeedbackSummaryListInPage(int page) override;
*/

QFuture<EventoResult<DTO_FeedbackSummary>> repositoryImpl::getFeedbackSummary(EventoID eventoId){
    return QtConcurrent::run([=] {
        std::vector<DTO_Feedback> feedbacks;
        feedbacks.push_back(DTO_Feedback{1,1,1,QString("so cool")});
        return EventoResult(DTO_FeedbackSummary{1,1,1,1,QString("so cool"),feedbacks});
    });
}

QFuture<EventoResult<std::pair<int,std::vector<std::pair<int,int>>>>> repositoryImpl::getFeedbackSummaryListInPage(EventoID eventoId){
    return QtConcurrent::run([=] {
        std::vector<std::pair<int,int>> a;
        a.push_back(std::make_pair(1,1));
        return EventoResult(std::make_pair(1,a));
    });
}

std::vector<DTO_Slide> repositoryImpl::getSlideList(EventoException &err)
{
    std::vector<DTO_Slide> res;

    for(int i = 0; i<slide_data_list.size(); i++) {
        slide_data unit = slide_data_list.at(i);
        res.push_back(DTO_Slide {
            unit.id.toInt(),
            unit.event_id.toInt(),
            unit.title,
            unit.link,
            unit.url
        });
    }
    return res;
}

std::vector<DTO_Slide> repositoryImpl::getEventSlideList(EventoID id, EventoException &err)
{
    std::vector<DTO_Slide> res;

    for(int i = 0; i<slide_data_list.size(); i++) {
        slide_data unit = slide_data_list.at(i);
        if(!unit.event_id.compare(QString::number(id))){
            res.push_back(DTO_Slide{
                unit.id.toInt(),
                unit.event_id.toInt(),
                unit.title,
                unit.link,
                unit.url
            });
        }
    }
    return res;
}

QFuture<EventoResult<QString>> repositoryImpl::getTypeList()
{
    return QtConcurrent::run([this]{
        QJsonArray res;
        for(int i = 0; i< type_data_list.size(); i++) {
            type_data unit = type_data_list.at(i);
            QJsonObject item;
            item.insert("id", unit.id.toInt());
            item.insert("name", unit.type_name);
            res.push_back(item);
        }
        return EventoResult(QString(QJsonDocument(res).toJson(QJsonDocument::Compact).toStdString().c_str()));
    });
}

QFuture<EventoResult<QString>> repositoryImpl::getLocationList()
{
    return QtConcurrent::run([this]{
        QJsonArray res;

        for(int i = 0; i<location_data_list.size(); i++) {
            if(!location_data_list.at(i).parent_id.compare("0")){
                res.push_back(formatToTree(location_data_list,
                                           location_data_list.at(i).id,
                                           location_data_list.at(i).location_name));
            }
        }
        return EventoResult(QString(QJsonDocument(res).toJson(QJsonDocument::Compact).toStdString().c_str()));
    });
}

QFuture<EventoResult<bool>> repositoryImpl::feedbackEvent(const DTO_Feedback &feedback)
{
    return QtConcurrent::run([=] {
        QString participationId;
        for(int i = 0; i < participate_data_list.size(); i++){
            if(!participate_data_list.at(i).event_id.compare(QString::number(feedback.eventId)) &&
                !participate_data_list.at(i).user_id.compare("B22041234")) {
                participationId = participate_data_list.at(i).id;
                break;
            }
        }

        for(int i = 0; i < feedback_data_list.size(); i++){
            if(!feedback_data_list.at(i).participate_id.compare(participationId)){
                return EventoResult{ EventoExceptionCode::UnexpectedError, "has feedback" };
            }
        }
        feedback_data_list.push_back(feedback_data{
            feedback.content,
            QString::number(feedback_data_list.size() + 1),
            participationId,
            QString::number(feedback.score),
            "B22041234"
        });
        return EventoResult();
    });
}

std::vector<DTO_Evento> repositoryImpl::getQualifiedEvent(EventoException& err, int type, const std::vector<int> &dep, const QDate &day)
{
    std::vector<DTO_Evento> res;
    std::vector<DTO_Evento> eventoList;
    if(type == -1) {
        eventoList = readAllEvento();
    }
    else {
        eventoList = readEventoByType(QString::number(type));
    }

    for(int i = 0; i < eventoList.size(); i++){
        QString day_str = eventoList.at(i).gmtEventStart.date().toString("yyyy-MM-dd hh:mm:ss").split(" ").at(0);
        QDate day_temp = QDate::fromString(day_str, "yyyy-MM-dd");
        int eventId = eventoList.at(i).id;
        if(day.isNull() || day_temp == day){
            if(dep.empty()){
                res.push_back(eventoList.at(i));
            }
            else{
                for(int j = 0; j < dep.size(); j++){
                    if(eventDepartmentMatch(eventId, dep.at(j))){
                        res.push_back(eventoList.at(i));
                    }
                }
            }
        }
    }
    return res;
}

QStringList repositoryImpl::getActionStateList(EventoException &err)
{
    return QStringList{
                       "ADMIN",
                       "MANAGER",
                       "PUBLIC",
                       "LOGIN",
                       "INVISIBLE"
    };
}

QStringList repositoryImpl::getActionList(EventoException &err)
{
    // FIXME: implement
    return QStringList();
}

std::vector<DTO_UserBrief> repositoryImpl::getEventManagerList(const EventoID &eventoId, EventoException& err)
{
    std::vector<DTO_UserBrief> res;

    for(int i = 0; i<manager_user_data_list.size(); i++){
        if(!manager_user_data_list.at(i).event_id.compare(QString::number(eventoId))){
            QStringList ids = manager_user_data_list.at(i).manager_id.split(" ");
            for(int j = 0; j<ids.count(); j++){
                res.push_back(readUserBrief(ids.at(j)));
            }
            return res;
        }
    }
    err = EventoException(EventoExceptionCode::UnexpectedError, "get_event_manager_list json error");
    return res;
}

std::vector<DTO_UserBrief> repositoryImpl::getAdminUserList(EventoException &err)
{
    std::vector<DTO_UserBrief> res;
    QByteArray file_str = admin_user_data;

    QJsonParseError jsonError;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(file_str, &jsonError));
    if(jsonError.error == QJsonParseError::NoError && jsonDoc.isArray()){
        QJsonArray jsonArr = jsonDoc.array();
        for(int i = 0; i < jsonArr.count(); i++){
            res.push_back(readUserBrief(jsonArr.at(i).toString()));
        }
    }
    return res;
}

QString repositoryImpl::getAdminPermissionTreeData(EventoException &err)
{
    return QString(QJsonDocument::fromJson(admin_permission_data).toJson(QJsonDocument::Compact).toStdString().c_str());
}

QString repositoryImpl::getManagerPermissionTreeData(EventoException &err)
{
    return QString(QJsonDocument::fromJson(manager_permission_data).toJson(QJsonDocument::Compact).toStdString().c_str());
}
