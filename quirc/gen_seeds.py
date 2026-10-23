#!/usr/bin/env python3
import os, struct, sys
OUT = sys.argv[1] if len(sys.argv) > 1 else "seeds"
os.makedirs(OUT, exist_ok=True)
def write_seed(name, w, h, pixels):
    assert len(pixels) == w*h
    with open(os.path.join(OUT, name), "wb") as f:
        f.write(struct.pack(">HH", w, h)); f.write(bytes(pixels))
n = 0
try:
    import qrcode
    from qrcode.constants import ERROR_CORRECT_L, ERROR_CORRECT_M, ERROR_CORRECT_Q, ERROR_CORRECT_H
    from PIL import Image
    payloads = ["https://example.com","HELLO","1234567890","https://attacker.example/\x00\x01pad",
                "A"*300, "https://example.com/"+"x"*1200, "mailto:a@b.co", "SJISあいう"]
    eccs = [ERROR_CORRECT_L, ERROR_CORRECT_M, ERROR_CORRECT_Q, ERROR_CORRECT_H]
    for i, text in enumerate(payloads):
        qr = qrcode.QRCode(error_correction=eccs[i % 4], box_size=3, border=4)
        qr.add_data(text); qr.make(fit=True)
        img = qr.make_image(fill_color="black", back_color="white").convert("L")
        if img.width > 400: img = img.resize((400,400), Image.NEAREST)
        w,h = img.size; write_seed(f"qr_{i:02d}_{w}x{h}.bin", w, h, list(img.getdata())); n+=1
except Exception as e:
    print("warn:", e, file=sys.stderr)
def finder(size):
    pat=[1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,0,1,1,1,0,1,1,0,1,1,1,0,1,1,0,1,1,1,0,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1]
    px=[255]*(size*size)
    def stamp(ox,oy):
        for y in range(7):
            for x in range(7):
                if pat[y*7+x]: px[(oy+y)*size+(ox+x)]=0
    for (ox,oy) in [(2,2),(size-9,2),(2,size-9)]:
        if ox>=0 and oy>=0: stamp(ox,oy)
    return px
for sz in (29,49,97): write_seed(f"finders_{sz}.bin", sz, sz, finder(sz)); n+=1
write_seed("black_64.bin",64,64,[0]*4096); n+=1
write_seed("white_64.bin",64,64,[255]*4096); n+=1
write_seed("checker_64.bin",64,64,[0 if (x^y)&1 else 255 for y in range(64) for x in range(64)]); n+=1
write_seed("strip_1x256.bin",256,1,[0 if i&1 else 255 for i in range(256)]); n+=1
write_seed("tiny_1x1.bin",1,1,[0]); n+=1
print(f"wrote {n} seeds to {OUT}/")
