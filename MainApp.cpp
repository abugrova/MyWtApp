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
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>

#include <Wt/Dbo/Dbo.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WColor.h>

#include <string>
#include <thread>

namespace dbo = Wt::Dbo;

mainApp::mainApp(const Wt::WEnvironment &env) : Wt::WApplication(env)
{
    auto sqlite3 = std::make_unique<Wt::Dbo::backend::Sqlite3>(WApplication::appRoot() + "table.db");
    sqlite3->setProperty("show-queries", "true");
    session.setConnection(std::move(sqlite3));

    session.mapClass<Building>("building");
    session.mapClass<Institute>("institute");
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

    mainPage();
}

void mainApp::mainPage()
{
    dbo::Transaction transaction(session);
    auto institutes = session.find<Institute>().resultList();

    auto containerAll = this->root()->addNew<Wt::WContainerWidget>();
    auto container = containerAll->setLayout(std::make_unique<Wt::WVBoxLayout>());
    auto head = container->addWidget(std::make_unique<Wt::WText>());
    head->setText("Выберите интитут");
    head->toggleStyleClass("h2", true);

    for (auto institute : institutes)
    {
        auto btn = container->addWidget(std::make_unique<Wt::WPushButton>());
        btn->setText(institute->shortName);
        btn->clicked().connect([=]()
                               {
            choosenInstitute = institute;
            instituteChoosen(); });
    }
}

void mainApp::instituteChoosen()
{
    auto c = this->root()->children().front();
    this->root()->removeChild(c);

    auto containerAll = this->root()->addNew<Wt::WContainerWidget>();
    auto container = containerAll->setLayout(std::make_unique<Wt::WVBoxLayout>());
    auto head = container->addWidget(std::make_unique<Wt::WText>());
    head->setText("Выберите корпус");
    head->toggleStyleClass("h2", true);

    dbo::Transaction transaction(session);
    auto buildings = session.find<Building>().resultList();

    for (auto building : buildings)
    {
        auto btn = container->addWidget(std::make_unique<Wt::WPushButton>());
        btn->setText(building->number);
        btn->clicked().connect([=]()
                               {
            choosenBuilding = building;
            buildingChoosen(); });
    }
}

void mainApp::buildingChoosen()
{
    auto c = this->root()->children().front();
    this->root()->removeChild(c);

    auto containerAll = this->root()->addNew<Wt::WContainerWidget>();
    auto container = containerAll->setLayout(std::make_unique<Wt::WVBoxLayout>());
    auto head = container->addWidget(std::make_unique<Wt::WText>());
    head->setText("Выберите день недели");
    head->toggleStyleClass("h2", true);
    auto twoColumnsBtns = container->addWidget(std::make_unique<Wt::WContainerWidget>());
    auto twoColumnsBtnsH = twoColumnsBtns->setLayout(std::make_unique<Wt::WHBoxLayout>());
    auto twoColumnsBtns1 = twoColumnsBtnsH->addWidget(std::make_unique<Wt::WContainerWidget>());
    auto twoColumnsBtns1V = twoColumnsBtns1->setLayout(std::make_unique<Wt::WVBoxLayout>());
    auto head1 = twoColumnsBtns1V->addWidget(std::make_unique<Wt::WText>());
    head1->setText("Четная неделя");
    head1->toggleStyleClass("h3", true);
    std::vector<std::string> days{
        "Понедельник",
        "Вторник",
        "Среда",
        "Четверг",
        "Пятница",
        "Суббота",
    };
    for (auto i = 0u; i < days.size(); i++)
    {
        auto btn = twoColumnsBtns1V->addWidget(std::make_unique<Wt::WPushButton>());
        btn->setText(days.at(i));
        btn->clicked().connect([i, this]()
                               {
            choosenDay = i;
            evenWeek = true;
            dayIsChoosen(); });
    }
    auto twoColumnsBtns2 = twoColumnsBtnsH->addWidget(std::make_unique<Wt::WContainerWidget>());
    auto twoColumnsBtns2V = twoColumnsBtns2->setLayout(std::make_unique<Wt::WVBoxLayout>());
    auto head2 = twoColumnsBtns2V->addWidget(std::make_unique<Wt::WText>());
    head2->setText("НеЧетная неделя");
    head2->toggleStyleClass("h3", true);
    for (auto i = 0u; i < days.size(); i++)
    {
        auto btn = twoColumnsBtns2V->addWidget(std::make_unique<Wt::WPushButton>());
        btn->setText(days.at(i));
        btn->clicked().connect([i, this]()
                               {
            choosenDay = i;
            evenWeek = false;
            dayIsChoosen(); });
    }
}

