//  This is ThermoFun library+API (https://bitbucket.org/gems4/ThermoFun)
//
/// \file ThermoFunWidget.cpp
/// ThermoFunWidget - Widget to work with ThermoFun data
//
// BSONUI is a C++ Qt5-based widget library and API aimed at implementing
// the GUI for editing/viewing the structured data kept in a NoSQL database,
// with rendering documents to/from JSON/YAML/XML files, and importing/
// exporting data from/to arbitrary foreign formats (csv etc.). Graphical
// presentation of tabular data (csv format fields) is also possible.
//
// Copyright (c) 2015-2016 Svetlana Dmytriieva (svd@ciklum.com) and
//   Dmitrii Kulik (dmitrii.kulik@psi.ch)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU (Lesser) General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// BSONUI depends on the following open-source software products:
// BSONIO (https://bitbucket.org/gems4/bsonio); Qt5 (https://qt.io);
// Qwtplot (http://qwt.sourceforge.net).

#include <iostream>
#include <QSettings>
#include <QByteArray>
#include <QMessageBox>
#include <QKeyEvent>
#include <QSortFilterProxyModel>
#include "ThermoFunWidget.h"
#include "ui_ThermoFunWidget.h"
#include "bsonui/TableEditWindow.h"
#include "bsonui/QueryWidget.h"
using namespace bsonio;


ThermoFunData::ThermoFunData()
{
  name = "ThermoFunTask1";
  comment = "write comment here...";
  calcStatus = "...";
  schemaName = "VertexSubstance";
  query = "";
  T =25;
  tPrecision = 0;
  unitsT = "C Celsius";
  P =1;
  pPrecision = 0;
  tppairs.push_back({25.,1.});
  unitsP = "b bar";
  properties.push_back("gibbs_energy");
  propertyUnits.push_back("J/mol");
  propertyPrecision.push_back(0);
}


void ThermoFunData::toBson( bson *obj ) const
{
    bson_append_string( obj, "Name", name.c_str() );
    bson_append_string( obj, "Description", comment.c_str() );
    bson_append_string( obj, "SchemaName", schemaName.c_str() );
    if(!query.empty())
    {
        bson whilebson;
        jsonToBson( &whilebson, query );
        bson_append_bson( obj, "Query", &whilebson  );
        bson_destroy(&whilebson);
    }
    bson_append_double( obj,"Temperature", T );
    bson_append_double( obj,"TemperaturePrecision", tPrecision );
    bson_append_string( obj,"TemperatureUnits", unitsT.c_str() );
    bson_append_double( obj,"Pressure", P );
    bson_append_double( obj,"PressurePrecision", pPrecision );
    bson_append_string( obj,"PressureUnits", unitsP.c_str() );

    bson_append_start_array(obj, "TemperaturePressurePoints");
    for(uint ii=0; ii<tppairs.size(); ii++)
    {
        bson_append_start_object( obj, to_string(ii).c_str() );
        bson_append_double( obj, "T", tppairs[ii][0] );
        bson_append_double( obj, "P", tppairs[ii][1] );
       bson_append_finish_object(obj);
    }
    bson_append_finish_array(obj);

    bson_append_start_array(obj, "PropertiesList");
    for(uint ii=0; ii<properties.size(); ii++)
       bson_append_string( obj, to_string(ii).c_str(), properties[ii].c_str() );
    bson_append_finish_array(obj);
    bson_append_start_array(obj, "PropertyUnits");
    for(uint ii=0; ii<propertyUnits.size(); ii++)
       bson_append_string( obj, to_string(ii).c_str(), propertyUnits[ii].c_str() );
    bson_append_finish_array(obj);
    bson_append_start_array(obj, "PropertyPrecision");
    for(uint ii=0; ii<propertyPrecision.size(); ii++)
       bson_append_int( obj, to_string(ii).c_str(), propertyPrecision[ii] );
    bson_append_finish_array(obj);
}

