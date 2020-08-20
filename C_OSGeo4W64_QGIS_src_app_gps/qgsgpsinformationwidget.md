# qgsgpsinformationwidget——GPS信息窗口



# 1.构造函数

## 1.1.连接按钮相关

```c++
connect( mConnectButton, &QPushButton::toggled, this, &QgsGpsInformationWidget::mConnectButton_toggled );//监听连接按钮，按下后会执行槽函数切换按钮状态
void QgsGpsInformationWidget::mConnectButton_toggled( bool flag )
{
  if ( flag )
  {
    connectGps();
  }
  else
  {
    disconnectGps();
  }
}
```

### 1.1.1.connectGps()

对应的连接GPS函数为：

```c++
void QgsGpsInformationWidget::connectGps()
{
  // 清除定位页面字段，以更好地表明发生了什么（或没有发生）
  mTxtLatitude->clear();
  mTxtLongitude->clear();
  mTxtAltitude->clear();
  mTxtDateTime->clear();
  mTxtSpeed->clear();
  mTxtDirection->clear();
  mTxtHdop->clear();
  mTxtVdop->clear();
  mTxtPdop->clear();
  mTxtFixMode->clear();
  mTxtFixType->clear();
  mTxtQuality->clear();
  mTxtSatellitesUsed->clear();
  mTxtStatus->clear();
  //以上属性全是界面的QLineEdit单行文本输入框指针

  mLastGpsPosition = QgsPointXY( 0.0, 0.0 );//最新的GPS位置设为原点

  QString port;

  //三个按钮可供选择不同的连接方式
  if ( mRadUserPath->isChecked() )//mRadUserPath是QRadioButton指针
  {
    //mCboDevices是QComboBox复选框，根据选中内容设置端口号
    port = mCboDevices->currentData().toString();

    if ( port.isEmpty() )
    {
      QMessageBox::information( this, tr( "/gps" ), tr( "No path to the GPS port "
                                "is specified. Please enter a path then try again." ) );
      //toggle the button back off
      mConnectButton->setChecked( false );//设置按钮不可选
      return;
    }
  }
  else if ( mRadGpsd->isChecked() )
  {
    port = QStringLiteral( "%1:%2:%3" ).arg( mGpsdHost->text(), mGpsdPort->text(), mGpsdDevice->text() );
  }
  else if ( mRadInternal->isChecked() )
  {
    port = QStringLiteral( "internalGPS" );
  }

  mGPSPlainTextEdit->appendPlainText( tr( "Connecting…" ) );
  showStatusBarMessage( tr( "Connecting to GPS device %1…" ).arg( port ) );

  QgsGpsDetector *detector = new QgsGpsDetector( port );//QgsGpsDetector是监听GPS端口的类
  connect( detector, static_cast < void ( QgsGpsDetector::* )( QgsGpsConnection * ) > ( &QgsGpsDetector::detected ), this, &QgsGpsInformationWidget::connected );
  connect( detector, &QgsGpsDetector::detectionFailed, this, &QgsGpsInformationWidget::timedout );
  detector->advance();   // start the detection process
}
```

### 1.1.2.disconnectGps()

对应的断开GPS连接的函数：

