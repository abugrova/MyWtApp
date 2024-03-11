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

    static mainApp *mainPageInst()
    {
        return (mainApp *)WApplication::instance();
    }

    // wtable for rooms table
    std::shared_ptr<Wt::WTable> roomsTable{};

    std::shared_ptr<Wt::WLineEdit> newNumber{};
    std::shared_ptr<Wt::WLineEdit> newBuilding{};
    std::shared_ptr<Wt::WLineEdit> newCapacity{};
    std::shared_ptr<Wt::WCheckBox> newProjector{};
    std::shared_ptr<Wt::WCheckBox> newMultiDeck{};
    std::shared_ptr<Wt::WLineEdit> newPCs{};

    std::shared_ptr<Wt::WPushButton> saveNewRoomButton{};


    std::shared_ptr<Wt::WTable> bookingTable{};
    std::shared_ptr<Wt::WLineEdit> newStartTime{};
    std::shared_ptr<Wt::WLineEdit> newEndTime{};
    std::shared_ptr<Wt::WLineEdit> newLecturer{};
    std::shared_ptr<Wt::WLineEdit> newSubject{};
    std::shared_ptr<Wt::WLineEdit> newRoom{};
    std::shared_ptr<Wt::WPushButton> saveNewBookingButton{};


    ~mainApp(){
         log("info")  <<"start desctruction";
         //saveNewRoomButton.reset();
         log("info")  <<"end desctruction";
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
