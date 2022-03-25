
from bcc import BPF

# 1) load BPF program
b = BPF(src_file="hello.c")
b.attach_kprobe(event="do_sys_openat2", fn_name="hello_world") # fn_name 对应hello.c中的入口hook函数

# 2) print header
print("%-18s %-16s %-6s %-16s" % ("TIME(s)", "COMM", "PID", "FILE"))

# 3) define the callback for perf event
start = 0
def print_event(cpu, data, size):
    global start
    event = b["eventstest"].event(data)  # eventstest对应hello.c中的定义的perf事件
    if start == 0:
            start = event.ts
    time_s = (float(event.ts - start)) / 1000000000
    print("%-18.9f %-16s %-6d %-16s" % (time_s, event.comm, event.pid, event.fname))

# 4) loop with callback to print_event
b["eventstest"].open_perf_buffer(print_event)
while 1:
    try:
        b.perf_buffer_poll()  # 读取映射的内容
    except KeyboardInterrupt:
        exit()