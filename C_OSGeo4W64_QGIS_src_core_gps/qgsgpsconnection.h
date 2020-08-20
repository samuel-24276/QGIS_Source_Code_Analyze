/***************************************************************************
                          qgsgpsconnection.h  -  description
                          -------------------
    begin                : November 30th, 2009
    copyright            : (C) 2009 by Marco Hugentobler
    email                : marco at hugis dot net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSGPSCONNECTION_H
#define QGSGPSCONNECTION_H

#include <QDateTime>
#include "qgis.h"
#include <QObject>
#include <QString>

#include "qgis_core.h"

class QIODevice;

#ifdef SIP_RUN
% ModuleHeaderCode
#include "qgsgpsconnection.h"
% End
#endif

/**
 * \ingroup core
 * \class QgsSatelliteInfo
 * Encapsulates information relating to a GPS satellite.
*/
class CORE_EXPORT QgsSatelliteInfo							//导出的库文件类，卫星信息相关
{
  public:

    /**
     * Contains the satellite identifier number.包含卫星标识符
     *
     * The satellite identifier number can be used to identify a satellite inside the satellite system.卫星标识符在卫星系统中唯一标识一颗卫星。
     * For satellite system GPS the satellite identifier number represents the PRN (Pseudo-random noise)对于卫星系统GPS，卫星标识符号代表PRN（伪随机噪声）号。
     * number. For satellite system GLONASS the satellite identifier number represents the slot number. 对于卫星系统GLONASS，卫星标识符号表示插槽号。
     */
    int id = 0;

    /**
     * TRUE if satellite was used in obtaining the position fix.
     */
    bool inUse = false;

#ifndef SIP_RUN

    /**
     * Elevation of the satellite, in degrees.卫星的高度，以度为单位。
     */
    double elevation = std::numeric_limits< double >::quiet_NaN();
#else

    /**
     * Elevation of the satellite, in degrees.
     */
    double elevation;
#endif

#ifndef SIP_RUN

    /**
     * The azimuth of the satellite to true north, in degrees.卫星到正北的方位角，以度为单位。
     */
    double azimuth = std::numeric_limits< double >::quiet_NaN();
#else

    /**
     * The azimuth of the satellite to true north, in degrees.
     */
    double azimuth;
#endif

    /**
     * Signal strength (0-99dB), or -1 if not available.信号强度（0-99dB），或-1（如果不可用）。
     */
    int signal = -1;
};

/**
 * \ingroup core
 * \class QgsGpsInformation
 * Encapsulates information relating to a GPS position fix.封装与GPS定位相关的信息。
*/
class CORE_EXPORT QgsGpsInformation//===================================================================================GPS信息类====================================================
{
  public:

    /**
     * GPS fix status       GPS定位状态
     * \since QGIS 3.10
     */
    enum FixStatus
    {
      NoData,
      NoFix,
      Fix2D,
      Fix3D
    };

    /**
     * Latitude in decimal degrees, using the WGS84 datum. A positive value indicates the Northern Hemisphere, and
     * a negative value indicates the Southern Hemisphere.使用WGS84基准以十进制度数表示的纬度。 正值表示北半球，负值表示南半球。
     */
    double latitude = 0;

    /**
     * Longitude in decimal degrees, using the WGS84 datum. A positive value indicates the Eastern Hemisphere, and
     * a negative value indicates the Western Hemisphere.使用WGS84基准以十进制度数表示的经度。 正值表示东半球，负值表示西半球。
     */
    double longitude = 0;

    /**
     * Altitude (in meters) above or below the mean sea level.高于或低于平均海平面的高度（以米为单位）。
     */
    double elevation = 0;

    /**
     * Ground speed, in km/h.地面速度，以km / h为单位。
     */
    double speed = 0;

#ifndef SIP_RUN

    /**
     * The bearing measured in degrees clockwise from true north to the direction of travel.从正北到行进方向的顺时针方向度数。
     */
    double direction = std::numeric_limits< double >::quiet_NaN();
#else

    /**
     * The bearing measured in degrees clockwise from true north to the direction of travel.
     */
    double direction;
#endif

    /**
     * Contains a list of information relating to the current satellites in view.包含与所查看的当前卫星有关的信息列表。
     */
    QList<QgsSatelliteInfo> satellitesInView;

    /**
     * Dilution of precision.稀释精度
     */
    double pdop = 0;

