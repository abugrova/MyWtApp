#include <Wt/Dbo/backend/Sqlite3.h>

#include "MainApp.h"

#include <Wt/WContainerWidget.h>
#include <Wt/WTable.h>
#include <Wt/WTableCell.h>
#include <Wt/WLineEdit.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WCheckBox.h>
#include <Wt/WTabWidget.h>
#include <Wt/WMessageBox.h>

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
            log("error") << e.what();
        }

        transaction.commit();
    }
    dbo::Transaction transaction(session);
    // create tab widget
    Wt::WTabWidget *tabW = this->root()->addNew<Wt::WTabWidget>();
    tabW->setStyleClass("tabwidget");

    tabW->addTab(std::move(createRoomTable()), "Rooms table", Wt::ContentLoading::Eager);
    tabW->addTab(std::move(createBookingTable()), "Booking table", Wt::ContentLoading::Eager);
}
std::unique_ptr<Wt::WContainerWidget> mainApp::createRoomTable()
{
    auto containerForRoomsTable = std::make_unique<Wt::WContainerWidget>();
    roomsTable = (containerForRoomsTable->addWidget(std::make_unique<Wt::WTable>()));

    roomsTable->setHeaderCount(1);
    roomsTable->setWidth(Wt::WLength("80%"));
    roomsTable->elementAt(0, 0)->addNew<Wt::WText>("#");
    roomsTable->elementAt(0, 1)->addNew<Wt::WText>("Number");
    roomsTable->elementAt(0, 2)->addNew<Wt::WText>("Building");
    roomsTable->elementAt(0, 3)->addNew<Wt::WText>("Capacity");
    roomsTable->elementAt(0, 4)->addNew<Wt::WText>("Projector");
    roomsTable->elementAt(0, 5)->addNew<Wt::WText>("MultiDeck");
    roomsTable->elementAt(0, 6)->addNew<Wt::WText>("PCs");

    {
        using Rooms = dbo::collection<dbo::ptr<Room>>;
        Rooms rooms = session.find<Room>();
        int row = 0;
        for (const dbo::ptr<Room> &room : rooms)
        {
            row++;
            roomsTable->elementAt(row, 0)->addNew<Wt::WText>(Wt::WString("{1}").arg(row));
            roomsTable->elementAt(row, 1)->addNew<Wt::WText>(Wt::WString("{1}").arg(room->number));
            roomsTable->elementAt(row, 2)->addNew<Wt::WText>(Wt::WString("{1}").arg(room->building));
            roomsTable->elementAt(row, 3)->addNew<Wt::WText>(Wt::WString("{1}").arg(room->capacity));
            roomsTable->elementAt(row, 4)->addNew<Wt::WText>(Wt::WString("{1}").arg(room->projector ? "Yes" : "No"));
            roomsTable->elementAt(row, 5)->addNew<Wt::WText>(Wt::WString("{1}").arg(room->multideck ? "Yes" : "No"));
            roomsTable->elementAt(row, 6)->addNew<Wt::WText>(Wt::WString("{1}").arg(room->personalComputers));
        }
        row++;
        roomsTable->elementAt(row, 0)
            ->addNew<Wt::WText>(Wt::WString("{1}").arg(row));
        newNumber = (roomsTable->elementAt(row, 1)->addNew<Wt::WLineEdit>());

        newBuilding = (roomsTable->elementAt(row, 2)->addNew<Wt::WLineEdit>());
        newCapacity = (roomsTable->elementAt(row, 3)->addNew<Wt::WLineEdit>());
        newProjector = (roomsTable->elementAt(row, 4)->addNew<Wt::WCheckBox>());
        newMultiDeck = (roomsTable->elementAt(row, 5)->addNew<Wt::WCheckBox>());
        newPCs = (roomsTable->elementAt(row, 6)->addNew<Wt::WLineEdit>());
    }
    saveNewRoomButton = (containerForRoomsTable->addWidget(std::make_unique<Wt::WPushButton>("Save")));
    saveNewRoomButton->clicked().connect(
        [t = this]()
        {
            t->addNewRoomToDb();
            t->saveNewRoomButton->setText("Saved");
        });
    roomsTable->addStyleClass("table");
    roomsTable->toggleStyleClass("table-striped", true);
    roomsTable->toggleStyleClass("table-bordered", true);
    roomsTable->toggleStyleClass("table-hover", true);
    roomsTable->toggleStyleClass("table-sm", true);
    return containerForRoomsTable;
}

