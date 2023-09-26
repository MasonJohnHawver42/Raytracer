

def balls(x, y, z, r, d):

    print("sphere {} {} {} {}".format(x / 1.0, y / 1.0, z / 1.0, r / 1.0))

    if (r > .125):
        m = r + (r * d)
        balls(x + m, y, z, r * d, d)
        balls(x, y + m, z, r * d, d)
        balls(x, y, z + m, r * d, d)

        balls(x - m, y, z, r * d, d)
        balls(x, y - m, z, r * d, d)
        balls(x, y, z - m, r * d, d)


balls(0, 0, 10, 2, .5)