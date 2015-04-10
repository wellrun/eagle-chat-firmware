//
//  fifo_master_race.h
//  FifoMasterRace
//
//  Created by Kevin Ward on 4/10/15.
//  Copyright (c) 2015 Kevin Ward. All rights reserved.
//

#ifndef __FifoMasterRace__fifo_master_race__
#define __FifoMasterRace__fifo_master_race__
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t *data;
    uint16_t size;
    uint16_t unit_length;
    uint16_t h;
    uint16_t t;
    uint16_t stored;
} one_true_fifo;


void fifo_setup(one_true_fifo *f, uint16_t size, uint16_t unit_length, uint8_t *data);

void fifo_reset(one_true_fifo *f);


uint8_t *fifo_peek(one_true_fifo *f);

void fifo_skip(one_true_fifo *f);


void fifo_write(one_true_fifo *f, uint8_t *data, uint16_t len);


bool fifo_isFull(one_true_fifo *f);

bool fifo_isEmpty(one_true_fifo *f);

    
#ifdef __cplusplus
}
#endif

#endif /* defined(__FifoMasterRace__fifo_master_race__) */