    /**
     * Horizontal dilution of precision.水平稀释精度
     */
    double hdop = 0;

    /**
     * Vertical dilution of precision.垂直稀释精度
     */
    double vdop = 0;

#ifndef SIP_RUN
    //! Horizontal accuracy in meters水平精度，以米为单位
    double hacc = std::numeric_limits< double >::quiet_NaN();
    //! Vertical accuracy in meters垂直精度，以米为单位
    double vacc = std::numeric_limits< double >::quiet_NaN();
#else
    //! Horizontal accuracy in meters
    double hacc;
    //! Vertical accuracy in meters
    double vacc;
#endif

    /**
     * The date and time at which this position was reported, in UTC time.世界统一时间
     */
    QDateTime utcDateTime;

    /**
     * Fix mode (where M = Manual, forced to operate in 2D or 3D or A = Automatic, 3D/2D)定位模式
     */
    QChar fixMode;

    /**
     * Contains the fix type, where 1 = no fix, 2 = 2d fix, 3 = 3d fix定位类型
     */
    int fixType = 0;

    /**
     * GPS quality indicator (0 = Invalid; 1 = Fix; 2 = Differential, 3 = Sensitive)GPS质量指示器
     */
    int quality = -1;

    /**
     * Count of satellites used in obtaining the fix.定位程序的卫星数。
     */
    int satellitesUsed = 0;

    /**
     * Status (A = active or V = void)
     */
    QChar status;

    /**
     * IDs of satellites used in the position fix.定位中使用的卫星ID。
     */
    QList<int> satPrn;

    /**
     * TRUE if satellite information is complete.如果卫星信息完整，则为TRUE。
     */
    bool satInfoComplete = false;

    /**
     * Returns whether the connection information is valid返回连接信息是否有效
     * \since QGIS 3.10
     */
    bool isValid() const;

    /**
     * Returns the fix status定位状态
     * \since QGIS 3.10
     */
    FixStatus fixStatus() const;
};

/**
 * \ingroup core
 * Abstract base class for connection to a GPS device
*/
class CORE_EXPORT QgsGpsConnection : public QObject
{
#ifdef SIP_RUN
#include <qgsgpsdconnection.h>
#include <qgsnmeaconnection.h>
#endif


#ifdef SIP_RUN
    SIP_CONVERT_TO_SUBCLASS_CODE
    if ( sipCpp->inherits( "QgsGpsdConnection" ) )
      sipType = sipType_QgsGpsdConnection;
    else if ( sipCpp->inherits( "QgsNmeaConnection" ) )
      sipType = sipType_QgsNmeaConnection;
    else
      sipType = NULL;
    SIP_END
#endif

    Q_OBJECT
  public:

    enum Status
    {
      NotConnected,
      Connected,
      DataReceived,
      GPSDataReceived
    };

    /**
     * Constructor
        \param dev input device for the connection (e.g. serial device). The class takes ownership of the object
      */
    QgsGpsConnection( QIODevice *dev SIP_TRANSFER );
    ~QgsGpsConnection() override;
    //! Opens connection to device
    bool connect();
    //! Closes connection to device
    bool close();

    //! Sets the GPS source. The class takes ownership of the device class设置GPS信号源。 该类拥有设备类的所有权
    void setSource( QIODevice *source SIP_TRANSFER );

    //! Returns the status. Possible state are not connected, connected, data received返回状态
    Status status() const { return mStatus; }

    //! Returns the current gps information (lat, lon, etc.)返回当前的gps信息
    QgsGpsInformation currentGPSInformation() const { return mLastGPSInformation; }

  signals:
    void stateChanged( const QgsGpsInformation &info );
    void nmeaSentenceReceived( const QString &substring ); // added to capture 'raw' data

  protected:
    //! Data source (e.g. serial device, socket, file,...)信号源
    std::unique_ptr< QIODevice > mSource;
    //! Last state of the gps related variables (e.g. position, time, ...)GPS相关变量的最新状态
    QgsGpsInformation mLastGPSInformation;
    //! Connection status连接状态
    Status mStatus = NotConnected;

  private:
    //! Closes and deletes mSource关闭并清除信号源
    void cleanupSource();
    void clearLastGPSInformation();

  protected slots:
    //! Parse available data source content解析可用数据源内容
    virtual void parseData() = 0;
};

#endif // QGSGPSCONNECTION_H
