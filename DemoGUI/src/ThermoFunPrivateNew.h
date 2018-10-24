#ifndef THERMOFUNWIDGETPRIVATE_H
#define THERMOFUNWIDGETPRIVATE_H

#include <QObject>
#include <QtWidgets>
#include "ThermoFunWidgetNew.h"
#include "thermomodel.h"
#include "containers.h"
// ThermoFun includes
#ifdef FROM_SRC
#include "../src/Interfaces/Interface.h"
#include "../src/DBClient/DatabaseClient.h"
#include "../src/DBClient/SubstanceData.h"
#include "../src/Reaction.h"
#include "../src/Substance.h"
#else
#include "thermofun/Interfaces/Interface.h"
#include "thermofun/DBClient/DatabaseClient.h"
#include "thermofun/DBClient/SubstanceData.h"
#include "thermofun/Reaction.h"
#include "thermofun/Substance.h"
#endif

using MapLevelReaction             = map<string, ThermoFun::Reaction>;
using MapLevelSubstances           = map<string, vector<ThermoFun::Substance>>;
using MapSymbolMapLevelReaction    = map<string, MapLevelReaction>;
using MapSymbolMapLevelSubstances  = map<string, MapLevelSubstances>;

extern const int defSourcetdb;

/// ThermoFun data to calculation
struct ThermoFunData
{
    string name;    ///< Task (file) name
    string comment; ///< Task description
    string calcStatus; ///< ThermoFun colaculation status

    string schemaName;  ///< Selected schema ( "VertexSubstance" or "VertexReaction" )

    // selection data
    /// Selected ThermoDataSet
    string idThermoDataSet;
    /// Selected SourceTDBs
    vector<int> sourceTDBs;
    /// Define ELEMENTS table data
    vector<ThermoFun::ElementKey> elements;

    // old selection data
    jsonio::DBQueryData _query;       ///< Current schema
    string _idReactionSet; ///< Id of reaction set,
                          ///< if not empty we load records only from ReactionSet
    int _sourcetdb;      ///< Current sourcetdb

    // TP data
    string unitsT;          ///< Current units of T
    string unitsP;          ///< Current units of P
    std::vector<std::vector<double>> tppairs;

    // Properties data
    vector<string> properties;    ///< Properties names list
    vector<string> propertyUnits; ///< Units of property
    vector<int>    propertyPrecision; ///< Units of property
    int pPrecision, tPrecision;

    // work data
    map<string,string> mapUnits = ThermoFun::defaultPropertyUnits;
    map<string, int>   mapPrecision = ThermoFun::defaultPropertyDigits;


    /// Default values to task
    ThermoFunData();

    /// Write current task to configuration file fileName
    void toJsonNode( jsonio::JsonDom *object ) const;
    /// Read current task from configuration file fileName
    void fromJsonNode( const jsonio::JsonDom *object );

    ///  Clear properies and query if schema changed
    void resetSchemaName( const string& newSchemaName );

    /// Get string definition of selection
    std::string selection_to_string() const;
};


class ThermoFunPrivateNew: public QObject
{

    Q_OBJECT

    ThermoFunWidgetNew* window;

   // Internal data
   string _curSchemaName = "";
   bool isDefaultQuery = false;

   /// Current project description
   ThermoFunData _data;
   /// Selected substances
   std::shared_ptr<ThermoViewModel>  substModel;
   /// Selected reactions
   std::shared_ptr<ThermoViewModel>  reactModel;

   // Window data  ------------------------------------

   // keys list data
   /// Selected data view
   jsonui::TMatrixTable* valuesTable = 0;

   // define  ELEMENTS table model
   ElementsDataContainer *elementContainer = 0;
   jsonui::TMatrixTable *elementTable = 0;
   jsonui::TMatrixModel* elementModel = 0;

   //ThermoFun data to edit;
   TPContainer* _TPContainer = 0;
   jsonui::TMatrixTable*  _TPlistTable = 0;
   jsonui::TMatrixModel*  _TPlistModel = 0;

