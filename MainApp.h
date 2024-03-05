#ifndef MAINAPP_H
#define MAINAPP_H

/*
 * This is main application class, in fact - brain of all project
 * his working in every time, listen signals, connect objects and change
 * main page state. So may be in some point we can build state machine in loop method
 */

#include <Wt/WApplication.h>
#include <Wt/WText.h>
#include <Wt/WTemplate.h>
#include <Wt/Dbo/Dbo.h>

class mainApp : public Wt::WApplication
{
public:
    Wt::Dbo::Session session;

    mainApp(const Wt::WEnvironment& env);

    static mainApp* mainPageInst() {
        return (mainApp*)WApplication::instance();
    }

    unsigned int refresh_ctr_ = 0;

    bool                      isFirstLogin = true;
};

#endif // MAINAPP_H