std::unique_ptr<Wt::WContainerWidget> mainApp::createBookingTable()
{
    auto containerForBookingTable = std::make_unique<Wt::WContainerWidget>();
    bookingTable = (containerForBookingTable->addWidget(std::make_unique<Wt::WTable>()));

    bookingTable->setHeaderCount(1);
    bookingTable->setWidth(Wt::WLength("80%"));

    bookingTable->elementAt(0, 0)->addNew<Wt::WText>("#");
    bookingTable->elementAt(0, 1)->addNew<Wt::WText>("Room");
    bookingTable->elementAt(0, 2)->addNew<Wt::WText>("Start time");
    bookingTable->elementAt(0, 3)->addNew<Wt::WText>("End time");
    bookingTable->elementAt(0, 4)->addNew<Wt::WText>("Subject");
    bookingTable->elementAt(0, 5)->addNew<Wt::WText>("Lecturer");

    {
        using Booking = dbo::collection<dbo::ptr<BookingRecord>>;
        Booking bookings = session.find<BookingRecord>();
        int row = 0;
        for (const dbo::ptr<BookingRecord> &booking : bookings)
        {
            row++;
            bookingTable->elementAt(row, 0)->addNew<Wt::WText>(Wt::WString("{1}").arg(row));
            bookingTable->elementAt(row, 1)->addNew<Wt::WText>(Wt::WString("{1}").arg(booking->room->number));
            bookingTable->elementAt(row, 2)->addNew<Wt::WText>(Wt::WString("{1}").arg(booking->startTime));
            bookingTable->elementAt(row, 3)->addNew<Wt::WText>(Wt::WString("{1}").arg(booking->endTime));
            bookingTable->elementAt(row, 4)->addNew<Wt::WText>(Wt::WString("{1}").arg(booking->subject));
            bookingTable->elementAt(row, 5)->addNew<Wt::WText>(Wt::WString("{1}").arg(booking->lecturer));
        }
        row++;
        bookingTable->elementAt(row, 0)->addNew<Wt::WText>(Wt::WString("{1}").arg(row));
        newRoom = (bookingTable->elementAt(row, 1)->addNew<Wt::WLineEdit>());
        newStartTime = (bookingTable->elementAt(row, 2)->addNew<Wt::WLineEdit>());
        newEndTime = (bookingTable->elementAt(row, 3)->addNew<Wt::WLineEdit>());
        newSubject = (bookingTable->elementAt(row, 4)->addNew<Wt::WLineEdit>());
        newLecturer = (bookingTable->elementAt(row, 5)->addNew<Wt::WLineEdit>());
    }
    saveNewBookingButton = (containerForBookingTable->addWidget(std::make_unique<Wt::WPushButton>("Save")));
    saveNewBookingButton->clicked().connect(
        [t = this]()
        {
            bool retFlag;
            t->addNewBookingToDb(retFlag);
            if (retFlag)
                return;
            t->saveNewBookingButton->setText("Saved");
        });
    bookingTable->addStyleClass("table");
    bookingTable->toggleStyleClass("table-striped", true);
    bookingTable->toggleStyleClass("table-bordered", true);
    bookingTable->toggleStyleClass("table-hover", true);
    bookingTable->toggleStyleClass("table-sm", true);
    return containerForBookingTable;
}

void mainApp::addNewBookingToDb(bool &retFlag)
{
    retFlag = true;
    dbo::Transaction transaction(session);
    auto requiredRoom = session.find<Room>().where("number = ?").bind(newRoom->text().toUTF8());
    auto sizeOfResult = requiredRoom.resultList().size();
    if (sizeOfResult != 1)
    {
        Wt::StandardButton answer = Wt::WMessageBox::show("Error",
                                                          "<p>Wrong data for room number!</p>",
                                                          Wt::StandardButton::Ok);
        // to remove unused var warning
        (void)answer;
        return;
    }
    auto booking = std::make_unique<BookingRecord>();
    booking->room = requiredRoom;
    booking->startTime = std::atoi(newStartTime->text().toUTF8().c_str());
    booking->endTime = std::atoi(newEndTime->text().toUTF8().c_str());
    booking->subject = newSubject->text().toUTF8();
    booking->lecturer = newLecturer->text().toUTF8();
    session.add(std::move(booking));
    retFlag = false;
}

void mainApp::addNewRoomToDb()
{
    dbo::Transaction addTransaction(session);
    auto room = std::make_unique<Room>();
    room->number = newNumber->text().toUTF8();
    room->building = newBuilding->text().toUTF8();
    room->capacity = std::atoi(newCapacity->text().toUTF8().c_str());
    room->projector = newProjector->isChecked();
    room->multideck = newMultiDeck->isChecked();
    room->personalComputers = std::atoi(newPCs->text().toUTF8().c_str());
    session.add(std::move(room));
}
