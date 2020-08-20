```c++
#include "qgis_core.h"
#include "qgsgpsconnection.h"
```



#  1.QgsNmeaConnection

> 继承了之前的**QgsGpsConnection**连接类，实现了**parseData()**槽函数

## 1.1.public

- `QgsNmeaConnection( QIODevice *device SIP_TRANSFER );`继承基类的构造函数

## 1.2.protected

进行处理之前存储在设备中的数据**mStringBuffer**——QString

`void processStringBuffer();`将mStringBuffer拆分为句子并调用libnmea，根据句子内容的不同调用以下不同函数进行处理。

- `void processGgaSentence( const char *data, int len );`处理GGA格式的数据，第一参数为数据的指针，第二参数为数据的长度。
- `void processRmcSentence( const char *data, int len );`处理GGA格式的数据
- `void processGsvSentence( const char *data, int len );  `  处理GSV格式的数据
- `void processVtgSentence( const char *data, int len );    `处理VTG格式的数据
- `void processGsaSentence( const char *data, int len );   ` 处理GSA格式的数据
- `void processGstSentence( const char *data, int len );   ` 处理GST格式的数据
- `void processHdtSentence( const char *data, int len );    `处理HDT格式的数据
- `void processHchdgSentence( const char *data, int len );`处理HCHDG格式的数据
- `void processHchdtSentence( const char *data, int len );`处理HCHDT格式的数据

以上所有处理各种格式数据的函数都用到了nmea_parse_数据格式()的函数，该函数在本文件内没有定义，只能是在包含的头文件里的关于libnmea的三个头文件里定义：

```c++
#include "qgsnmeaconnection.h"
#include "qgslogger.h"

#include <QIODevice>
#include <QApplication>
#include <QStringList>

//from libnmea，以下三个头文件在C:\OSGeo4W64\QGIS\external\nmea目录下
#include "parse.h"
#include "gmath.h"
#include "info.h"

// for sqrt
#include <math.h>
```

# 2.parse

## 2.1.parse.h

定义了名为`nmeaPARSER`的结构体，代码如下：

```c++
typedef struct _nmeaPARSER
{
  void *top_node = nullptr;
  void *end_node = nullptr;
  unsigned char *buffer;	//定义缓冲区
  int buff_size;			//缓冲区大小
  int buff_use;				//已使用的缓冲区大小

} nmeaPARSER;
```

接下来定义几个相关函数：

```c++
int     nmea_parser_init( nmeaPARSER *parser );				//解析器初始化
void    nmea_parser_destroy( nmeaPARSER *parser );			//解析器销毁

int     nmea_parse(
  nmeaPARSER *parser,
  const char *buff, int buff_sz,
  nmeaINFO *info				//把NMEA信息类作为成员变量，该类定义了符合NMEA标准的各种参数，如经纬度，UTC等，在info.h里定义
);	


int     nmea_parser_push( nmeaPARSER *parser, const char *buff, int buff_sz );
int     nmea_parser_top( nmeaPARSER *parser );
int     nmea_parser_pop( nmeaPARSER *parser, void **pack_ptr );
int     nmea_parser_peek( nmeaPARSER *parser, void **pack_ptr );
int     nmea_parser_drop( nmeaPARSER *parser );
int     nmea_parser_buff_clear( nmeaPARSER *parser );
int     nmea_parser_queue_clear( nmeaPARSER *parser );
```

## 2.2.parse.c

- `int _nmea_parse_time( const char *buff, int buff_sz, nmeaTIME *res )`解析NMEA标准数据里的时间，解析成功返回0，失败返回-1。参数buff是被解析的时间数据，buff_sz是解析数据的长度，根据不同长度把时间数据解析成不同格式（如hhmmss、hhmmss.s、hhmmss.ss、hhmmss.sss等)。

- `int nmea_pack_type( const char *buff, int buff_sz )`通过标题定义数据包类型,buff是buff数据包缓冲区的常量字符指针，buff_sz是数据包大小，函数内部定义了9种格式即9种数据包类型，通过字符串比较函数确定是哪种格式。

- `int nmea_find_tail( const char *buff, int buff_sz, int *res_crc )`在缓冲区中找到数据包的尾部（“ \ r \ n”）并**检查控制和（CRC）**。res_crc是一个整数指针，用于返回数据包的CRC（必须定义），整个函数返回包尾的字节数。

- `int nmea_parse_GPGGA( const char *buff, int buff_sz, nmeaGPGGA *pack )`解析buff为GPGGA数据，解析成功返回1，解析失败返回0。pack是一个数据包的指针，解析完成后的数据将赋值给pack。这个函数还不是最底层，依旧调用了其他函数。

- `int nmea_parse_GPGST( const char *buff, int buff_sz, nmeaGPGST *pack )`

- `int nmea_parse_GPGSA( const char *buff, int buff_sz, nmeaGPGSA *pack )`

- `int nmea_parse_GPGSV( const char *buff, int buff_sz, nmeaGPGSV *pack )`

- `int nmea_parse_GPRMC( const char *buff, int buff_sz, nmeaGPRMC *pack )`

- `int nmea_parse_GPHDT( const char *buff, int buff_sz, nmeaGPHDT *pack )`

- `int nmea_parse_GPVTG( const char *buff, int buff_sz, nmeaGPVTG *pack )`

- `int nmea_parse_HCHDG( const char *buff, int buff_sz, nmeaHCHDG *pack )`

- `int nmea_parse_HCHDT( const char *buff, int buff_sz, nmeaHCHDT *pack )`

- `void nmea_GPGGA2info( nmeaGPGGA *pack, nmeaINFO *info )`

  通过GGA数据包数据填充nmeaINFO结构

- `void nmea_GPGST2info( nmeaGPGST *pack, nmeaINFO *info )`

- `void nmea_GPGSA2info( nmeaGPGSA *pack, nmeaINFO *info )`

- `void nmea_GPGSV2info( nmeaGPGSV *pack, nmeaINFO *info )`

- `void nmea_GPRMC2info( nmeaGPRMC *pack, nmeaINFO *info )`

- `void nmea_GPVTG2info( nmeaGPVTG *pack, nmeaINFO *info )`

# 3.info&&sentence

在头文件里定义了以下几个结构体：

- 定位数据**nmeaPOS**，包含经度和纬度两个属性
- 卫星数据**nmeaSATELLITE**，包含卫星id，用于定位的数量，海拔高度，方位角，信号强度5个属性
- 有关所有可见卫星的信息**nmeaSATINFO**，包含可见卫星数量，卫星信息，使用中的卫星数（不在可见范围内）3个属性
- 来自所有已解析数据包的摘要GPS信息**nmeaINFO**，也用于生成NMEA流

还有一个用于生成零数据的函数`void nmea_zero_INFO( nmeaINFO *info );`

生成其他格式的零数据的函数在**sentence.h**里定义，还定义了每种格式对应的枚举类型`nmeaPACKTYPE`和每种格式的结构体：nmeaGPGGA、nmeaGPGST、nmeaGPGSA、nmeaGPGSV、nmeaGPRMC、nmeaGPVTG、nmeaGPHDT、nmeaHCHDG、nmeaHCHDT。