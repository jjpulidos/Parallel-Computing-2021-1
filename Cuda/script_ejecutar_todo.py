from datetime import datetime
from numpy import array
from os import system as do
import numpy as np
from math import floor

logfile = "log.txt"

sizes = ["720p", "1080p", "4k"]
blocks = [1, 2, 5, 10, 20, 30]
threads_x = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512]
threads_y = []

do("mkdir in 2>/dev/null")
# for t_num in blocks:
#     do(f"mkdir out_{t_num} 2>/dev/null")

do("make")

date = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
do(f"echo 'run at {date}' >> {logfile}")

print()
for size in sizes:

    print(f"\nsmoothing image of size {size}")

    log_fname = f"result_{size}.txt"
    in_fname = f"in/sana_noise{size}.jpg"

    do(f"echo __________ >> {logfile}")
    do(f"echo img size: {size} >> {logfile}")
    for t_num in blocks:

        print(f"  with block= {t_num}")

        #         if size == "720p":
        #             # threads_x = np.arange(10, 1280 / t_num,
        #                                   # floor(0.1 * (1280 / t_num)))
        #             # threads_y = np.arange(10, 720 / t_num, floor(0.1 * (720 / t_num)))
        #             threads_x = []
        #
        #         if size == "1080p":
        #             threads_x = np.arange(10, 1920 / t_num,
        #                                   floor(0.1 * (1920 / t_num)))
        #             threads_y = np.arange(10, 1080 / t_num,
        #                                   floor(0.1 * (1080 / t_num)))
        #         if size == "4k":
        #             threads_x = np.arange(10, 3840 / t_num,
        #                                   floor(0.1 * (3840 / t_num)))
        #             threads_y = np.arange(10, 2160 / t_num,
        #                                   floor(0.1 * (2160 / t_num)))

        do(f"echo .......... >> {logfile}")
        do(f"echo 'block = {t_num}' >> {logfile}")
        out_fname = f"out_{t_num}/sana_smoothed{size}.jpg"
        for i in range(len(threads_x)):
            do(f"echo 'thread x = {threads_x[i]}' >> {logfile}")
            # do(f"echo 'thread y = {threads_y[i]}' >> {logfile}")
            do(f"./cuda {t_num} {in_fname} {out_fname} {threads_x[i]} {threads_x[i]}>> {logfile}"
               )
            # do(f"echo './cuda {t_num} {in_fname} {out_fname} {threads_x[i]} {threads_x[i]}'>> {logfile}"
            #    )
        #Promedio
        # nums = 10
        # while nums > 0:
        #     do(f"./cuda {t_num} {in_fname} {out_fname} 0 0 >> logtpm.txt")
        #     nums = nums - 1
        # file = open("logtpm.txt", "r+")
        # l = list(map(float, file.read().splitlines()))
        # file.seek(0)
        # file.truncate()
        # avg = sum(l) / len(l)
        # do(f"echo 'time = {avg}' >> {logfile}")

do(f"echo ========== >> {logfile}")
