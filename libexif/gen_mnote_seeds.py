#!/usr/bin/env python3
"""Seed generator for libexif-mnote-harness: emits [vendor_byte][note_payload].
Pulls the real MakerNote bytes out of sample JPEGs (so seeds already sit deep
inside a vendor parser), plus a few degenerate notes. Point it at a folder of
real vendor photos. Needs: pip install exifread (or piexif)."""
import os, sys, struct, glob
# vendor index MUST match VENDORS[] order in the C harness:
VIDX = {"canon":0,"olympus":1,"nikon":3,"fujifilm":4,"pentax":5,"casio":6,"apple":7,"sanyo":8,"epson":9}
def extract_makernote(jpg):
    d = open(jpg,"rb").read()
    i = d.find(b"Exif\x00\x00")
    if i < 0: return None
    tiff = d[i+6:]
    bo = "<" if tiff[:2]==b"II" else ">"
    (ifd0,) = struct.unpack(bo+"I", tiff[4:8])
    (n,) = struct.unpack(bo+"H", tiff[ifd0:ifd0+2])
    for k in range(n):
        e = ifd0+2+k*12
        tag,typ,cnt = struct.unpack(bo+"HHI", tiff[e:e+8])
        (off,) = struct.unpack(bo+"I", tiff[e+8:e+12])
        if tag==0x927C:                       # MakerNote
            return tiff[off:off+cnt]
    return None
def guess_vendor(jpg):
    b=os.path.basename(jpg).lower()
    for k in VIDX:
        if k in b: return VIDX[k]
    return 0
def main(src, out):
    os.makedirs(out, exist_ok=True); c=0
    for jpg in glob.glob(os.path.join(src,"**","*.jp*g"), recursive=True):
        mn = extract_makernote(jpg)
        if not mn: continue
        seed = bytes([guess_vendor(jpg)]) + mn
        open(os.path.join(out, f"seed_{c:04d}"),"wb").write(seed); c+=1
    for vi in range(10):                      # degenerate per-vendor stubs
        open(os.path.join(out,f"stub_{vi}"),"wb").write(bytes([vi])+b"\x00"*32)
    print(f"wrote {c} real + 10 stub seeds to {out}")
if __name__=="__main__":
    if len(sys.argv)!=3: sys.exit("usage: gen_mnote_seeds.py <photos_dir> <out_dir>")
    main(sys.argv[1], sys.argv[2])
