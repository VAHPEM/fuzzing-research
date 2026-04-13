#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libexif/exif-data.h"
#include "libexif/exif-loader.h"
#include "libexif/exif-content.h"
#include "libexif/exif-entry.h"
#include "libexif/exif-format.h"
#include "libexif/exif-tag.h"
#include "libexif/exif-mnote-data.h"

__AFL_FUZZ_INIT();

static void content_foreach_func(ExifEntry *entry, void *data) {
    char buf[2001];
    (void)data;

    buf[sizeof(buf) - 1] = 0;
    buf[sizeof(buf) - 2] = 0;

    exif_tag_get_name(entry->tag);
    exif_format_get_name(entry->format);
    exif_entry_get_value(entry, buf, sizeof(buf) - 1);

    if (buf[sizeof(buf) - 2] != 0) {
        abort();
    }
}

static void data_foreach_func(ExifContent *content, void *callback_data) {
    exif_content_foreach_entry(content, content_foreach_func, callback_data);
}

static void test_exif_data(ExifData *d) {
    unsigned int i, c;
    char v[1024];
    ExifMnoteData *md;

    if (!d) return;

    exif_data_foreach_content(d, data_foreach_func, NULL);

    md = exif_data_get_mnote_data(d);
    if (!md) return;

    exif_mnote_data_ref(md);
    exif_mnote_data_unref(md);

    c = exif_mnote_data_count(md);
    for (i = 0; i < c; i++) {
        if (!exif_mnote_data_get_name(md, i)) continue;
        exif_mnote_data_get_name(md, i);
        exif_mnote_data_get_title(md, i);
        exif_mnote_data_get_description(md, i);
        exif_mnote_data_get_value(md, i, v, sizeof(v));
    }
}

int main(void) {
#ifdef __AFL_HAVE_MANUAL_CONTROL
    __AFL_INIT();
#endif

    unsigned char *buf = __AFL_FUZZ_TESTCASE_BUF;

    while (__AFL_LOOP(10000)) {
        int len = __AFL_FUZZ_TESTCASE_LEN;
        ExifLoader *loader;
        ExifData *d;
        unsigned char *out = NULL;
        unsigned int out_size = 0;

        if (len <= 0) {
            continue;
        }

        loader = exif_loader_new();
        if (!loader) {
            continue;
        }

        exif_loader_write(loader, buf, len);
        d = exif_loader_get_data(loader);
        exif_loader_unref(loader);

        if (!d) {
            continue;
        }

        test_exif_data(d);

        exif_data_save_data(d, &out, &out_size);
        free(out);
        out = NULL;

        exif_data_set_byte_order(d, EXIF_BYTE_ORDER_INTEL);
        exif_data_save_data(d, &out, &out_size);
        free(out);

        exif_data_unref(d);
    }

    return 0;
}
