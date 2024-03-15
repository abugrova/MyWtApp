#pragma once

#include <Wt/WApplication.h>
#include <Wt/WText.h>
#include <Wt/WTemplate.h>
#include <Wt/Dbo/Dbo.h>
namespace dbo = Wt::Dbo;

// forward declarations of data strcuts
struct Building;
struct Room;
struct BookingRecord;
struct Institute;

class mainApp : public Wt::WApplication
{
public:
    Wt::Dbo::Session session;

    mainApp(const Wt::WEnvironment &env);
    mainApp(const mainApp &other) = delete;

    static mainApp *mainPageInst()
    {
        return (mainApp *)WApplication::instance();
    }

    ~mainApp()
    {
        log("info") << "mainApp is to be destroyed";
    }

    // each function is separate page
    void mainPage();

    dbo::ptr<Institute> choosenInstitute{};
    void instituteChoosen();

    dbo::ptr<Building> choosenBuilding{};
    void buildingChoosen();

    int choosenDay{-1};
    bool evenWeek{};
    void dayIsChoosen();

    Wt::WLineEdit *newRoom;
    Wt::WLineEdit *newTime;
};

struct Building
{
    std::string number;
    std::string address;

    dbo::collection<dbo::ptr<Room>> rooms;

    template <class Action>
    void persist(Action &a)
    {
        dbo::field(a, number, "number");
        dbo::field(a, address, "address");
        dbo::hasMany(a, rooms, dbo::ManyToOne, "building");
    }
};

struct Institute
{
    std::string shortName{};
    std::string longName{};
    dbo::collection<dbo::ptr<Room>> rooms;

    template <class Action>
    void persist(Action &a)
    {
        dbo::field(a, shortName, "shortName");
        dbo::field(a, longName, "longName");
        dbo::hasMany(a, rooms, dbo::ManyToOne, "institute");
    }
};

struct Room
{
    std::string number{};
    dbo::ptr<Institute> institute{};
    dbo::ptr<Building> building{};
    int capacity{};
    bool projector{};
    bool multideck{};
    int personalComputers{};

    dbo::collection<dbo::ptr<BookingRecord>> bookingRecords;

    template <class Action>
    void persist(Action &a)
    {
        dbo::field(a, number, "number");
        dbo::belongsTo(a, institute, "institute");
        dbo::belongsTo(a, building, "building");
        dbo::field(a, capacity, "capacity");
        dbo::field(a, projector, "projector");
        dbo::field(a, multideck, "multideck");
        dbo::field(a, personalComputers, "personalComputers");
        dbo::hasMany(a, bookingRecords, dbo::ManyToOne, "room");
    }
};

struct BookingRecord
{
    int timeSlot{};
    int dayOfWeek{};
    bool evenWeek{};
    std::string lecturer{};
    std::string subject{};
    dbo::ptr<Room> room{};

    template <class Action>
    void persist(Action &a)
    {
        dbo::field(a, timeSlot, "timeSlot");
        dbo::field(a, dayOfWeek, "dayOfWeek");
        dbo::field(a, evenWeek, "evenWeek");
        dbo::field(a, lecturer, "lecturer");
        dbo::field(a, subject, "subject");

        dbo::belongsTo(a, room, "room");
    }
};
