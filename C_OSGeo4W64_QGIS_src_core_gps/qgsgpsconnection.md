# 1.QgsSatelliteInfo卫星信息类

## 1.1.Public属性

卫星标识符id——int

使用状态标识符inUse——bool

信号强度signal，以dB为单位，-1表示不可用——Int

## 2.2.宏定义属性

如果没有定义宏`SIP_RUN`，则定义以下属性：

卫星高度**elevation**——double

卫星到正北方向的方位角**azimuth**，以度为单位——double



# 2.GPS信息类QgsGpsInformation

## 2.1.public属性

GPS定位状态**FixStatus**——枚举类型

使用WGS84基准以十进制度数表示的纬度**latitude**——double

使用WGS84基准以十进制度数表示的经度**longitude**——double

海拔高度**elevation**——double

地面速度**speed**——double

卫星信息类列表**satellitesInView**——QList<QgsSatelliteInfo>

稀释精度**pdop**——double

水平稀释精度**hdop**——double

垂直稀释精度**vdop**——double

世界统一时间**utcDateTime**——QDateTime

定位模式**fixMode**——QChar

- 定位类型**fixType**——int

- GPS质量指示器（0 =无效； 1 =固定； 2 =差分，3 =敏感）**quality**——int

- GPS状态**status**——QChar，以上三者决定了GPS定位的状态

定位程序的卫星数**satellitesUsed**——int

定位中使用的卫星ID**satPrn**——QList<int>

卫星信息完整标识符**satInfoComplete**——bool

## 2.2.宏定义属性

如果没有定义宏**SIP_RUN**，则定义以下属性：

从正北到行进方向的顺时针方向度数**direction**——double

水平精度，以米为单位**hacc**——double

垂直精度，以米为单位**vacc**——double

## 2.3.public函数

连接信息是否有效**isValid()**——bool const

定位状态**fixStatus()**——枚举类型FixStatus const

# 3.GPS连接类QgsGpsConnection

> 根据宏**SIP_RUN**来判断连接类型**sipType**是GPS还是NMEA
>
> 该类继承QObject类，可以使用信号和槽机制

## 3.1.public

连接类型**Status**——枚举类型

- `QgsGpsConnection( QIODevice *dev SIP_TRANSFER );`初始化连接后监听连接设备的readyRead信号，若有此信号，则触发连接类的数据解析函数

- `~QgsGpsConnection() override;`清除信号源

- 连接设备**connect()**——bool

  无信号源直接返回；否则以读写和无缓冲方式打开信号源，若打开成功，则`mStatus = Connected;`，返回打开的结果。

- 关闭与设备的连接**close()**——bool

- 设置GPS信号源。 该类拥有设备类的所有权**setSource(QIODevice *source SIP_TRANSFER)**——void

  清除信号源，并使用参数的source 进行信号源重置，清除最新的GPS信息

- 返回状态**status()**——Status const

- 返回当前的gps信息**currentGPSInformation()**——QgsGpsInformation const

## 3.2.信号

- `void stateChanged( const QgsGpsInformation &info );`GPS状态改变发出此信号
- `void nmeaSentenceReceived( const QString &substring );`NMEA接收到句子后发出此信号

## 3.3.protected

- 信号源**mSource**，可以是串口设备，也可以是socket或文件——std::unique_ptr< QIODevice >
- GPS相关变量的最新状态**mLastGPSInformation**，如定位，时间——QgsGpsInformation
- 连接状态**mStatus**，初始化为未连接——Status

## 3.4.private

- `void cleanupSource();`关闭并清除信号源，如果有信号源则关闭，之后reset重置
- `void clearLastGPSInformation();`清除最新的GPS信息，即`mLastGPSInformation = QgsGpsInformation();`

## 3.5.protected slots

- `virtual void parseData() = 0;`解析可用数据源内容



