import random
import math

grads  = []

for i in range(256):
    theta = math.acos(random.uniform(-1, 1))
    phi = random.uniform(0, 2 * math.pi)

    x = math.cos(phi) * math.sin(theta)
    y = math.sin(phi) * math.sin(theta)
    z = math.cos(theta)

    grads.append([x, y, z])
    
print(",\n".join([ str(grad).replace("[", "{").replace("]", "}")  for grad in grads]))