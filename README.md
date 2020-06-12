# usb_power_meter

硬件基于STM32F103C8T6+INA199A2DCKR+ESP8266实现的USB功率计，STM32使用stm32duino编程，ESP8266使用【开发板管理器-esp8266】直接编程

原理：在两个TYPEC的电源线间增加一个5毫欧的电阻，然后测量电阻两端的压降和其中一侧的电压，即可得到功率值，然后对结果进行格式化，并定时通过串口发送给ESP8266，然后进行RSA加密，然后传输给服务器，服务器可以使用固件自带的，需要到[pyro.cafe](https://pyro.cafe)进行注册账号使用，也可以自行搭建

附带硬件Gerber文件，可以直接提交厂家生产，开源链接为[lceda](https://lceda.cn/Pyrokine/USBgong-shuai-ji)，欢迎fork

如果已经有ESP模块，可以直接在网站注册账号，然后按格式汇报数据，这也是可行的，已提供的格式如下，会不定期更新，目前设置历史记录最大显示1w个记录点位，若10S的汇报间隔，最大能显示一天多一点的信息，可以自行调整汇报频率，如果太高的话，或者滥用服务的话，可能会被ban哦，请善待绒布球ヾ(✿ﾟ▽ﾟ)ノ

## 数据格式

“01”：  
数据示例：  
<img src="https://github.com/Pyrokine/usb_power_meter/blob/master/FIRMWARE/ProtocolExample/01.png" width="400" alt="01">  
结果示例：  
<img src="https://github.com/Pyrokine/usb_power_meter/blob/master/FIRMWARE/ProtocolExample/01Sample.png" width="400" alt="01Sample">  
数据采用json格式转字符串发送，其中protocol指示出协议版本，内容在data中，每组数据中的key为数据（传感器）编号，value包含一个float型变量和一个str型变量，分别表示数值和单位，暂时未限制最大组数，可以根据实际情况调整


## RSA加密

ASCII示例：999049050051052053054055  
加密示例：<12345654321>[1111111111][2222222222][333333333]  
利用公钥对上文字符串进行分组，每一组不超过n的长度，目前选用512位加密，因为加密时转换为ASCII码，首位设为999（ASCII无意义），之后拼接49个字符对应的ASCII码，即最长为150位长度的十进制，然后进行RSA加密，加密后字符串用英文中括号包裹，然后拼接，然后将设备ID用<>包裹拼接，然后向服务器进行post即可，可以使用https方式发送
