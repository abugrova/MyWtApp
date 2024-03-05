#include <Wt/Dbo/backend/Sqlite3.h>

#include "MainApp.h"

#include <Wt/WContainerWidget.h>
#include <Wt/WSelectionBox.h>
#include <Wt/WText.h>
mainApp::mainApp(const Wt::WEnvironment& env) : Wt::WApplication(env) {
    auto sqlite3 = std::make_unique<Wt::Dbo::backend::Sqlite3>(WApplication::appRoot() + "example.db");
    sqlite3->setProperty("show-queries", "true");
    session.setConnection(std::move(sqlite3));


    Wt::Dbo::Transaction transaction(session);
    try {
        session.createTables();
        log("info") << "Database created";
    } catch (...) {
        log("info") << "Using existing database";
    }

    transaction.commit();
        
    auto container = this->root()->addWidget(std::make_unique<Wt::WContainerWidget>());

    Wt::WSelectionBox *sb2 = container->addNew<Wt::WSelectionBox>();
    sb2->addItem("Bacon");
    sb2->addItem("Cheese");
    sb2->addItem("Mushrooms");
    sb2->addItem("Green peppers");
    sb2->addItem("Ham");
    sb2->addItem("Pepperoni");
    sb2->addItem("Red peppers");
    sb2->addItem("Turkey");
    sb2->setHeight(500);
    sb2->setSelectionMode(Wt::SelectionMode::Extended);
    std::set<int> selection;    // By default select the items with index 1 and 4.
    selection.insert(1);        // Index 1 corresponds to the 2nd item.
    selection.insert(4);        // Index 4 corresponds to the 5th item.
    sb2->setSelectedIndexes(selection);
    sb2->setMargin(10, Wt::Side::Right);

    Wt::WText *out = container->addNew<Wt::WText>();
    out->addStyleClass("help-block");

    sb2->activated().connect([=] {
        Wt::WString selected;

        int i=0;
        std::set<int> newSelection = sb2->selectedIndexes();
        for (std::set<int>::iterator it = newSelection.begin();
            it != newSelection.end(); ++it) {
            if (!selected.empty())
                selected += ", ";

            selected += sb2->itemText(*it);
            i++;
        }
         sb2->setHeight(200 + 50*i);
        out->setText(Wt::WString("You choose {1}.").arg(selected));
    });
}

