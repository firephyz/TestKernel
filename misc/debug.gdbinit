target remote :1234
set architecture i386
symbol-file debug/boot.sym
hbreak boot.s:37
continue
layout next
layout next
focus next
display/x $ax
display/x $bx
display/x $cx
display/x $dx
display/x $si
display/x $di
display/x $bp