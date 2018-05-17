//#include "BSONUITestMainWindow.h"

#include "jsonio/io_settings.h"
#include "jsonui/JSONUIBase.h"
#include "ThermoFunWidgetNew.h"
#include "jsonio/nservice.h"
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>

using namespace jsonui;
using namespace jsonio;

int main(int argc, char *argv[])
{

    jsonio::BsonioSettings::settingsFileName = "ThermoFunDemoGUI.json";
    QApplication a(argc, argv);

    std::list<jsonui::JSONUIBase*> bsonuiWindows;

    jsonui::onEventfunction onCloseEvent;
    jsonui::ShowWidgetFunction showWidget;
//    TBSONUITestWin w;
//    w.show();


    try{
          JSONUIBase* testWidget;
          testWidget = new ThermoFunWidgetNew( /*this*/ );

          testWidget->setOnCloseEventFunction(onCloseEvent);
          testWidget->setShowWidgetFunction(showWidget);
          bsonuiWindows.push_back(testWidget);
          testWidget->show();

      }
     catch(jsonio_exeption& e)
    {}
//     {
//         QMessageBox::critical( a, e.title(), e.what() );
//     }
//     catch(std::exception& e)
//      {
//         QMessageBox::critical( a, "std::exception", e.what() );
//      }
    return a.exec();
}
