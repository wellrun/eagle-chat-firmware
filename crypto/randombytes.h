#ifndef RANDOMBYTES_H
#define RANDOMBYTES_H

/* Function prototype for the randombytes function that AVRNaCl expects.
 * Do not change this signature, as that would break AVRNaCl.
 */
void randombytes(unsigned char *x,unsigned long long xlen);

#endif
