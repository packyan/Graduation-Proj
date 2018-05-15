# Data-Drive Facial Animation

标签（空格分隔）： 毕业设计

---
## Face-fit
It's based on [CSIRO-FaceAnalysis-SDK](http://face.ci2cv.net/) to tracking face and find the face feature points through a webcam, as the same time pack the face feature points data and transmit them to a Unity3D server.Use RBF(Radial Basis Function) Interpolation to drive those additional points more smoothly. Transmit RBF witghts and other parameters to server, decrease the data packat size.

## Unity3D C\#
### Usage:
1. `socket.cs` is used to communicata with the client, and receive Face-fit data form a linux client.
2. `example.cs` drag it to the skin model, and it drives the model move.
3. `ohter_models.cs` other models on face, follow the movement of the skin model.
3. `Initializeface.cs` is used to initialize face nodes in Unity3D and drive these nodes to make face animation. Only use in test.

## Other Floders
Only use to prictice and test some features.

### Logs:
1. 2018.04.09 First test finished. Just simpely receive data from client, and drive some spheres.
2. 2018.04.21 `face-fit` use linear interp to add additional face point, it's purely to increase the number of points. So the facial animation looks very hard. Optimize the server to receive separated data packets.
3. 2018.04.23 `face-fit` now use  RBF(Radial Basis Function) Interpolation to drive those additional points more smoothly. But it's still a 2D facial animation, considering add static Z-axis coordinate to build a 3D face model.
4. 2018.04.26 edit `face-fit` and C# script, it can drawing a very simple facial animation.


## 1. 目录

## 2. 项目简介
Data-Drive Facial Animation

## 3.技术路线
本课题基于Opencv开源平台，研究和实现一种简便、高效地通过普通RGB相机采集人脸表情数据，利用跟踪到的人脸特征点位移数据驱动参数化人脸模型，从而获得真实、自然的人脸表情动画效果。

在LINUX下采集人脸表情数据，并将采集到的数据通过Socket通信方式发送到异地的使用者，在异地对人脸表情数据进行插值，获得更自然真实的人脸并且此方法对带宽的需求较小，可以用于通信条件较差的环境。异地获得人脸数据后，通过UNITY 3D驱动人脸模型生成动画。

## 4.Usage:

face-fit 通过视频流的方式获取面部图片, 从图片中提取用户脸部特征点信息, 再通过socket将特征点数据发送到unity3d服务端进行人脸动画驱动.










