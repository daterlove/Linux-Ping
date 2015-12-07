#linux下ｐｉｎｇ简易实现
obj = main.o Send.o Recv.o PrintInfo.o 

make:$(obj)
	gcc -c $(obj) -pthread
	gcc -o Linux_ping $(obj) -pthread
clean:
	rm -rf  *.o
