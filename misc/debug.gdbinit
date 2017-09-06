target remote :1234
set architecture i386
symbol-file debug/boot.sym
hbreak kernel_start
layout next
layout next
layout next
focus cmd
continue