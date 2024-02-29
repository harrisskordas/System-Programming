
#ifndef PROD_CONS_H
#define PROD_CONS_H

typedef struct pool_t pool_t;

void pool_initialize(pool_t * pool_ptr, int pool_size);

void pool_place(pool_t * pool, int data);

int pool_obtain(pool_t * pool);

void pool_destroy(pool_t * pool);

#endif /* PROD_CONS_H */

