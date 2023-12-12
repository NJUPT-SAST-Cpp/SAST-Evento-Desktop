#ifndef USER_SERVICE_H
#define USER_SERVICE_H

#include "types.h"

#include <QFuture>
#include <shared_mutex>

class UserService {
private:
    UserService() = default;

public:
    static UserService& getInstance() {
        static UserService singleton;
        return singleton;
    }

    void checkIn(EventoID eventId, const QString& code, bool refreshAll);
    void subscribeEvento(EventoID id, bool selection);
    void registerEvento(EventoID id, bool selection);
};

#endif // USER_SERVICE_H
