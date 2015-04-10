//
//  fifo_master_race.c
//  FifoMasterRace
//
//  Created by Kevin Ward on 4/10/15.
//  Copyright (c) 2015 Kevin Ward. All rights reserved.
//

#include "fifo_master_race.h"
#include <string.h>

void fifo_setup(one_true_fifo *f, uint16_t size, uint16_t unit_length, uint8_t *data) {
    f->data = data;
    f->unit_length = unit_length;
    f->size = size;
}

void fifo_reset(one_true_fifo *f) {
    f->h = 0;
    f->t = 0;
    f->stored = 0;
}


uint8_t *fifo_peek(one_true_fifo *f) {
    return &f->data[f->t];
}

void fifo_skip(one_true_fifo *f) {
    f->t += f->unit_length;
    if (f->t == f->size)
        f->t = 0;
    --(f->stored);
}

void fifo_write(one_true_fifo *f, uint8_t *data, uint16_t len) {
    memcpy(&f->data[f->h], data, len > 0 ? len : f->unit_length);
    f->h += f->unit_length;
    if (f->h == f->size)
        f->h = 0;
    ++(f->stored);
}


bool fifo_isFull(one_true_fifo *f) {
    return (f->stored != 0) && (f->t == f->h);
}

bool fifo_isEmpty(one_true_fifo *f) {
    return f->stored == 0;
}