# Graduation Proj

标签（空格分隔）： 毕业设计

---
## 1. 目录

## 2. 项目简介
面部表情是人与人交流的一个重要渠道。人类可以通过不同的面部表情来传达各种情绪与信息，使人与人的交流更加便捷。为了表达出人脸面部表情的丰富情感，需要构建真实的人脸三维模型，并驱动产生人脸动画。这项工作不仅在生物学，人工智能，心理学有广泛的研究，而且在计算机图像与图形学中也是一个备受关注的研究热点。

人脸动画技术是指利用人脸面部的细微运动来表现虚拟化身(Avatar)的表情以及其情感变化，在影视、游戏、可视通信方面都有广泛的应用。在影视与游戏方面，人脸动画能展现出作品中的角色细腻的表情，让观众与玩家体会到虚拟世界中角色的情绪，增强了代入感，使作品的表现力极大增强。以《阿凡达》等为代表的科幻电影、以《黑色洛城》为代表的各种大型游戏，都是人脸动画技术应用于影视游戏领域的经典案例。在可视通信方面，目前比较具有代表性的是苹果公司手机上搭载Animoji，其使用面部识别传感器来检测用户面部表情变化，来生成表情丰富的表情动画，为网络通讯带来了全新体验。同时人脸动画也在视频通讯方面也由很重要的运用，在低带宽的情况下，通过人脸动画技术仅需在网络中传输少量的人脸参数及动画数据，即可在异地生成较为真实的人脸动画，对实现远程视频会议有着重要的理论与实际意义。因为人脸动画技术应用前景广，所以由许多优秀的学者投入到其研究中去，这促进了人脸动画技术的发展。

人脸动画是以人脸模型的几何和纹理为基础，采用动画相关技术使得人脸模型产生整体或局部的形变，最终达到模拟人脸运动的目的。

基于图像处理的方法一般不需要专业的三维扫描设备，仅需要普通的RGB相机，通过拍摄若干张人脸照片作为输入，从图像中检测出人脸的几何信息，人脸检测指的是根据输入图像，确定图像中的人脸大小和位置，并采集人脸纹理。目前主要有基于统计的方法和基于知识的方法两种。此方法成本低，能够在线生成人脸模型，实时性高，适合应用在对人脸模型的精度要求不高而对实时性要求高的场合。

数据驱动模型是一种基于样本图像的方法。其基本思想是从大量源视频图像里通过面部识别和分析技术抽取、捕获真人说话和带各种面部表情的真实面部图像，以此建立一个样本图像数据库，然后通过算法把这些样本图像连接起来，从而生成新的人脸图像序列。基于数据驱动方法的优点是合成的人脸图像质量高，较为逼真、自然；缺点是建立模型的训练阶段需要大量的原始图片。而且数据是基于某个特定用户的，较难移植到其他人身上

## 3.技术路线
本课题基于Opencv开源平台，研究和实现一种简便、高效地通过普通RGB相机采集人脸表情数据，利用跟踪到的人脸特征点位移数据驱动参数化人脸模型，从而获得真实、自然的人脸表情动画效果。

在LINUX下采集人脸表情数据，并将采集到的数据通过Socket通信方式发送到异地的使用者，在异地对人脸表情数据进行插值，获得更自然真实的人脸并且此方法对带宽的需求较小，可以用于通信条件较差的环境。异地获得人脸数据后，通过UNITY 3D驱动人脸模型生成动画。











