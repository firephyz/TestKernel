target remote :1234
set architecture i386
symbol-file debug/boot.sym
break kinit.c:168
layout next
layout next
layout next
focus next
continue