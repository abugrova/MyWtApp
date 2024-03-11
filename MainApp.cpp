#include <Wt/Dbo/backend/Sqlite3.h>

#include "MainApp.h"

#include <Wt/WContainerWidget.h>
#include <Wt/WTable.h>
#include <Wt/WTableCell.h>
#include <Wt/WLineEdit.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WCheckBox.h>

#include <Wt/Dbo/Dbo.h>
#include <string>

namespace dbo = Wt::Dbo;

mainApp::mainApp(const Wt::WEnvironment &env) : Wt::WApplication(env)
{
    auto sqlite3 = std::make_unique<Wt::Dbo::backend::Sqlite3>(WApplication::appRoot() + "table.db");
    sqlite3->setProperty("show-queries", "true");
    session.setConnection(std::move(sqlite3));

    session.mapClass<Room>("room");
    session.mapClass<BookingRecord>("bookingRecord");
    {
        Wt::Dbo::Transaction transaction(session);
        try
        {
            session.createTables();
            log("info") << "Database created";
        }
        catch (dbo::Exception &e)
        {
            log("!!! failed: ") << e.what();
        }

        transaction.commit();
    }

    auto container = this->root()->addWidget(std::make_unique<Wt::WContainerWidget>());

    table.reset(container->addWidget(std::make_unique<Wt::WTable>()));
    
    table->setHeaderCount(1);
    table->setWidth(Wt::WLength("100%"));

    table->elementAt(0, 0)->addNew<Wt::WText>("#");
    table->elementAt(0, 1)->addNew<Wt::WText>("Number");
    table->elementAt(0, 2)->addNew<Wt::WText>("Building");
    table->elementAt(0, 3)->addNew<Wt::WText>("Capacity");
    table->elementAt(0, 4)->addNew<Wt::WText>("Projector");
    table->elementAt(0, 5)->addNew<Wt::WText>("MultiDeck");
    table->elementAt(0, 6)->addNew<Wt::WText>("PCs");

    using Rooms = dbo::collection<dbo::ptr<Room>>;
    dbo::Transaction transaction(session);
    Rooms rooms = session.find<Room>();
    int row = 0;
    for (const dbo::ptr<Room> &room : rooms)
    {
        row++;
        table->elementAt(row, 0)
            ->addNew<Wt::WText>(Wt::WString("{1}").arg(row));
        table->elementAt(row, 1)
            ->addNew<Wt::WText>(Wt::WString("{1}").arg(room->number));
        table->elementAt(row, 2)
            ->addNew<Wt::WText>(Wt::WString("{1}").arg(room->building));
        table->elementAt(row, 3)
            ->addNew<Wt::WText>(Wt::WString("{1}").arg(room->capacity));
        table->elementAt(row, 4)
            ->addNew<Wt::WText>(Wt::WString("{1}").arg(room->projector?"Yes":"No"));
        table->elementAt(row, 5)
            ->addNew<Wt::WText>(Wt::WString("{1}").arg(room->multideck?"Yes":"No"));
        table->elementAt(row, 6)
            ->addNew<Wt::WText>(Wt::WString("{1}").arg(room->personalComputers));
    }
    row++;
    table->elementAt(row, 0)
        ->addNew<Wt::WText>(Wt::WString("{1}").arg(row));
    newNumber.reset(table->elementAt(row, 1)->addNew<Wt::WLineEdit>());

    newBuilding.reset(table->elementAt(row, 2)
                           ->addNew<Wt::WLineEdit>());
    newCapacity.reset(table->elementAt(row, 3)
                           ->addNew<Wt::WLineEdit>());
    newProjector.reset(table->elementAt(row, 4)
                            ->addNew<Wt::WCheckBox>());
    newMultiDeck.reset(table->elementAt(row, 5)
                            ->addNew<Wt::WCheckBox>());
    newPCs.reset(table->elementAt(row, 6)
                      ->addNew<Wt::WLineEdit>());

    auto saveButton = container->addWidget(std::make_unique<Wt::WPushButton>("Save"));
    saveButton->clicked().connect(
        [=]()
        {
            dbo::Transaction transaction(session);

            auto room = std::make_unique<Room>();
            room->number = newNumber->text().toUTF8();
            room->building = newBuilding->text().toUTF8();
            room->capacity = std::atoi(newCapacity->text().toUTF8().c_str());
            room->projector = newProjector->isChecked();
            room->multideck = newMultiDeck->isChecked();
            room->personalComputers = std::atoi(newPCs->text().toUTF8().c_str());
            session.add(std::move(room));
            saveButton->setText("Saved");
        });
    table->addStyleClass("table");
    table->toggleStyleClass("table-striped",true);
    table->toggleStyleClass("table-bordered",true);
    table->toggleStyleClass("table-hover",true);
    table->toggleStyleClass("table-sm",true);
}