   TPropertyContainer* _PropertyContainer = 0;
   jsonui::TMatrixTable*  _PropertyTable = 0;
   jsonui::TMatrixModel*  _PropertyModel = 0;

   // internal functions
   void initWindow();
   void freeInternal();
   void updateElementsModel();
   void updateSelectMessage();
   void linkChange();

   MapSymbolMapLevelReaction recordsMapLevelDefinesReaction_(ThermoFun::MapSubstSymbol_MapLevel_IdReaction mapSymbol_Level_idReact);

public slots:

    void pPChanged(int val)
    {
      _data.pPrecision = val;
    }
    void tPChanged(int val)
    {
      _data.tPrecision = val;
    }
    void TUnitsChanged(const QString& text)
    {
      _data.unitsT = text.toStdString();
    }
    void PUnitsChanged(const QString& text)
    {
      _data.unitsP = text.toStdString();
    }
    void nameChanged(const QString& text)
    {
      _data.name = text.toStdString();
    }
    void commentChanged(const QString& text)
    {
      _data.comment = text.toStdString();
    }

    void retrieveConnectedData(map<string,string> mapOfConnectedIds, vector<string> &linkedSubstSymbols,
     vector<string> &linkedReactSymbols, vector<string> &linkedSubstClasses, vector<string> &linkedSubstIds);


public:

   ThermoFun::DatabaseClient dbclient;

   ThermoFunPrivateNew( ThermoFunWidgetNew* awindow );

   ~ThermoFunPrivateNew()
   {
     freeInternal();
   }

   const ThermoFunData& data() const
   {
     return _data;
   }

   bool isSubstances() const
   {
     return _data.schemaName != "VertexReaction";
   }

   const jsonio::ValuesTable&  getValues( bool isSubst ) const
   {
     if( isSubst )
       return substModel->getValues();
     else
       return reactModel->getValues();
   }

   string getThermoPropertiesName() const
   {
       std::size_t pos = std::string("Vertex").length();
       std::string schemaName = "ThermoProperties" + _data.schemaName.substr (pos);
       return schemaName;
   }

   void newThermoFunData( const ThermoFunData& newdata );

   /// Reset internal data (after select dialog )
   void updateData( const std::string& aThermoDataSet,
                    const std::vector<int>& sourcetdbs,
                    const std::vector<ThermoFun::ElementKey>& elementKeys,
                    const jsonio::ValuesTable&  substanceValues,
                    const jsonio::ValuesTable&  reactionValues );

   void reallocTP( int newsize );
   void updateTP( const string& unitsT, const string& unitsP,
                      const std::vector<std::vector<double>>& tppairs);
   void updateProperty( const vector<string>& properties );

   void typeChanged(const string& newSchemaName);

   // calc functions
   void loadSubstData( const vector<int>& selNdx,
     vector<string>& aKeyList, vector<string>& substancesSymbols,
     vector<string>& substancesClass );
   void loadReactData( const vector<int>& selNdx,
     vector<string>& aKeyList, vector<string>& reactionsSymbols );

   std::map<std::string, int>  getSubstDataIndex()
   {
       return dbclient.substData().getDataName_DataIndex();
   }

   void retrieveConnectedDataSymbols(const vector<string>& substKeys, const vector<string>& reactKeys,
                                     vector<string> &linkedSubstSymbols, vector<string> &linkedReactSymbols,
                                     vector<string> &linkedSubstClasses, vector<string> &linkedSubstIds);

   MapSymbolMapLevelReaction   recordsMapLevelDefinesReaction();
   MapSymbolMapLevelReaction   recordsMapLevelDefinesReaction(vector<string> connectedSubstIds , vector<string> connectedSubstSymbols);

   void setSubstanceLevel(string substSymbol, string level);

   double calcData(const vector<string>& substKeys, const vector<string>& reactKeys,
     const vector<string>& substancesSymbols,  const vector<string>& reactionsSymbols,
     const string& solventSymbol, bool FormatBox, bool calcSubstFromReact, bool calcReactFromSubst , struct timeval start);

   // temporaly functions
   void resetElementsintoRecord( bool isreact, const string& aKey );

};



#endif // THERMOFUNWIDGETPRIVATE_H