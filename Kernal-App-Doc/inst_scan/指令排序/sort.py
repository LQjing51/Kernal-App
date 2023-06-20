import functools

fn = 'tmp.txt'
sorted_fn = 'sorted.txt'

def split(a):
    num = [0,0,0]
    cnt = 0
    for i in range(0,len(a)):
        # print(i, a[i])
        if a[i] >= 'A' and a[i] <= 'Z':
            continue
        if a[i] >= '0' and a[i] <= '9':
            num[cnt] = num[cnt]*10 + int(a[i])
        if a[i] == '.':
            cnt = cnt + 1
        if a[i] == ' ':
            break
    return num

def cmp(a,b):
    a = str(a)
    b = str(b)
    a.lstrip()
    b.lstrip()
    numa = split(a)
    numb = split(b)

    cnt = 0        
    if a[0] > b[0]:
        return 1
    elif a[0] < b[0]:
        return -1
    else:
        while(cnt < 3):
            if (numa[cnt] > numb[cnt]): return 1
            elif (numa[cnt] < numb[cnt]): return -1
            cnt = cnt + 1
        return 0
    
with open(fn,encoding='utf-8') as first_file:
    rows = first_file.readlines()
    sorted_rows = sorted(rows, key = functools.cmp_to_key(cmp), reverse=False)
    with open(sorted_fn,'w',encoding='utf-8') as second_file:
        for row in sorted_rows:
            second_file.write(row.lstrip())

