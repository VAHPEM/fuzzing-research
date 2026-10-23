/* dxffuzz.c — DXF-only libFuzzer harness for GNU LibreDWG.
 *
 * libredwg's OSS-Fuzz harness (examples/llvmfuzz.c) auto-detects the input
 * format by its first bytes ("AC"->DWG decode, "{"->JSON, else->DXF). Since the
 * OSS-Fuzz corpus is DWG-dominated, the DXF importer (in_dxf.c, ~15k lines) is
 * shallow-fuzzed. This harness forces the DXF path so every exec exercises it.
 *
 * Build: see README.md (link against a sanitized libredwg static lib). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include <dwg.h>
#include "decode.h"
#include "bits.h"
#include "in_dxf.h"
int LLVMFuzzerTestOneInput (const unsigned char *data, size_t size);
int
LLVMFuzzerTestOneInput (const unsigned char *data, size_t size)
{
  Dwg_Data dwg;
  Bit_Chain dat;
  unsigned char *copy;
  if (size < 2 || size > 10 * 1024 * 1024)
    return 0;
  copy = (unsigned char *)malloc (size + 1);
  if (!copy)
    return 0;
  memcpy (copy, data, size);
  copy[size] = '\0';                 /* DXF parser uses strtol/strchr on text */
  memset (&dwg, 0, sizeof (dwg));
  memset (&dat, 0, sizeof (dat));
  dat.chain = copy;
  dat.size = size + 1;
  dat.byte = 0;
  dat.bit = 0;
  (void)dwg_read_dxf (&dat, &dwg);   /* force the DXF importer path */
  dwg_free (&dwg);
  free (copy);
  return 0;
}
