#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "quirc.h"
#define MAX_DIM 1024
#define MAX_PIXELS (1u << 20)
static struct quirc *q = NULL;
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size){
    if (size < 4) return 0;
    int w = (data[0] << 8) | data[1];
    int h = (data[2] << 8) | data[3];
    const uint8_t *pix = data + 4; size_t pix_avail = size - 4;
    if (w < 1) w = 1; if (h < 1) h = 1;
    if (w > MAX_DIM) w = MAX_DIM; if (h > MAX_DIM) h = MAX_DIM;
    if ((uint64_t)w * (uint64_t)h > MAX_PIXELS) return 0;
    if (!q){ q = quirc_new(); if (!q) return 0; }
    if (quirc_resize(q, w, h) < 0) return 0;
    int bw = 0, bh = 0; uint8_t *buf = quirc_begin(q, &bw, &bh);
    if (!buf) return 0;
    size_t need = (size_t)bw * (size_t)bh;
    size_t copy = pix_avail < need ? pix_avail : need;
    memcpy(buf, pix, copy);
    if (copy < need) memset(buf + copy, 0, need - copy);
    quirc_end(q);
    int count = quirc_count(q);
    for (int i = 0; i < count; i++){
        struct quirc_code code; struct quirc_data qdata;
        quirc_extract(q, i, &code); quirc_decode(&code, &qdata);
        quirc_flip(&code); quirc_decode(&code, &qdata);
    }
    return 0;
}
