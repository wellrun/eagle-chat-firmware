//
//  main.cpp
//  FifoMasterRace
//
//  Created by Kevin Ward on 4/10/15.
//  Copyright (c) 2015 Kevin Ward. All rights reserved.
//

#include "fifo_master_race.h"

int main(int argc, const char * argv[]) {
    // insert code here...

#define SLOTS       10
#define UNIT_LEN    250

    one_true_fifo fifo;

    uint8_t data_buf[SLOTS*UNIT_LEN];

    fifo_setup(&fifo, sizeof(data_buf), UNIT_LEN, data_buf);

    fifo_reset(&fifo);

    uint8_t s[] = "A really long string. Guys.";

    while(!fifo_isFull(&fifo)) {
        fifo_write(&fifo, s, sizeof(s));
    }

    while(!fifo_isEmpty(&fifo)) {
        printf("%s\n", fifo_peek(&fifo));
        fifo_skip(&fifo);
    }


    return 0;
}
