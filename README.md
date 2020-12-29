# ------ hello-world
A test repository for GitHub use.

Also adding this line as part of change while adding a new branch.

# ------- xdr-test.c
xdr_test.c : A small program to test XDR encode and decode
To compile xdr_test : gcc -I/usr/include/tirpc xdr_test.c -ltirpc

# ------- PID chain while a program is executed on Linux

## bash shell is the parent of a.out 
$ ps -aef | grep 3943
pramod      3943    2448  0 Dec25 pts/1    00:00:00 bash
pramod     39251    3943 99 12:51 pts/1    00:07:38 ./a.out
pramod     39269    2517  0 12:59 pts/0    00:00:00 grep --color=auto 3943

## gnome-terminal-server is the parent of 2 terminals with bash shells
$ ps -aef | grep 2448
pramod      2448    1854  0 Dec25 ?        00:00:55 /usr/libexec/gnome-terminal-server
pramod      2517    2448  0 Dec25 pts/0    00:00:00 bash
pramod      3943    2448  0 Dec25 pts/1    00:00:00 bash

## systemd is the parent of gnome-terminal-server
$ ps -aef | grep 1854
pramod      1854       1  0 Dec25 ?        00:00:02 /lib/systemd/systemd --user
pramod      1855    1854  0 Dec25 ?        00:00:00 (sd-pam)

## init process is the parent of systemd
UID          PID    PPID  C STIME TTY          TIME CMD
root           1       0  0 Dec25 ?        00:00:15 /sbin/init splash
root           2       0  0 Dec25 ?        00:00:00 [kthreadd]

## PID 0 is swapper / sched and is run as part of kernel (not user mode process), hence not shown in ps output

