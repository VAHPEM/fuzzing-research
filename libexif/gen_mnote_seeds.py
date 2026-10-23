#!/usr/bin/env python3
"""Extract real MakerNote bodies from vendor JPEG/Exif files and emit
[vendor_byte][note-after-signature] seeds for libexif-mnote-harness.c.

Follows IFD0 -> ExifIFDPointer(0x8769) -> EXIF sub-IFD -> MakerNote(0x927C),
honouring TIFF byte order. Reconstruction is exact: for signature vendors the
leading signature is stripped and re-added by the harness; Canon (no signature)
is detected via Make=="Canon" and emitted whole.
Usage: gen_mnote_seeds.py <src_dir> <out_dir>"""
import os, sys, struct, glob
# (vendor_index, signature) — MUST match VENDORS[] in the C harness (no Apple):
VSIGS = [(1,b"OLYMP\x00"),(2,b"OLYMPUS\x00"),(3,b"Nikon\x00\x02"),(4,b"FUJIFILM"),
         (5,b"AOC\x00II"),(6,b"QVC\x00"),(7,b"SANYO\x00"),(8,b"EPSON\x00")]
def u16(b,o,bo): return struct.unpack(bo+"H",b[o:o+2])[0]
def u32(b,o,bo): return struct.unpack(bo+"I",b[o:o+4])[0]
def find_tag(tiff,ifd_off,bo,tag):
    if ifd_off+2>len(tiff): return None
    n=u16(tiff,ifd_off,bo)
    for k in range(n):
        e=ifd_off+2+k*12
        if e+12>len(tiff): break
        t=u16(tiff,e,bo); typ=u16(tiff,e+2,bo); cnt=u32(tiff,e+4,bo)
        val=tiff[e+8:e+12]
        if t==tag: return (typ,cnt,val,e)
    return None
def extract(path):
    d=open(path,"rb").read()
    i=d.find(b"Exif\x00\x00")
    if i<0: return None
    tiff=d[i+6:]
    if len(tiff)<8: return None
    bo = "<" if tiff[:2]==b"II" else ">"
    ifd0=u32(tiff,4,bo)
    make=None
    mk=find_tag(tiff,ifd0,bo,0x010F)
    if mk:
        _,cnt,val,_=mk
        moff = u32(tiff,0,bo) and None  # placeholder
        off = struct.unpack(bo+"I",val)[0] if cnt>4 else None
        s = tiff[off:off+cnt] if off is not None else val[:cnt]
        make = s.split(b"\x00")[0].decode("latin1","ignore")
    ep=find_tag(tiff,ifd0,bo,0x8769)       # ExifIFDPointer
    if not ep: return None
    exif_ifd=struct.unpack(bo+"I",ep[2])[0]
    mn=find_tag(tiff,exif_ifd,bo,0x927C)   # MakerNote
    if not mn: return None
    typ,cnt,val,_=mn
    off=struct.unpack(bo+"I",val)[0] if cnt>4 else None
    note = tiff[off:off+cnt] if off is not None else val[:cnt]
    return note, make
def main(src,out):
    os.makedirs(out,exist_ok=True); n=0; byv={}
    for f in glob.glob(os.path.join(src,"**","*"),recursive=True):
        if not os.path.isfile(f): continue
        try: r=extract(f)
        except Exception: r=None
        if not r: continue
        note,make=r
        emitted=False
        for idx,sig in VSIGS:
            if note.startswith(sig):
                open(os.path.join(out,f"s{n:05d}"),"wb").write(bytes([idx])+note[len(sig):])
                byv[idx]=byv.get(idx,0)+1; n+=1; emitted=True; break
        if not emitted and make=="Canon":
            open(os.path.join(out,f"s{n:05d}"),"wb").write(bytes([0])+note)
            byv[0]=byv.get(0,0)+1; n+=1
    print(f"wrote {n} seeds to {out}; by vendor index: {dict(sorted(byv.items()))}")
if __name__=="__main__":
    if len(sys.argv)!=3: sys.exit("usage: gen_mnote_seeds.py <src_dir> <out_dir>")
    main(sys.argv[1],sys.argv[2])
