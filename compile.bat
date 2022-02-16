@rem ±àÒë
windres -o 3dgomoku.o 3dgomoku.rc
g++ basic3d.cpp put_chess.cpp ui.cpp var.cpp 3dgomoku.o -o 3dÎå×ÓÆå.exe -lgdi32 -lwinmm -lcomctl32 -lcomdlg32 -ld3dx9_43 -ld3d9 -Wl,-s -Wl,--stack=16777216 -Os  -static-libgcc -static-libstdc++ -fexec-charset=GBK -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic -mwindows
pause 
