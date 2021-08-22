from datetime import datetime
from os import system as do

logfile = "log.txt"

sizes = ["720p", "1080p", "4k"]
threads = [1, 2, 4, 8, 16]

do("mkdir in 2>/dev/null")
# for t_num in threads:
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
    for t_num in threads:

        print(f"  with threads = {t_num}")

        do(f"echo .......... >> {logfile}")
        do(f"echo 'threads = {t_num}' >> {logfile}")
        out_fname = f"out_{t_num}/sana_smoothed{size}.jpg"

        # if t_num > 1:
        #   do(f"./paralel 5 {t_num} {in_fname} {out_fname} >> {logfile}")
        # else:
        #   do(f"./sequential 5 {in_fname} {out_fname} >> {logfile}")
        #Promedio
        nums = 10
        if t_num > 1:
            while nums > 0:
                do(f"./paralel 5 {t_num} {in_fname} {out_fname} >> logtmp.txt")
                nums = nums - 1
        else:
            while nums > 0:
                do(f"./sequential 5 {in_fname} {out_fname} >> logtmp.txt")
                nums = nums - 1

        file = open("logtmp.txt", "r+")
        l = list(map(float, file.read().splitlines()))
        file.seek(0)
        file.truncate()
        avg = sum(l) / len(l)
        do(f"echo 'time = {avg}' >> {logfile}")

do(f"echo ========== >> {logfile}")
