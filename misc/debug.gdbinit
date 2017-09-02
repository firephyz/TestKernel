target remote :1234
set architecture i386
symbol-file debug/boot.sym
hbreak _load_os
display/x $eax
display/x $ebx
display/x $ecx
display/x $edx
display/x $esp
display/x $eip
continue