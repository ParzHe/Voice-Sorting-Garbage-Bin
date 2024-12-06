# Voice-Sorting-Garbage-Bin 语音控制的垃圾分类垃圾桶
## Introduction
基于Arduino Uno（Maker Uno）, ESP8286 和 LU-ASR01 的可以使用语音，网页和按钮控制开启关闭的智能垃圾桶原型。<br/>
A smart trash can prototype based on Arduino Uno (Maker Uno), ESP8286 and LU-ASR01 that can be turned on and off using voice, web and button control.<br/>

## Modules
For the voice control module, we use a microcontroller LU-ASR01 from China. This requires a special IDE named 天问block(tian wen block) to compile the voice control function. The `CodeForVoiceControl.hd` script requires 天问Block to open.   

For other scripts of Maker Uno or ESP8286, Arduino IDE is the tool we use to develop.  

`project_WIFI_part` is code folder for ESP8286, which plays the role of the web server;  
`Project_Arduino_part` is the code folder for slave Maker Uno to control servos, buttons  
`Project_detect_full` is the code folder for the module to detect whether the garbage is full in the bin.  

## Demo Videos
Demo Video：【物联网项目课程作业｜智能语音识别垃圾分类系统｜基于Arduino LD3322语音模块 ESP8266 Wi-Fi模块｜满溢检测｜网页控制】 https://www.bilibili.com/video/BV1cG411Y7dN/?share_source=copy_web&vd_source=b26700de0028265802bd24b8686c6c8b
