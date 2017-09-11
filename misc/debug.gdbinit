target remote :1234
set architecture i386
symbol-file debug/boot.sym
hbreak kernel_start
hbreak boot.s:50
hbreak boot.s:60
layout next
layout next
layout next
continue