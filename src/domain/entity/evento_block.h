#ifndef EVENTO_BLOCK_H
#define EVENTO_BLOCK_H

#include "types.h"

struct EventoBlock {
    EventoID id;
    QString title;
    QString time;
    double rowStart; // index: 8:00 -> 0
    double rowEnd;   // index: 23:00 -> 15
    int columnStart; // index: Monday -> 0
    bool editable;
};

#endif // EVENTOBLOCK_H
