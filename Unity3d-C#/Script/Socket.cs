﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System;
using System.IO;
using System.Runtime.InteropServices;
/// <summary>  
/// scoket服务器监听端口脚本  
/// </summary>  

public class Socket : MonoBehaviour
{

    private Thread thStartServer;//定义启动socket的线程 

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
    public struct face_fit_msg
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
        public byte[] packages_head;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 66*2)]
        //test point 66
        public double[] face_fit_data;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
        public byte[] package_end;
    }
    public static class face_fit
    {
       public static face_fit_msg face_data_recv;
    }
    //face_fit_msg face_data_recv;
    private void Awake()
    {
        byte[] buffer = new byte[Marshal.SizeOf(new face_fit_msg())];
        face_fit.face_data_recv = Bytes2Struct<face_fit_msg>(buffer);
    }
    void Start()
    {
        thStartServer = new Thread(StartServer);
        thStartServer.Start();//启动该线程
    }

    void Update()
    {
    }

    protected T Bytes2Struct<T>(byte[] bytes)
    {
        IntPtr arrPtr = Marshal.UnsafeAddrOfPinnedArrayElement(bytes, 0);
        return (T)Marshal.PtrToStructure(arrPtr, typeof(T));
    }

    private void StartServer()
    {
        const int bufferSize = 8192;//缓存大小,8192字节  
        IPAddress ip = IPAddress.Parse("192.168.177.111");

        TcpListener tlistener = new TcpListener(ip, 6666);
        tlistener.Start();
        Debug.Log("Socket服务器监听启动......");


        do
        {
            try  //直接关掉客户端，服务器端会抛出异常  
            {
                TcpClient remoteClient = tlistener.AcceptTcpClient();//接收已连接的客户端,阻塞方法  
                Debug.Log("客户端已连接！local:" + remoteClient.Client.LocalEndPoint + "<---Client:" + remoteClient.Client.RemoteEndPoint);
                NetworkStream streamToClient = remoteClient.GetStream();//获得来自客户端的流  
                                                                        //接收客户端发送的数据部分  
                byte[] buffer = new byte[bufferSize];//定义一个缓存buffer数组  
                int byteRead = streamToClient.Read(buffer, 0, bufferSize);//将数据搞入缓存中（有朋友说read()是阻塞方法，测试中未发现程序阻塞）  
                if (byteRead == 0)//连接断开，或者在TCPClient上调用了Close()方法，或者在流上调用了Dispose()方法。  
                {
                    Debug.Log("客户端连接断开......");
                    break;
                }
                int package_size = Marshal.SizeOf(new face_fit_msg());
                int package_num = byteRead / package_size;
                //Debug.Log("face_fit_msg size :" + package_num);
                for(int i = 0; i < package_num; ++i) {
                    byte[] each_struct = new byte[package_size];
                    Array.Copy(buffer, i* package_size, each_struct, 0, package_size);
                    //Debug.Log("package length: " + each_struct.Length + '\n' + BitConverter.ToString(each_struct));
                    //Debug.Log("msg length :" + byteRead);
                    face_fit.face_data_recv = Bytes2Struct<face_fit_msg>(each_struct);
                    //check each head and end
                    //if check successfully, then do next operation.
                    //Debug.Log("package head : " + BitConverter.ToString(face_fit.face_data_recv.packages_head));
                   //Debug.Log("face-fit data: " + face_fit.face_data_recv.face_fit_data[0] + ' '
                   //     + face_fit.face_data_recv.face_fit_data[1] + ' '
                   //     + face_fit.face_data_recv.face_fit_data[2] + ' '
                   //     + face_fit.face_data_recv.face_fit_data[3] + ' ');
                    //Debug.Log("package end : " + BitConverter.ToString(face_fit.face_data_recv.package_end));
                }
                
                // string msg = Encoding.Unicode.GetString(buffer, 0, byteRead);//从二进制转换为字符串对应的客户端会有从字符串转换为二进制的方法 
                //string msg = Encoding.ASCII.GetString(buffer, 0, byteRead);// 直接接受字符串
                //Debug.Log("接收数据：" + msg + ".数据长度:[" + byteRead + "byte]");
            }
            catch (Exception ex)
            {
                Debug.Log("客户端异常：" + ex.Message);
                break;
            }
        }
        while (true);

    }

    void OnApplicationQuit()
    {
        thStartServer.Abort();//在程序结束时杀掉线程，想起以前老谢给我讲的，起线程就像拉屎，完事一定要记得自己擦，系统不会给你擦，经测试不擦第二次启动unity会无响应  
    }

}