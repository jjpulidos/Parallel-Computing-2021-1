from os import system as do

sizes = ["720p", "1080p", "4k"]
threads = [1, 2, 4, 8, 16]

do("mkdir in 2>/dev/null")
for t_num in threads:
  do(f"mkdir out_{t_num} 2>/dev/null")

do("make")

print()
for size in sizes:

  print(f"\nsmoothing image of size {size}\n")
  
  log_fname = f"result_{size}.txt"
  in_fname = f"in/sana_noise{size}.jpg"

  for t_num in threads:
  
    print(f"\nwith {t_num} threads\n")
  
    out_fname = f"out_{t_num}/sana_smoothed{size}.jpg"
    if t_num > 1:
      do(f"./paralel 5 {t_num} {in_fname} {out_fname}")
    else:
      do(f"./sequential 5 {in_fname} {out_fname}")

  print("\ndone\n")
    

