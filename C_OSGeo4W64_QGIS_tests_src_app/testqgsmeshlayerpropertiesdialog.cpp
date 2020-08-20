/***************************************************************************
     testqgsmeshlayerpropertiesdialog.cpp
     ------------------------------------
    Date                 : January 2019
    Copyright            : (C) 2019 by Peter Petrik
    Email                : zilolv at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "qgstest.h"
#include "qgisapp.h"
#include "qgsapplication.h"
#include "qgsmeshlayer.h"
#include "qgsmeshdataprovider.h"
#include "qgsmeshlayerproperties.h"
#include "qgsmeshrendereractivedatasetwidget.h"
#include "qgsfeedback.h"
#include "qgis.h"
#include "qgsmapcanvas.h"

#include <QTemporaryFile>

/**
 * \ingroup UnitTests
 * This is a unit test for the mesh layer properties dialog
 */
class TestQgsMeshLayerPropertiesDialog : public QObject
{
    Q_OBJECT
  public:
    TestQgsMeshLayerPropertiesDialog();

  private slots:
    void initTestCase();// will be called before the first testfunction is executed.
    void cleanupTestCase();// will be called after the last testfunction was executed.
    void init() {} // will be called before each testfunction is executed.
    void cleanup() {} // will be called after every testfunction.

    void testInvalidLayer();
    void testCrs();
    void testDatasetGroupTree();

  private:
    QgisApp *mQgisApp = nullptr;
    QgsMeshLayer *mpMeshLayer = nullptr;
};

TestQgsMeshLayerPropertiesDialog::TestQgsMeshLayerPropertiesDialog() = default;

//runs before all tests
void TestQgsMeshLayerPropertiesDialog::initTestCase()
{
  qDebug() << "TestQgisAppClipboard::initTestCase()";
  // init QGIS's paths - true means that all path will be inited from prefix
  QgsApplication::init();
  QgsApplication::initQgis();
  mQgisApp = new QgisApp();

  QString testDataDir = QStringLiteral( TEST_DATA_DIR ) + QStringLiteral( "/mesh/" );
  QString uri( testDataDir + "/quad_and_triangle.2dm" );
  mpMeshLayer = new QgsMeshLayer( uri, "Triangle and Quad MDAL", "mdal" );

  QgsProject::instance()->addMapLayers(
    QList<QgsMapLayer *>() << mpMeshLayer );
}

//runs after all tests
void TestQgsMeshLayerPropertiesDialog::cleanupTestCase()
{
  QgsApplication::exitQgis();
}

void TestQgsMeshLayerPropertiesDialog::testInvalidLayer()
{
  QgsMeshLayer invalidLayer;
  std::unique_ptr< QgsMeshLayerProperties > dialog = qgis::make_unique< QgsMeshLayerProperties> ( &invalidLayer,
      mQgisApp->mapCanvas() );

  QVERIFY( dialog );
}

void TestQgsMeshLayerPropertiesDialog::testCrs()
{
  std::unique_ptr< QgsMeshLayerProperties > dialog = qgis::make_unique< QgsMeshLayerProperties> ( mpMeshLayer,
      mQgisApp->mapCanvas()
                                                                                                );
  QCOMPARE( dialog->mCrsSelector->crs(), mpMeshLayer->crs() );
  QgsCoordinateReferenceSystem crs = QgsCoordinateReferenceSystem::fromEpsgId( 27700 );
  dialog->mCrsSelector->setCrs( crs );
  QCOMPARE( crs, mpMeshLayer->crs() );
}

void TestQgsMeshLayerPropertiesDialog::testDatasetGroupTree()
{
  QString testDataDir = QStringLiteral( TEST_DATA_DIR ) + QStringLiteral( "/mesh/" );
  QString uri( testDataDir + "/trap_steady_05_3D.nc" );
  QgsMeshLayer meshLayer( uri, "", "mdal" );

  QgsMeshRendererSettings rendererSettings = meshLayer.rendererSettings();
  rendererSettings.setActiveScalarDatasetGroup( 1 );
  meshLayer.setRendererSettings( rendererSettings );
  QCOMPARE( meshLayer.rendererSettings().activeScalarDatasetGroup(), 1 );

  QgsMeshRendererActiveDatasetWidget activeDatasetWidget;
  activeDatasetWidget.setLayer( &meshLayer );
  activeDatasetWidget.syncToLayer();

  QCOMPARE( activeDatasetWidget.activeScalarDatasetGroup(), 1 );

  std::unique_ptr<QgsMeshDatasetGroupTreeItem> rootItem( meshLayer.datasetGroupTreeRootItem()->clone() );
  rootItem->child( 1 )->setIsEnabled( false );
  meshLayer.setDatasetGroupTreeRootItem( rootItem.get() );

  QCOMPARE( activeDatasetWidget.activeScalarDatasetGroup(), 0 );

}

QGSTEST_MAIN( TestQgsMeshLayerPropertiesDialog )
#include "testqgsmeshlayerpropertiesdialog.moc"
