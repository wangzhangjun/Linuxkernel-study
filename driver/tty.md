# tty
在Linux上有三种类型的tty驱动程序：控制台、串口和pty.控制台和pty驱动程序已经被编写好了，而且可能也不必为这两类tty驱动程序编写其他的驱动程序。这使得任何使用tty核心与用户和系统交互的新驱动程序都可被看成是串口的驱动程序。

### 1.tty_driver
任何与tty驱动程序相关的最主要的数据结构是tty_driver，与该tty相关的任何信息，都存放在这个数据结构中。

```
struct tty_driver {
 297        int     magic;          /* magic number for this structure */
 298        struct kref kref;       /* Reference management */
 299        struct cdev **cdevs;
 300        struct module   *owner;
 301        const char      *driver_name;
 302        const char      *name;
 303        int     name_base;      /* offset of printed name */
 304        int     major;          /* major device number */
 305        int     minor_start;    /* start of minor device number */
 306        unsigned int    num;    /* number of devices allocated */
 307        short   type;           /* type of tty driver */
 308        short   subtype;        /* subtype of tty driver */
 309        struct ktermios init_termios; /* Initial termios */
 310        unsigned long   flags;          /* tty driver flags */
 311        struct proc_dir_entry *proc_entry; /* /proc fs entry */
 312        struct tty_driver *other; /* only used for the PTY driver */
 313
 314        /*
 315         * Pointer to the tty data structures
 316         */
 317        struct tty_struct **ttys;
 318        struct tty_port **ports;
 319        struct ktermios **termios;
 320        void *driver_state;
 321
 322        /*
 323         * Driver methods
 324         */
 325
 326        const struct tty_operations *ops;
 327        struct list_head tty_drivers;
 328};
```
例如：type字段存放的是哪一种的tty，在Linux中用以下宏来表示各种tty,另外注意所有的tty_driver是通过一个链表组织起来的（最后一个结构体成员为list_head），所以当你需要使用某种类型的tty时，需要对这个链表进行遍历。

```
419/* tty driver types */
420#define TTY_DRIVER_TYPE_SYSTEM          0x0001
421#define TTY_DRIVER_TYPE_CONSOLE         0x0002
422#define TTY_DRIVER_TYPE_SERIAL          0x0003  //表示的是串口
423#define TTY_DRIVER_TYPE_PTY             0x0004
424#define TTY_DRIVER_TYPE_SCC             0x0005  /* scc driver */
425#define TTY_DRIVER_TYPE_SYSCONS         0x0006

```

### 2.用户态通过统一的read,open,write对tty进行读写操作，对tty的所有操作封装在tty_fops这个结构体中。
```
cat /proc/kallsyms | grep tty_fops
```
查看符号表，当然你也可以替换他。
