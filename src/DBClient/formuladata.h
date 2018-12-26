#ifndef _FORMULADATA_H
#define _FORMULADATA_H

#include <iostream>
#include <map>
#include <set>
#include "formulaparser.h"
#include "jsonio/dbvertexdoc.h"

namespace ThermoFun {

/// Key fields of Element vertex
struct ElementKey
{
  string symbol;
  int class_;
  int isotope;

  ElementKey( const string& asymbol, int aclass = 0 /*ELEMENT*/, int aisotope = 0 ):
   symbol(asymbol), class_(aclass), isotope(aisotope)
  { }

  ElementKey( const string& asymbol, const string& typeline ):
      symbol(asymbol)
  { classIsotopeFrom(typeline); }

  void classIsotopeFrom(const string& line );
  string key() const;
  string getSymbol() const;

  void toJsonNode_( jsonio::JsonDom *object ) const;
  void fromJsonNode_( const jsonio::JsonDom *object );
  void fromJsonNode( const jsonio::JsonDom *object );
  //void toBson( bson *obj ) const;
  //void fromBson( const char* obj );

};

bool operator <( const ElementKey&,  const ElementKey& );
bool operator >( const ElementKey&,  const ElementKey& );
bool operator==( const ElementKey&,  const ElementKey& );
bool operator!=( const ElementKey&,  const ElementKey& );

/// Elements values, loaded from Database
struct ElementValues
{
  string recid;            // Record id
  double atomic_mass;   // "Atomic (molar) mass, g/atom (g/mole)"
  double entropy;   // "Atomic entropy S0 at standard state, J/mole/K"
  double heat_capacity;   // "Atomic heat capacity Cp0 at standard state, J/mole/K"
  double volume;   // "Atomic volume V0 at standard state, cm3/mole"
  int valence;   // "Default valence number (charge) in compounds"
  int number;     // "Index in Periodical (Mendeleev's) table"
  std::string name;
  // ...
};

///     Loading from Database a map of element symbol,
/// valence, entropy and other
using DBElementsData = map<ElementKey, ElementValues >;

/// Values extracted from chemical formulae
struct FormulaValues
{
    ElementKey key;
    short   valence;
    double  stoichCoef;

    FormulaValues( const ElementKey& akey, double  astoichCoef, short  avalence ):
      key(akey), valence(avalence), stoichCoef(astoichCoef)
    { }
};

/// Values calculated from formula
/// vectors of molar masses, entropies and charges of substances.
struct FormulaProperites
{
    string formula;
    double charge;
    double atomic_mass;
    double elemental_entropy;
    double atoms_formula_unit;
};

/// Internal parsed data
using FormulaElementsData = vector<FormulaValues>;

/// Description of disassembled formula token
class FormulaToken
{
    string formula;
    FormulaElementsData datamap;  ///< List of tokens
    map<ElementKey, double>  elements_map;
    set<ElementKey>  elements;    ///< Set of existed elements
//    std::map<std::string, double> mapElementsCoeff; ///< Map of elements symbols and coefficients
    double aZ;                    ///< Calculated charge in Mol

protected:

    void clear();
    void unpack( list<ICTERM>& itt_ );

public:

    FormulaToken( const string& aformula ):aZ(0)
    {
      setFormula(aformula);
    }

    ~FormulaToken(){}

    //--- Selectors

    void setFormula( const string& aformula );
    const string& getFormula() const
    {
      return formula;
    }

    double calculateCharge();
    double charge() const
    {
        return aZ;
    }

    //--- Value manipulation

    /// Return true if all elements from formula present into system
    bool checkElements( const string& aformula );
    /// Throw exeption if in formula have element not present into system
    void exeptionCheckElements( const string& subreacKey, const string& aformula );
    /// Build list of elements not present into system
    string testElements( const string& aformula );

    /// Calculate charge, molar mass, elemental entropy, atoms per formula unit
    /// from chemical formulae
    void calcFormulaProperites( FormulaProperites& propert );

    /// Get a row of stoichiometry matrix from the unpacked formula,
    /// sysElemens - list of element keys
    vector<double> makeStoichiometryRowOld(const vector<ElementKey>& sysElemens );
//    Eigen::VectorXd makeStoichiometryRow(const vector<ElementKey>& sysElemens );

   /// Throw exeption if charge imbalance
    void exeptionCargeImbalance();

    /// Get of existed elements
    const map<ElementKey, double>& getElements_map() const
    { return elements_map; }
    const set<ElementKey>& getElements() const
    { return elements; }

//    const std::vector<double>& getCoefficients() const
//    { return coefficients; }

};

vector<ElementKey> getDBElements( jsonio::TDBVertexDocument* elementDB, const vector<string>& idList );
string ElementsKeysToJson( const set<ElementKey>& elements );
/// Work only with internal elements list (wihout map)
bool ElementsKeysFromJson( const string elmsjson, set<ElementKey>& elements );
/// Read elements only from special fields
bool ElementsKeysFromJsonDomArray( const string& keypath, const jsonio::JsonDom *object, set<ElementKey>& elements );

struct Element;

class ChemicalFormula
{

  /// Loading from database elements
  static  DBElementsData dbElements;
  static  vector<string> queryFields;

  static void addOneElement( jsonio::TDBVertexDocument* elementDB );
  static void addOneElement(Element element);

 public:

  static ElementValues undefinedProperites;

  static const DBElementsData& getDBElements()
  {
     return dbElements;
  }

  static jsonio::DBQueryData getDefaultQuery()
  {
    return jsonio::DBQueryData("{\"_label\": \"element\" }",jsonio::DBQueryData::qTemplate );
  }

  static void setDBElements( jsonio::TDBVertexDocument* elementDB,
                             const jsonio::DBQueryData& query =  ChemicalFormula::getDefaultQuery() );
  static void setDBElements( jsonio::TDBVertexDocument* elementDB, const vector<string>& keyList );
  static void setDBElements(std::map<std::string, Element> elements );

  static vector<ElementKey> elementsRow();

  static map<ElementKey, double> extractElements_map(  const vector<string>& formulalist );
  static set<ElementKey>         extractElements(  const vector<string>& formulalist );
  static FormulaProperites         calcThermo(  const string formula_ );
  static vector<FormulaProperites> calcThermo(  const vector<string>& formulalist );
  static vector<vector<double>> calcStoichiometryMatrixOld(  const vector<string>& formulalist );
//  static Eigen::MatrixXd calcStoichiometryMatrix(  const vector<string>& formulalist );

  static const ElementValues& elementProperites( const ElementKey& elementKey )
  {
    auto itr = dbElements.find(elementKey);
    if( itr == dbElements.end() )
      return undefinedProperites;
    else
      return itr->second;
  }

};

auto elementKeyToElement(ElementKey elementKey) -> Element;

}

#endif // _FORMULADATA_H