void ThermoFunData::fromBson( const char* bsobj )
{
    ThermoFunData deflt;
    vector<double> pointsT, pointsP;

    if(!bson_find_string( bsobj, "Name", name ) )
        name=deflt.name;
    if(!bson_find_string( bsobj, "Description", comment ) )
        comment=deflt.comment;
    if(!bson_find_string( bsobj, "SchemaName", schemaName ) )
        schemaName=deflt.schemaName;
    if(!bson_to_string( bsobj, "Query", query ) )
        query=deflt.query;

    if(!bson_find_value( bsobj, "Temperature", T ) )
        T=deflt.T;
    if(!bson_find_value( bsobj, "TemperaturePrecision", tPrecision ) )
        tPrecision=deflt.tPrecision;
    if(!bson_find_string( bsobj, "TemperatureUnits", unitsT ) )
        unitsT=deflt.unitsT;

    if(!bson_find_value( bsobj, "Pressure", P ) )
        P=deflt.P;
    if(!bson_find_value( bsobj, "PressurePrecision", pPrecision ) )
        pPrecision=deflt.pPrecision;
    if(!bson_find_string( bsobj, "PressureUnits", unitsP ) )
        unitsP=deflt.unitsP;

    // read tp pairs
    tppairs.clear();
    bson_iterator it;
    bson_type type;
    type =  bson_find_from_buffer(&it, bsobj, "TemperaturePressurePoints" );
    if( type != BSON_ARRAY )
      tppairs =deflt.tppairs;
    else
    {
       double Ti, Pi;
       bson_iterator i;
       bson_iterator_from_buffer(&i, bson_iterator_value(&it));
       while (bson_iterator_next(&i))
       {
         if(!bson_find_value( bson_iterator_value(&i), "T", Ti ) )
            Ti=100.;
         if(!bson_find_value( bson_iterator_value(&i), "P", Pi ) )
            Pi = 5.;
         tppairs.push_back({Ti, Pi});
       }
    }

    bson_read_array( bsobj, "PropertiesList", properties );
    if( properties.empty() )
        properties =deflt.properties;
    bson_read_array( bsobj, "PropertyUnits", propertyUnits );
    if( properties.empty() )
        properties =deflt.properties;
    bson_read_array( bsobj, "PropertyPrecision", propertyPrecision );
    if( properties.empty() )
        properties =deflt.properties;
    }


//----------------------------------------------------------------------

void ThermoFunWidget::closeEvent(QCloseEvent* e)
{
    if( queryResultWindow )
     queryResultWindow->close();

    if( queryWindow )
     queryWindow->close();

    if( _csvWin )
     _csvWin->close();

    if( !onCloseEvent(this) )
           e->ignore();
       else
           QWidget::closeEvent(e);
}


