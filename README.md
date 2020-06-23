# usb_power_meter

硬件基于STM32F103C8T6+INA199A2DCKR+ESP8266实现的USB功率计，STM32使用stm32duino编程，ESP8266使用【开发板管理器-esp8266】直接编程

原理：在公头和母座的电源线间增加一个5毫欧的采样电阻，然后测量电阻两端的压降和其中一侧的电压，即可得到功率值，然后对结果进行格式化，并定时通过串口发送给ESP8266，然后进行RSA加密，然后传输给服务器，服务器可以使用固件自带的，需要到[pyro.cafe](https://pyro.cafe)进行注册账号使用，也可以自行搭建

附带硬件Gerber文件，可以直接提交厂家生产，开源链接为[四口功率计](https://lceda.cn/Pyrokine/USBgong-shuai-ji)和[三口功率计](https://lceda.cn/Pyrokine/USB-gong-lv-ji-3-kou)，欢迎fork，其中四口的为双type-c，支持pd协议和qc4.0协议充电，实测45w稳定充电，三口的为type-c转type-a，仅支持qc3.0协议，实测18w稳定充电

如果已经有ESP模块，可以直接在网站注册账号，然后按格式汇报数据，这也是可行的，已提供的格式如下，会不定期更新，目前设置历史记录最大显示1w个记录点位，若10S的汇报间隔，最大能显示一天多一点的信息，可以自行调整汇报频率，如果太高的话，或者滥用服务的话，可能会被ban哦，请善待绒布球ヾ(✿ﾟ▽ﾟ)ノ

## 实物图  

四口PD充电  
<img src="https://github.com/Pyrokine/usb_power_meter/blob/master/FIRMWARE/ProtocolExample/PD-HW.jpg" width="400" alt="PD-HW">  
<img src="https://github.com/Pyrokine/usb_power_meter/blob/master/FIRMWARE/ProtocolExample/PD-Data.png" width="400" alt="PD-Data">  

三口QC充电  
<img src="https://github.com/Pyrokine/usb_power_meter/blob/master/FIRMWARE/ProtocolExample/QC3.0-HW.png" width="400" alt="QC3.0-HW">  
<img src="https://github.com/Pyrokine/usb_power_meter/blob/master/FIRMWARE/ProtocolExample/QC3.0-Data.png" width="400" alt="QC3.0-Data">  


## 数据格式

“01”：  
数据示例：  
其中可以增加任意其他随机数干扰，比如增加一项"mux":"random()"，使得文本拥有更高的随机性
<img src="https://github.com/Pyrokine/usb_power_meter/blob/master/FIRMWARE/ProtocolExample/01.png" width="400" alt="01">  
结果示例：  
<img src="https://github.com/Pyrokine/usb_power_meter/blob/master/FIRMWARE/ProtocolExample/01Sample.png" width="400" alt="01Sample">  
数据采用json格式转字符串发送，其中protocol指示出协议版本，内容在data中，每组数据中的key为数据（传感器）编号，value包含一个float型变量和一个str型变量，分别表示数值和单位，暂时未限制最大组数，可以根据实际情况调整


## RSA加密
算法的解释在[博客随笔](https://www.cnblogs.com/Pyrokine/p/13072526.html)  
ASCII示例：999049050051052053054055  
加密示例：<12345654321>[1111111111][2222222222][333333333]  
利用公钥对上文字符串进行分组，每一组不超过n的长度，目前选用512位加密，因为加密时转换为ASCII码，首位设为999（ASCII无意义），之后拼接49个字符对应的ASCII码，即最长为150位长度的十进制，然后进行RSA加密，加密后字符串用英文中括号包裹，然后拼接，然后将设备ID用<>包裹拼接，然后向服务器进行post即可，可以使用https方式发送
