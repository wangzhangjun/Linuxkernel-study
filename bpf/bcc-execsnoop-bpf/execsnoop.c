/* Tracing execve system call. */
#include <uapi/linux/ptrace.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/bpf.h>

// consts for arguments (ensure below stack size limit 512)
#define ARGSIZE 64
#define TOTAL_MAX_ARGS 5
#define FULL_MAX_ARGS_ARR (TOTAL_MAX_ARGS * ARGSIZE)
#define LAST_ARG (FULL_MAX_ARGS_ARR - ARGSIZE)  // 最后一个参数的开始位置
/*
为什么要定义这些宏，这是因为char argv[] 是要保存从用户态传递给execve的参数。
这时候，你肯定是要在一个函数中区读取这些参数，然后保存到数组中。但是因为ebpf栈大小只有512个字节。
所以不能超过512个字节，必须要保证每次读取的内容是不能超过栈的大小的。
所以就定义了最大的参数个数和参数长度。
*/

// perf event map (sharing data to userspace) and hash map (sharing data between tracepoints)
struct data_t {
    u32 pid;
    char comm[TASK_COMM_LEN];
    int retval;
    unsigned int args_size;
    char argv[FULL_MAX_ARGS_ARR];
};
BPF_PERF_OUTPUT(events);  //定义一个性能事件，用于向用户态发送数据
BPF_HASH(tasks, u32, struct data_t); //定义一个hash映射，u32 -> data_t

// helper function to read string from userspace.
static int __bpf_read_arg_str(struct data_t *data, const char *ptr)
{
    if (data->args_size > LAST_ARG) {
        return -1;
    }

    //bpf_probe_read_user_str()  返回的是包含字符串结束符\0的长度
    int ret = bpf_probe_read_user_str(&data->argv[data->args_size], ARGSIZE, (void *)ptr);
    if (ret > ARGSIZE || ret < 0) {
        return -1;
    }

    // increase the args size. the first tailing '\0' is not counted and hence it would be overwritten by the next call.
    data->args_size += (ret - 1);

    return 0;
}

// sys_enter_execve tracepoint.
TRACEPOINT_PROBE(syscalls, sys_enter_execve)  // 这里改为execveat为啥就不行了
{
    unsigned int ret = 0;
    const char **argv = (const char **)(args->argv); //BCC把所有参数都放到了args中，可以使用args->argv来访问参数列表

    struct data_t data = {  };
    u32 pid = bpf_get_current_pid_tgid();
    data.pid = pid;
    bpf_get_current_comm(&data.comm, sizeof(data.comm));
    
    // bpf_trace_printk("hello", 5);
    
    if (__bpf_read_arg_str(&data, (const char *)argv[0]) < 0) { //  get the binary name (first argment)
        goto out;
    }
    // get other arguments (skip first arg because it has already been read)
#pragma unroll //#pragma unroll  告诉编译器，把源码中的循环自动展开。这就避免了最终的字节码中包含循环。
    for (int i = 1; i < TOTAL_MAX_ARGS; i++) {
        if (__bpf_read_arg_str(&data, (const char *)argv[i]) < 0) {
            goto out;
        }
    }
out:
    // store the data in hash map
    tasks.update(&pid, &data);
    return 0;
}

// sys_exit_execve tracepoint
TRACEPOINT_PROBE(syscalls, sys_exit_execve)
{
    // query the data from hash map
    u32 pid = bpf_get_current_pid_tgid();
    struct data_t *data = tasks.lookup(&pid);

    // submit perf events after getting the retval
    if (data != NULL) {
        data->retval = args->ret;
        events.perf_submit(args, data, sizeof(struct data_t));

        // clean up the hash map
        tasks.delete(&pid);
    }

    return 0;
}