ThermoFunWidget::ThermoFunWidget(QSettings *amainSettings,ThriftSchema *aschema, QWidget *parent) :
    BSONUIBase(amainSettings, aschema, parent),
    curSchemaName(""), ui(new Ui::ThermoFunWidget),
    dataTable(0), pTable(0), tableModel(0), queryWindow(0), queryResultWindow(0)
{
    _shemaNames.push_back("VertexSubstance");
    _shemaNames.push_back("VertexReaction");
    _shemaNames.push_back("VertexReactionSet");
    curSchemaName = _shemaNames[0];

    // set up default data
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose); // automatically delete itself when window is closed
    QString title = qApp->applicationName()+" Structured Data Editor and Database Browser";
    setWindowTitle(title);

    //set up main parameters
    bson_init(&curRecord);
    bson_finish(&curRecord);

    //define schema checkbox
    for( uint ii=0; ii<_shemaNames.size(); ii++ )
      ui->typeBox->addItem(_shemaNames[ii].c_str());
    ui->typeBox->setCurrentText(curSchemaName.c_str());

   // define edit tree view
   aHeaderData << "key" << "value"  << "comment" ;
   fieldTable =  new TBsonView(  ui->bsonWidget );
   model_schema = new TSchemaNodeModel(  schema, &curRecord,
        curSchemaName, aHeaderData, this/*ui->centralWidget*/ );
   deleg_schema = new TSchemaNodeDelegate();
   fieldTable->setModel(model_schema);
   fieldTable->setItemDelegate(deleg_schema);
   fieldTable->setColumnWidth( 0, 150 );
   fieldTable->expandToDepth(0);
   ui->gridLayout->addWidget(fieldTable, 1, 0, 1, 2);
   fieldTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
   disconnect( fieldTable, SIGNAL(customContextMenuRequested(QPoint)),
          fieldTable, SLOT(slotPopupContextMenu(QPoint)));

   // define all keys tables
   defKeysTables();
   resetDBClient( curSchemaName );

   // define ThermoFun data
   _TPContainer = new TPContainer( "T", { "T", "P" }, _data.tppairs );
   _TPlistTable  = new TMatrixTable( ui->outWidget );
   TMatrixDelegate* deleg = new TMatrixDelegate();
   _TPlistTable->setItemDelegate(deleg);
   _TPlistModel = new TMatrixModel( _TPContainer, this );
   _TPlistTable->setModel(_TPlistModel);
   _TPlistTable->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch/*Interactive*/ );
   ui->gridLayout_3->addWidget(_TPlistTable, 1, 0, 1, 1);

   _PropertyContainer = new TPropertyContainer( "Property", _data.properties, _data.propertyUnits, _data.propertyPrecision );
   _PropertyTable  = new TMatrixTable( ui->inWidget );
    deleg = new TMatrixDelegate();
   _PropertyTable->setItemDelegate(deleg);
   _PropertyModel = new TMatrixModel( _PropertyContainer, this );
   _PropertyTable->setModel(_PropertyModel);
   _PropertyTable->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch/*Interactive*/ );
   ui->gridLayout_2->addWidget(_PropertyTable, 6, 0, 1, 7);
   ui->pName->setText(_data.name.c_str());
   ui->pComment->setText(_data.comment.c_str());
   ui->pTVal->setValue(_data.T);
   ui->pPVal->setValue(_data.P);
   ui->pTunit->setCurrentText( _data.unitsT.c_str());
   ui->pPunit->setCurrentText(_data.unitsP.c_str());
   ui->pPrecision->setValue(_data.pPrecision);
   ui->tPrecision->setValue(_data.tPrecision);

   ui->calcStatus->setText(_data.calcStatus.c_str());

   // define menu
   setActions();
   ui->keySplitter->setStretchFactor(0, 1);
   ui->mainSplitter->setStretchFactor(0, 0);
   ui->mainSplitter->setStretchFactor(1, 0);
   ui->mainSplitter->setStretchFactor(2, 1);
   ui->mainSplitter->setCollapsible(0, false);
   ui->mainSplitter->setCollapsible(2, false);

   // reset in/out queries
   if(  pTable->model()->rowCount() > 0 )
   {  // read first line
       openRecordKey( pTable->model()->index(0,0) );
   }else
     CmNew();
}

ThermoFunWidget::~ThermoFunWidget()
{
    // free keys query data
    if(queryResultWindow)
    {
       queryResultWindow->close();
       delete queryResultWindow;
    }
    if(queryWindow)
    {
       queryWindow->close();
       delete queryWindow;
    }

    if( _csvWin )
     delete _csvWin;

    if( dataTable )
      delete dataTable;
    if( pTable )
      delete pTable;

    delete _TPlistTable;
    delete _TPContainer;
    delete _PropertyTable;
    delete _PropertyContainer;

    delete ui;
    cout << "~ThermoFunWidget" << endl;
}

//-----------------------------------------------------

//
void ThermoFunWidget::resetTypeBox( const QString & text )
{
    disconnect( ui->typeBox, SIGNAL(currentIndexChanged(const QString&)),
             this, SLOT(typeChanged(const QString&)));
    ui->typeBox->setCurrentText(text);
    connect( ui->typeBox, SIGNAL(currentIndexChanged(const QString&)),
             this, SLOT(typeChanged(const QString&)));
}

/// Change current Edge
void ThermoFunWidget::typeChanged(const QString & text)
{
  try {
        string newname = text.toUtf8().data();
        if(newname == curSchemaName)
          return;

        curSchemaName = newname;
        _data.schemaName = curSchemaName;

        bson_destroy( &curRecord );
        bson_init( &curRecord );
        bson_finish( &curRecord );
        model_schema->setupModelData( &curRecord, curSchemaName);
        // fieldTable->expandToDepth(0);

        if(!isDefaultQuery)
        {  resetDBClient(curSchemaName );
           if(  pTable->model()->rowCount() > 0 )
              openRecordKey( pTable->model()->index(0,0), true );
        }
        else
         dbgraph->resetSchema( curSchemaName, false );

        CmResetThermoFunData();

    }
   catch(bsonio_exeption& e)
   {
       QMessageBox::critical( this, e.title(), e.what() );
   }
   catch(std::exception& e)
    {
       QMessageBox::critical( this, "std::exception", e.what() );
    }
}