```c++
void QgsGpsInformationWidget::disconnectGps()
{
  if ( mLogFile && mLogFile->isOpen() )//mLogFile是头文件定义的文件指针
  {
    //mNmea是头文件定义的QgsGpsConnection GPS连接指针
    disconnect( mNmea, &QgsGpsConnection::nmeaSentenceReceived, this, &QgsGpsInformationWidget::logNmeaSentence );
    mLogFile->close();//写入完成后关闭
    delete mLogFile;
    mLogFile = nullptr;
  }

  //unregisterConnection是QgsGpsConnectionRegistry类的函数，作用是移除连接
  QgsApplication::gpsConnectionRegistry()->unregisterConnection( mNmea );
  delete mNmea;
  mNmea = nullptr;
  //mMapMarker是QgsGpsMarker类的一个指针，用于标记gps指针位置
  if ( mMapMarker )  // marker should not be shown on GPS disconnected - not current position
  {
    delete mMapMarker;
    mMapMarker = nullptr;
  }
 //mMapBearingItem是QgsGpsBearingItem类的指针，该类用于显示GPS设备方位的画布项目
  if ( mMapBearingItem )
  {
    delete mMapBearingItem;
    mMapBearingItem = nullptr;
  }
  mGPSPlainTextEdit->appendPlainText( tr( "Disconnected…" ) );
  mConnectButton->setChecked( false );
  mConnectButton->setText( tr( "&Connect" ) );
  showStatusBarMessage( tr( "Disconnected from GPS device." ) );

  setStatusIndicator( NoData );
}
```

### 1.1.3.logNmeaSentence()

```c++
void QgsGpsInformationWidget::logNmeaSentence( const QString &nmeaString )
{
  //mLogFileGroupBox是QgsCollapsibleGroupBox指针
  if ( mLogFileGroupBox->isChecked() && mLogFile && mLogFile->isOpen() )
  {
    //mLogFileTextStream是一个QTextStream文本流指针，写入NMEA数据和回车换行
    mLogFileTextStream << nmeaString << "\r\n"; // specifically output CR + LF (NMEA requirement)
  }
}
```

关联的类：

- `QgsPointXY`——表示2D点的类。
  `QgsPointXY`表示具有X和Y坐标的位置。在大多数情况下，最好使用也支持Z和M值的`QgsPoint`。

- `QgsGpsDetector`——监听GPS端口的类

- `QgsGpsConnection`——GPS连接类

- `QgsGpsConnectionRegistry`——注册/取消注册现有GPS连接的类，以便所有类和插件均可使用该信息。

  `QgsGpsConnectionRegistry`通常不是直接创建的，而是通过`QgsApplication :: gpsConnectionRegistry（）`访问的。

- `QgsGpsMarker`——应该是在画布上标记gps指针位置的类
- `QgsGpsBearingItem`——用于显示GPS设备方位的画布项目的类

关联的属性：

- `mLastGpsPosition`——最新的GPS位置，是`QgsPointXY`指针
- `mLogFile`——`QFile`指针
- `mNmea`——`QgsGpsConnection`指针，表示GPS连接
- `mLogFileTextStream`——`QTextStream`文本流指针，存储NMEA数据
- `mMapMarker`——`QgsGpsMarker`指针，标记gps指针位置
- `mMapBearingItem`——`QgsGpsBearingItem`类指针，用于显示GPS设备方位的画布项目

## 1.2.居中/聚焦按钮相关

```c++
connect( mRecenterButton, &QPushButton::clicked, this, &QgsGpsInformationWidget::recenter );				//聚焦地图，应该是以当前定位点为中心显示地图
void QgsGpsInformationWidget::recenter()
{
  try
  {
    const QgsPointXY center = mCanvasToWgs84Transform.transform( mLastGpsPosition, QgsCoordinateTransform::ReverseTransform );
    mMapCanvas->setCenter( center );
    mMapCanvas->refresh();
  }
  catch ( QgsCsException & )
  {

  }
}
```

mLastGpsPosition是头文件138行定义的`QgsPointXY`对象，描述最新的GPS位置。

**QgsCoordinateTransform**类用于在两个地图坐标系之间进行转换。此类可以将地图坐标转换为其他坐标参考系统。
它通常与地图图层相关联，并且可以在图层的坐标系和地图画布的坐标系之间进行转换，尽管可以在更一般的意义上使用它来转换坐标。
当用于在图层和地图画布之间进行转换时，所有对源坐标系和目标坐标系的引用都分别指图层和地图画布。 所有操作都是从该层的角度进行的。 例如，正向变换将坐标从图层的坐标系转换到地图画布。

