/***************************************************************************
                          qgsgpsconnection.cpp  -  description
                          --------------------
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

#include "qgsgpsconnection.h"

#include <QCoreApplication>
#include <QTime>
#include <QIODevice>
#include <QStringList>
#include <QFileInfo>

#include "qgsnmeaconnection.h"
#include "qgslogger.h"
#include "info.h"


bool QgsGpsInformation::isValid() const//返回连接信息是否有效
{
  bool valid = false;
  //如果 GPS质量指示器为0 或者 定位类型为NMEA_FIX_BAD 或者 GPS状态为'V'， 连接信息无效
  if ( status == 'V' || fixType == NMEA_FIX_BAD || quality == 0 ) // some sources say that 'V' indicates position fix, but is below acceptable quality
  {
    valid = false;
  }
  else if ( fixType == NMEA_FIX_2D )
  {
    valid = true;
  }
  //如果 GPS质量指示器大于0 或者 定位类型为NMEA_FIX_3D 或者 GPS状态为'A'， 连接信息有效
  else if ( status == 'A' || fixType == NMEA_FIX_3D || quality > 0 ) // good
  {
    valid = true;
  }

  return valid;
}

QgsGpsInformation::FixStatus QgsGpsInformation::fixStatus() const
{
  FixStatus fixStatus = NoData;//默认定位状态是无数据NoData

  // no fix if any of the three report bad; default values are invalid values and won't be changed if the corresponding NMEA msg is not received
  if ( status == 'V' || fixType == NMEA_FIX_BAD || quality == 0 ) // some sources say that 'V' indicates position fix, but is below acceptable quality
  {
    fixStatus = NoFix;
  }
  else if ( fixType == NMEA_FIX_2D ) // 2D indication (from GGA)
  {
    fixStatus = Fix2D;
  }
  else if ( status == 'A' || fixType == NMEA_FIX_3D || quality > 0 ) // good
  {
    fixStatus = Fix3D;
  }
  return fixStatus;
}


QgsGpsConnection::QgsGpsConnection( QIODevice *dev )
  : QObject( nullptr )
  , mSource( dev )
{
  clearLastGPSInformation();
  //监听连接设备的readyRead信号，若有此信号，则触发连接类的数据解析函数
  QObject::connect( dev, &QIODevice::readyRead, this, &QgsGpsConnection::parseData );
}

QgsGpsConnection::~QgsGpsConnection()
{
  cleanupSource();
}

bool QgsGpsConnection::connect()//连接GPS信号源
{
  if ( !mSource )
  {
    return false;
  }

  bool connected = mSource->open( QIODevice::ReadWrite | QIODevice::Unbuffered );
  if ( connected )
  {
    mStatus = Connected;
  }
  return connected;
}

bool QgsGpsConnection::close()
{
  if ( !mSource )
  {
    return false;
  }

  mSource->close();
  return true;
}

void QgsGpsConnection::cleanupSource()//清除信号源
{
  if ( mSource )
  {
    mSource->close();
  }
  mSource.reset();
}

void QgsGpsConnection::setSource( QIODevice *source )//设置信号源
{
  cleanupSource();
  mSource.reset( source );
  clearLastGPSInformation();
}

void QgsGpsConnection::clearLastGPSInformation()//清除最新的GPS信息
{
  mLastGPSInformation = QgsGpsInformation();
}
