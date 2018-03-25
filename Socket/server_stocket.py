#!/usr/bin/python
# -- coding: utf-8 --
import socket;
if "__main__" == __name__:
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM); #定义socket类型，网络通信，TCP
        print("create socket succ!");
        sock.bind(('192.168.177.111', 6666)); #套接字绑定的IP与端口
        print("bind socket succ!");
        sock.listen(5);
        print("listen succ!");
    except:
        print("init socket err!");
    while True:
        print("listen for client...");
        conn, addr = sock.accept(); #接受TCP连接，并返回新的套接字与IP地址
        print("get client");
        print(addr);
        conn.settimeout(5);
        szBuf = conn.recv(1024); #把接收的数据实例化
        print("recv:" + str(szBuf));
        if "0" == szBuf:  
            conn.sendall('exit');  
        else:  
            conn.sendall('welcome client!');  
        conn.close();
        print("end of sevice");