import os
import pygame as pg

def draw_shutter(screen):
    fgcl = (63, 63, 63)
    pg.draw.circle(screen, fgcl, (50, 50), 25)
    pg.draw.circle(screen, fgcl, (15, 68), 10)
    pg.draw.circle(screen, fgcl, (10, 46), 10)
    pg.draw.circle(screen, fgcl, (21, 26), 10)
    pg.draw.circle(screen, fgcl, (40, 13), 10)

def draw_dpad(screen):
    fgcl = (63, 63, 63)
    pg.draw.circle(screen, fgcl, (25, 12), 10)
    pg.draw.circle(screen, fgcl, (55, 12), 10)
    pg.draw.circle(screen, fgcl, (40, 40), 20)
    pg.draw.circle(screen, fgcl, (25, 68), 10)
    pg.draw.circle(screen, fgcl, (55, 68), 10)

def draw_play_menu(screen):
    fgcl = (255, 255, 255)
    pg.draw.polygon(screen, fgcl, [[20, 60], [30, 68], [20, 76]])
    pg.draw.line(screen, fgcl, [48, 62], [60, 62], 3)
    pg.draw.line(screen, fgcl, [48, 68], [60, 68], 3)
    pg.draw.line(screen, fgcl, [48, 74], [60, 74], 3)

def draw_play_del(screen):
    fgcl = (255, 255, 255)
    pg.draw.polygon(screen, fgcl, [[20, 60], [30, 68], [20, 76]])
    pg.draw.line(screen, fgcl, [52, 60], [56, 60], 2)
    pg.draw.line(screen, fgcl, [48, 62], [60, 62], 2)
    pg.draw.line(screen, fgcl, [48, 62], [48, 64], 2)
    pg.draw.line(screen, fgcl, [60, 62], [60, 64], 2)
    pg.draw.rect(screen, fgcl, [51, 65, 8, 10])

def gen_buf_y8(surface):
    (w, h) = surface.get_size()
    buf = bytearray(w * h)
    for y in range(h):
        for x in range(w):
            color = surface.get_at((x, y))
            y8 = (color.r + color.g + color.b) // 3
            buf[y * w + x] = y8
    return buf

def save_c(buf, fn, arrname):
    f = open(fn, "w")
    f.write(f"const unsigned char {arrname}[] = {{\n")
    newline = True
    for i in range(len(buf)):
        if newline:
            f.write("    ")
            newline = False
        f.write(f"0x{buf[i]:02x}, ")
        if (i % 8 == 7):
            f.write("\n")
            newline = True
    if not newline:
        f.write("\n")
    f.write("};\n")
    f.close()

def main():
    pg.init()
    screen = pg.display.set_mode((80, 80), 0, 32)
    screen.fill((0, 0, 0))
    draw_shutter(screen)
    pg.display.flip()
    pg.image.save(screen, "shutter.png")
    save_c(gen_buf_y8(screen), "shutter.c", "image_shutter")

    screen.fill((0, 0, 0))
    draw_dpad(screen)
    draw_play_menu(screen)
    pg.display.flip()
    pg.image.save(screen, "dpad1.png")
    save_c(gen_buf_y8(screen), "dpad1.c", "image_dpad1")

    screen.fill((0, 0, 0))
    draw_dpad(screen)
    draw_play_del(screen)
    pg.display.flip()
    pg.image.save(screen, "dpad2.png")
    save_c(gen_buf_y8(screen), "dpad2.c", "image_dpad2")

    going = True
    while going:
        for event in pg.event.get():
            if event.type == pg.QUIT:
                going = False

    pg.quit()

if __name__ == "__main__":
    main()