void ThermoFunWidget::resetDBClient(const string& schemaName_ )
{
  string defaultQuery="";
  if( isDefaultQuery)
    defaultQuery = _data.query;
  TDBGraph* newClient = newDBClient( schemaName_, defaultQuery );
    // no schema
  if( newClient == nullptr )
  {
      dbgraph.reset();
      ui->edgeQuery->setText("");
      return;
  }
  ui->edgeQuery->setText(newClient->GetLastQuery().c_str());
  dbgraph.reset( newClient );
  try{
         dbgraph->Open();
       }
   catch(bsonio_exeption& e)
    {
        cout << "Internal comment " << e.title() << e.what() << endl;
        // QMessageBox::critical( this, e.title(), e.what() );
    }
    catch(std::exception& e)
     {
        QMessageBox::critical( this, "std::exception", e.what() );
     }
    resetKeysTable();
    dbgraph->setKey( "*" );
}


void ThermoFunWidget::defKeysTables()
{
    // define table of keys
    pTable = new TKeyTable(ui->keySplitter,
                   [=]( const QModelIndex& index ){openRecordKey( index );});
    //TMatrixDelegate* deleg = new TMatrixDelegate();
    //pTable->setItemDelegate(deleg);
    ui->keySplitter->insertWidget(0, pTable);
    ui->keySplitter->setStretchFactor(0, 4);
    //pTable->horizontalHeader()->hide();
    pTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    pTable->horizontalHeader()->setSectionResizeMode( QHeaderView::Interactive );
    pTable->setEditTriggers( QAbstractItemView::AnyKeyPressed );
    pTable->setSortingEnabled(true);
    disconnect( pTable, SIGNAL(customContextMenuRequested(QPoint)),
           pTable, SLOT(slotPopupContextMenu(QPoint)));
    connect( pTable, SIGNAL( clicked(const QModelIndex& ) ), this, SLOT(openRecordKey( const QModelIndex& )));
}

void ThermoFunWidget::resetKeysTable()
  {
    if( dataTable )
      delete dataTable;
    dataTable = new TKeyListTableNew("select", dbgraph.get());
    tableModel = new TMatrixModel( dataTable, this );
    QSortFilterProxyModel *proxyModel;
    proxyModel = new QSortFilterProxyModel();
    proxyModel->setSourceModel( tableModel );
    pTable->setModel(proxyModel);
    pTable->hideColumn(0);
}

bool ThermoFunWidget::resetBson(const string& schemaName )
{
   // test legal schema name
   if( !curSchemaName.empty() &&
       !schemaName.empty() && schemaName != curSchemaName )
   return false;
   fieldTable->updateModelData( &curRecord, curSchemaName );
   fieldTable->update();
   return true;
}

void ThermoFunWidget::openRecordKey(  const QModelIndex& rowindex , bool resetInOutQuery  )
{
    // find key
    int row = rowindex.row();
    QModelIndex index = rowindex.sibling( row, 0);
    string key = pTable->model()->data(index).toString().toUtf8().data();

    // Read Record
    openRecordKey(  key, resetInOutQuery  );
}


void ThermoFunWidget::openRecordKey(  const string& reckey, bool  )
{
   try{
        if( isDefaultQuery )
         dbgraph->resetMode(true);

        // Read Record
        dbgraph->GetRecord( reckey.c_str() );

        if( isDefaultQuery )
        {
           dbgraph->resetMode(false);
           string _schema = dbgraph->GetSchemaName();
           if( _schema != curSchemaName )
           {
               curSchemaName = _schema;
               resetTypeBox( curSchemaName.c_str() );
               //typeChanged(_schema.c_str() );
           }
        }

        //string valDB = dbclient->GetJson();
        bson_destroy( &curRecord );
        //jsonToBson( &curRecord, valDB );
        dbgraph->GetBson(&curRecord);
        resetBson( ""/*dbclient->getKeywd()*/ );
        contentsChanged = false;

    }
    catch(bsonio_exeption& e)
    {
        QMessageBox::critical( this, e.title(), e.what() );
    }
    catch(std::exception& e)
     {
        QMessageBox::critical( this, "std::exception", e.what() );
     }

}

