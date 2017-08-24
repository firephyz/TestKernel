target remote :1234
set architecture i8086
symbol-file debug/boot.sym
hbreak _start
continue