void mainApp::dayIsChoosen()
{
    auto c = this->root()->children().front();
    this->root()->removeChild(c);

    auto containerAll = this->root()->addNew<Wt::WContainerWidget>();
    auto container = containerAll->setLayout(std::make_unique<Wt::WVBoxLayout>());
    auto head = container->addWidget(std::make_unique<Wt::WText>());
    head->setText("Выберите аудиторию и пару");
    head->toggleStyleClass("h2", true);
    auto table = container->addWidget(std::make_unique<Wt::WTable>());
    // make table butiful
    table->addStyleClass("table");
    // table->toggleStyleClass("table-striped", true);
    table->toggleStyleClass("table-bordered", true);
    table->toggleStyleClass("table-hover", true);
    table->toggleStyleClass("table-sm", true);

    table->setHeaderCount(1);
    table->setHeaderCount(1, Wt::Orientation::Vertical);
    table->elementAt(0, 0)->addNew<Wt::WText>("Аудитория/Пара");
    for (int i = 1; i <= 6; i++)
    {
        table->elementAt(0, i)->addNew<Wt::WText>(std::to_string(i) + " Пара");
    }

    dbo::Transaction t(session);
    using Rooms = dbo::collection<dbo::ptr<Room>>;
    Rooms rooms = session.find<Room>()
                      .where("institute_id = ?")
                      .bind(choosenInstitute)
                      .where("building_id = ?")
                      .bind(choosenBuilding);
    int row = 0;
    for (const dbo::ptr<Room> &room : rooms)
    {
        row++;
        table->elementAt(row, 0)->addNew<Wt::WText>(room->number);
        using BookingRecords = dbo::collection<dbo::ptr<BookingRecord>>;
        BookingRecords bookings = session.find<BookingRecord>()
                                      .where("room_id = ?")
                                      .bind(room)
                                      .where("dayOfWeek = ?")
                                      .bind(choosenDay)
                                      .where("evenWeek = ?")
                                      .bind(evenWeek);
        for (auto &booking : bookings)
        {
            auto cell = table->elementAt(row, booking->timeSlot);
            cell->addNew<Wt::WText>("Занято");
            cell->toggleStyleClass("table-secondary", true);
        }
    }

    auto containerAddRoom = container->addWidget(std::make_unique<Wt::WContainerWidget>());
    auto addRoomLine = containerAddRoom->setLayout(std::make_unique<Wt::WHBoxLayout>());
    {
        auto head = addRoomLine->addWidget(std::make_unique<Wt::WText>());
        head->setText("Введите номер аудитории");
        head->setWidth(Wt::WLength("10%"));
        auto input = addRoomLine->addWidget(std::make_unique<Wt::WLineEdit>());
        input->setWidth(Wt::WLength("30%"));
        newRoom = input;
    }

    auto containerAddTimeSlot = container->addWidget(std::make_unique<Wt::WContainerWidget>());
    auto addTimeLine = containerAddTimeSlot->setLayout(std::make_unique<Wt::WHBoxLayout>());
    {
        auto head = addTimeLine->addWidget(std::make_unique<Wt::WText>());
        head->setText("Введите номер пары");
        head->setWidth(Wt::WLength("10%"));
        auto input = addTimeLine->addWidget(std::make_unique<Wt::WLineEdit>());
        input->setWidth(Wt::WLength("30%"));
        newTime = input;
    }

    auto addButton = container->addWidget(std::make_unique<Wt::WPushButton>());
    addButton->setText("Добавить запись");
    addButton->clicked().connect([=]()
                                 {
        addButton->setText("Добавляем...");
        using namespace std::chrono;
        std::this_thread::sleep_for(500ms);

        dbo::Transaction add(session);
        auto requiredRoom = session.find<Room>()
                      .where("institute_id = ?")
                      .bind(choosenInstitute)
                      .where("building_id = ?")
                      .bind(choosenBuilding)
                      .where("number = ?").bind(newRoom->text().toUTF8());
        auto sizeOfResult = requiredRoom.resultList().size();
        if (sizeOfResult != 1)
        {
            Wt::StandardButton answer = Wt::WMessageBox::show("Ошибка",
                                                            "<p>Нет такой аудитории!</p>",
                                                            Wt::StandardButton::Ok);
            // to remove unused var warning
            (void)answer;
            addButton->setText("Добавить запись");
            return;
        }

        auto booking = std::make_unique<BookingRecord>();
        booking->room = requiredRoom.resultList().front();
        booking->timeSlot= std::atoi(newTime->text().toUTF8().c_str());
        if(booking->timeSlot>6 || booking->timeSlot<1){
            Wt::StandardButton answer = Wt::WMessageBox::show("Ошибка",
                                                            "<p>Неверный номер пары!</p>",
                                                            Wt::StandardButton::Ok);
            // to remove unused var warning
            (void)answer;
            log("error") << "invalid timeslot conversion, input was:" << newTime->text().toUTF8();
            addButton->setText("Добавить запись");
            return;
        }
        booking->dayOfWeek = choosenDay;
        booking->evenWeek = evenWeek;
        session.add(std::move(booking)); 
        add.commit();
        auto messageBox =
            container->addChild(std::make_unique<Wt::WMessageBox>(
                    "Запись успешно добавлена!",
                    "<p>Продолжить?</p>",
                    Wt::Icon::Information,
                    Wt::StandardButton::Yes | Wt::StandardButton::No));

        messageBox->setModal(true);

        messageBox->buttonClicked().connect([=] {
            if (messageBox->buttonResult() == Wt::StandardButton::Yes){
                dayIsChoosen();
            }
            else{
                container->removeChild(messageBox);
            }
        });
        messageBox->show(); });
}
