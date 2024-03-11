#ifndef MAINAPP_H
#define MAINAPP_H

#include <Wt/WApplication.h>
#include <Wt/WText.h>
#include <Wt/WTemplate.h>
#include <Wt/Dbo/Dbo.h>

class mainApp : public Wt::WApplication
{
public:
    Wt::Dbo::Session session;

    mainApp(const Wt::WEnvironment &env);
    mainApp(const mainApp &other) = delete;

    std::unique_ptr<Wt::WContainerWidget> createBookingTable();
    std::unique_ptr<Wt::WContainerWidget> createRoomTable();
    std::unique_ptr<Wt::WContainerWidget> createBooker();

    void addNewBookingToDb(bool &retFlag);
    void addNewRoomToDb();

    static mainApp *mainPageInst()
    {
        return (mainApp *)WApplication::instance();
    }

    // wtable for rooms table
    Wt::WTable *roomsTable{};

    Wt::WLineEdit *newNumber{};
    Wt::WLineEdit *newBuilding{};
    Wt::WLineEdit *newCapacity{};
    Wt::WCheckBox *newProjector{};
    Wt::WCheckBox *newMultiDeck{};
    Wt::WLineEdit *newPCs{};

    Wt::WPushButton *saveNewRoomButton{};

    Wt::WTable *bookingTable{};
    // unix epoch format?
    Wt::WLineEdit *newStartTime{};
    Wt::WLineEdit *newEndTime{};

    Wt::WLineEdit *newLecturer{};
    Wt::WLineEdit *newSubject{};
    Wt::WLineEdit *newRoom{};
    Wt::WPushButton *saveNewBookingButton{};

    ~mainApp()
    {
        log("info") << "start desctruction";
        // saveNewRoomButton.reset();
        log("info") << "end desctruction";
    }
    void refresh()
    {
        this->refresh();
    }
};

namespace dbo = Wt::Dbo;

struct BookingRecord;

struct Room
{
    std::string number;
    std::string building;
    int capacity;
    bool projector;
    bool multideck;
    int personalComputers;

    dbo::collection<dbo::ptr<BookingRecord>> bookingRecords;

    template <class Action>
    void persist(Action &a)
    {
        dbo::field(a, number, "number");
        dbo::field(a, building, "building");
        dbo::field(a, capacity, "capacity");
        dbo::field(a, projector, "projector");
        dbo::field(a, multideck, "multideck");
        dbo::field(a, personalComputers, "personalComputers");
        dbo::hasMany(a, bookingRecords, dbo::ManyToOne, "room");
    }
};

struct BookingRecord
{
    int64_t startTime;
    int64_t endTime;
    std::string lecturer;
    std::string subject;
    dbo::ptr<Room> room;

    template <class Action>
    void persist(Action &a)
    {
        dbo::field(a, startTime, "startTime");
        dbo::field(a, endTime, "endTime");
        dbo::field(a, lecturer, "lecturer");
        dbo::field(a, subject, "subject");

        dbo::belongsTo(a, room, "room");
    }
};

#endif // MAINAPP_H
