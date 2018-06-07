# ubuntu安装vmlinux查看内核调试信息

### 1.在终端中输入以下，添加库
```
echo "deb http://ddebs.ubuntu.com $(lsb_release -cs)-updates main restricted universe multiverse
deb http://ddebs.ubuntu.com $(lsb_release -cs)-security main restricted universe multiverse
deb http://ddebs.ubuntu.com $(lsb_release -cs)-proposed main restricted universe multiverse" | \
sudo tee -a /etc/apt/sources.list.d/ddebs.list

sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 428D7C01
```

### 2.update
```
sudo apt-get update
sudo apt-get install linux-image-$(uname -r)-dbgsym
```

### 3.vmliux
```
/usr/lib/debug/boot/vmlinux-$(uname -r)
```

### 4.gdb调试
```
gdb -q vmliux
disass sys_read
```
