#!/bin/bash
for names in $(cat /proc/modules | awk ' {print $1;} ')
        do echo -ne "$names\t     \t"
        modinfo $names | grep  license
done
