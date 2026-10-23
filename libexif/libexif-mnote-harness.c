/* libexif-mnote-harness.c  — structure-aware MakerNote harness (v2)
 *
 * Builds IFD0 -> ExifIFD(0x8769) -> MakerNote(0x927C) and splices the fuzzer
 * bytes into the MakerNote value, so every exec drives a vendor parser in
 * exif-mnote-data-*.c.  byte[0] selects the vendor; the rest is the note body
 * (the harness prepends the vendor signature).  Apple is intentionally absent:
 * its branch is commented out ("disabled until apple makernote can also be
 * saved") in libexif's interpret_maker_note(), i.e. dead code.
 *
 * Build on the fuzzing host (libFuzzer + ASan + UBSan):
 *   clang -g -O1 -fsanitize=fuzzer,address,undefined -fno-omit-frame-pointer \
 *     -DHAVE_CONFIG_H -I. -Ilibexif  libexif-mnote-harness.c \
 *     $(find libexif -name '*.c') -lm -o fuzz_mnote
 * Run:  ./fuzz_mnote -fork=$(nproc) -dict=mnote.dict -max_len=8192 corpus_mnote
 */
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "libexif/exif-data.h"
#include "libexif/exif-content.h"
#include "libexif/exif-entry.h"
#include "libexif/exif-mnote-data.h"
#include "libexif/exif-tag.h"
#include "libexif/exif-format.h"

typedef struct { const char *make; const unsigned char *sig; size_t siglen; } vendor_t;
#define SIG(s) (const unsigned char*)(s), (sizeof(s)-1)
/* verified against libexif *_identify(); Nikon/Sanyo/Epson via olympus module,
 * Casio via pentax module. Canon is selected by Make=="Canon" (no signature). */
static const vendor_t VENDORS[] = {
  {"Canon", SIG("")},            /* 0 canon    */
  {"",      SIG("OLYMP\0")},     /* 1 olympusV1 */
  {"",      SIG("OLYMPUS\0")},   /* 2 olympusV2 */
  {"",      SIG("Nikon\0\x02")}, /* 3 nikon    */
  {"",      SIG("FUJIFILM")},    /* 4 fuji     */
  {"",      SIG("AOC\0II")},     /* 5 pentaxV3 */
  {"",      SIG("QVC\0")},       /* 6 casioV2  */
  {"",      SIG("SANYO\0")},     /* 7 sanyo    */
  {"",      SIG("EPSON\0")},     /* 8 epson    */
};
static const size_t NV = sizeof(VENDORS)/sizeof(VENDORS[0]);
static void put16(unsigned char*p,uint16_t v){p[0]=(unsigned char)(v>>8);p[1]=(unsigned char)v;}
static void put32(unsigned char*p,uint32_t v){p[0]=(unsigned char)(v>>24);p[1]=(unsigned char)(v>>16);p[2]=(unsigned char)(v>>8);p[3]=(unsigned char)v;}
static void entry_cb(ExifEntry*e,void*u){char b[2001];(void)u;b[1999]=0;b[2000]=0;
  exif_tag_get_name(e->tag); exif_format_get_name(e->format);
  exif_entry_get_value(e,b,sizeof(b)-1); if(b[1999]!=0) abort();}
static void content_cb(ExifContent*c,void*u){exif_content_foreach_entry(c,entry_cb,u);}
static void exercise(ExifData*d){
  unsigned int i,n; char v[1024]; unsigned char*out=NULL; unsigned int os=0; ExifMnoteData*md;
  exif_data_foreach_content(d,content_cb,NULL);
  md=exif_data_get_mnote_data(d);
  if(md){ n=exif_mnote_data_count(md);
    for(i=0;i<n;i++){ if(!exif_mnote_data_get_name(md,i))continue;
      exif_mnote_data_get_title(md,i); exif_mnote_data_get_description(md,i);
      exif_mnote_data_get_value(md,i,v,sizeof(v)); } }
  exif_data_save_data(d,&out,&os); free(out); out=NULL;
  exif_data_set_byte_order(d, EXIF_BYTE_ORDER_INTEL);
  exif_data_save_data(d,&out,&os); free(out);
}
int LLVMFuzzerTestOneInput(const uint8_t*data,size_t size){
  if(size<2) return 0;
  const vendor_t*vd=&VENDORS[data[0]%NV];
  const uint8_t*mn=data+1; size_t mnlen=size-1;
  size_t ml=strlen(vd->make)+1;
  size_t nc=vd->siglen+mnlen; if(nc<12)nc=12;
  size_t ifd0=8, exif_off=38, make_off=56, note_off=make_off+ml, tot=6+note_off+nc;
  unsigned char*buf=(unsigned char*)calloc(1,tot); if(!buf) return 0;
  memcpy(buf,"Exif\0\0",6); unsigned char*T=buf+6;
  T[0]='M';T[1]='M';put16(T+2,0x002A);put32(T+4,(uint32_t)ifd0);
  unsigned char*ifd=T+ifd0; put16(ifd,2);
  unsigned char*e0=ifd+2,*e1=ifd+2+12;
  put16(e0,0x010F);put16(e0+2,2);put32(e0+4,(uint32_t)ml);  put32(e0+8,(uint32_t)make_off);
  put16(e1,0x8769);put16(e1+2,4);put32(e1+4,1);             put32(e1+8,(uint32_t)exif_off);
  put32(ifd+2+2*12,0);
  unsigned char*xifd=T+exif_off; put16(xifd,1);
  unsigned char*m0=xifd+2;
  put16(m0,0x927C);put16(m0+2,7);put32(m0+4,(uint32_t)nc);  put32(m0+8,(uint32_t)note_off);
  put32(xifd+2+1*12,0);
  memcpy(T+make_off,vd->make,ml);
  if(vd->siglen)memcpy(T+note_off,vd->sig,vd->siglen);
  memcpy(T+note_off+vd->siglen,mn,mnlen);
  ExifData*d=exif_data_new_from_data(buf,(unsigned int)tot); free(buf);
  if(d){ exercise(d); exif_data_unref(d); }
  return 0;
}
