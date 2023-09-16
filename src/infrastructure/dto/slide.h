#ifndef DTO_SLIDE_H
#define DTO_SLIDE_H

#include "types.h"

struct DTO_Slide {
    int id;
    EventoID eventId;
    QString title;
    QString link;
    Image url;
};

#endif // DTO_SLIDE_H
