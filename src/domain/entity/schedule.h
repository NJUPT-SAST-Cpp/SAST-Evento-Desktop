#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "types.h"

struct Schedule {
    EventoID id;
    QString title;
    EventState state;
    QString department;
    QString location;
    QString date;
    QString startTime;
    QString endTime;
    bool isChecked;
    bool isFeedback;
};

#endif  // SCHEDULE_H
