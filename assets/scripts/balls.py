

def clamp(a):
    return max(min(round(a, 3), 0.99), 0.01)

cnt = 0

def balls(x, y, z, r, d, depth):

    print("mtlcolor {} {} {} 1 1 1 0.2 0.6 0.2 50".format(clamp(1.0 / depth), clamp((1.0 / depth) ** 2), clamp((1.0 / depth) ** (1 / 2))))
    print("sphere {} {} {} {}".format(x / 1.0, y / 1.0, z / 1.0, r / 1.0))

    global cnt
    cnt += 1

    if (depth <= 6):
        m = r + (r * d)
        balls(x + m, y, z, r * d, d, depth + 1)
        balls(x, y + m, z, r * d, d, depth + 1)
        balls(x, y, z + m, r * d, d, depth + 1)

        balls(x - m, y, z, r * d, d, depth + 1)
        balls(x, y - m, z, r * d, d, depth + 1)
        balls(x, y, z - m, r * d, d, depth + 1)


print("""
imsize 1920 1080

eye      -8.0 -8.0 2.0
viewdir  1.0 0.85 1.0
updir    0.0 1.0 0.0
vfov     45.0

bkgcolor 0.0 0.0 0.0

attlight -79 -8 -69  1  .5 .5 .5  1.0 0.0001 0.000001
attlight  63 -8  73  1  .5 .5 .5  1.0 0.0001 0.000001
""")

balls(0, 0, 10, 2, .5, 1)

# print(cnt)