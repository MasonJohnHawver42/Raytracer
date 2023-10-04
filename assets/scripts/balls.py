

def clamp(a):
    return max(min(round(a, 3), 0.99), 0.01)

cnt = 0

def balls(x, y, z, r, d, depth):

    print("mtlcolor {} {} {}".format(clamp(1.0 / depth), clamp((1.0 / depth) ** 2), clamp((1.0 / depth) ** (1 / 2))))
    print("sphere {} {} {} {}".format(x / 1.0, y / 1.0, z / 1.0, r / 1.0))

    global cnt
    cnt += 1

    if (depth <= 5):
        m = r + (r * d)
        balls(x + m, y, z, r * d, d, depth + 1)
        balls(x, y + m, z, r * d, d, depth + 1)
        balls(x, y, z + m, r * d, d, depth + 1)

        balls(x - m, y, z, r * d, d, depth + 1)
        balls(x, y - m, z, r * d, d, depth + 1)
        balls(x, y, z - m, r * d, d, depth + 1)


balls(0, 0, 10, 2, .5, 1)

print(cnt)