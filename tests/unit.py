import sys
import os

from subprocess import call

n = len(sys.argv)

folder_path = sys.argv[1]
rt_path = sys.argv[2]
cp_path = sys.argv[3]

out_path = "./out"
diff_path = "./diff"

for fn in os.listdir(folder_path):
    f = os.path.join(folder_path, fn)
    if os.path.isfile(f):
        rt, ext = os.path.splitext(f)
        if (ext == ".txt"):
            print(str(f))
            print()
            call([rt_path, str(f), out_path + "/" + fn.split(".")[0] + ".ppm"])

print("\n------------------------\n")

# for fn in os.listdir(folder_path):
#     f = os.path.join(folder_path, fn)
#     if os.path.isfile(f):
#         rt, ext = os.path.splitext(f)
#         if (ext == ".ppm"):
#             call([cp_path, str(f), out_path + "/" + fn.split(".")[0] + ".ppm", diff_path + "/" + fn.split(".")[0] + ".ppm"])