void ThermoFunWidget::changeKeyList()
{
    // reset model data
    tableModel->resetMatrixData();

    string key = dbgraph->getKeyFromBson( curRecord.data );
    // undefined current record
    if( key.empty() ||  key.find_first_of("*?") != std::string::npos )
      return;

    pTable->setColumnHidden(0, false);
    // search `item` text in model
    QModelIndexList Items =  pTable->model()->match(  pTable->model()->index(0, 0),
       Qt::DisplayRole, QString(key.c_str()));
    if (!Items.isEmpty())
    {
        pTable->selectionModel()->select(Items.first(), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
        pTable->scrollTo(Items.first(), QAbstractItemView::EnsureVisible);
    }
    pTable->setColumnHidden(0, true);
}

/// Reset new ThermoFun data
void ThermoFunWidget::resetThermoFunData()
{
   _TPlistModel->resetMatrixData();
   _PropertyModel->resetMatrixData();

    ui->pName->setText(_data.name.c_str());
    ui->pComment->setText(_data.comment.c_str());
    ui->pTVal->setValue(_data.T);
    ui->tPrecision->setValue(_data.tPrecision);
    ui->pPVal->setValue(_data.P);
    ui->pPrecision->setValue(_data.pPrecision);
    ui->pTunit->setCurrentText( _data.unitsT.c_str());
    ui->pPunit->setCurrentText(_data.unitsP.c_str());
//    ui->FormatBox->setChecked(_data.isFixedFormat);

    isDefaultQuery = false;
    resetTypeBox( _data.schemaName.c_str() );
    typeChanged(_data.schemaName.c_str());
    // set new query
    isDefaultQuery = !_data.query.empty();
    if( dbgraph.get() != 0 )
    {
        if(!_data.query.empty() )
        { dbgraph->SetQueryJson(_data.query);
          dbgraph->runQuery();
          ui->edgeQuery->setText(_data.query.c_str());
        }

        // update search tables
        tableModel->resetMatrixData();
        if( queryResultWindow )
         queryResultWindow->updateTable();
    }
}

//-------------------------------------------------------------

/// Change current View menu selections
void ThermoFunWidget::updtViewMenu()
{
    ui->action_Show_comments->setChecked( TSchemaNodeModel::showComments );
    ui->action_Display_enums->setChecked(TSchemaNodeModel::useEnumNames);
    ui->action_Edit_id->setChecked(TSchemaNodeModel::editID );
    ui->actionKeep_Data_Fields_Expanded->setChecked(TBsonView::expandedFields );
}

/// Change model ( show/hide comments)
void ThermoFunWidget::updtModel()
{
   model_schema->updateModelData();
   fieldTable->header()->resizeSection(0, 150);
   fieldTable->header()->resizeSection(1, 250);
   fieldTable->expandToDepth(0);
}

/// Change table (Show Enum Names Instead of Values)
void ThermoFunWidget::updtTable()
{
    fieldTable->hide();
    fieldTable->show();
}

/// Update after change DB locations
void ThermoFunWidget::updtDB()
{
    // set up new dbclient
    resetDBClient( curSchemaName );
    // ??? clear current record
    CmNew();
}

/// Change flag "Display Comments to Data Fields"
void ThermoFunWidget::CmShowComments()
{
   showComments( ui->action_Show_comments->isChecked() );
}

/// Change flag "Show Enum Names Instead of Values"
void ThermoFunWidget::CmDisplayEnums()
{
    displayEnums(ui->action_Display_enums->isChecked());
}

/// Change flag "Edit _id of DB Documents (Records) "
void ThermoFunWidget::CmEditID()
{
  editID( ui->action_Edit_id->isChecked());
}

/// Change flag "Keep data fields expanded"
void ThermoFunWidget::CmEditExpanded()
{
  editExpanded( ui->actionKeep_Data_Fields_Expanded->isChecked() );
}
