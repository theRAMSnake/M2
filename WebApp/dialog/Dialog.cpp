#include "Dialog.hpp"
#include <Wt/WPushButton.h>

Dialog::Dialog(const std::string& name, std::unique_ptr<Wt::WWidget>&& primaryWidget)
: mImpl(name)
{
    mImpl.contents()->addWidget(std::move(primaryWidget));
    mImpl.contents()->setOverflow(Wt::Overflow::Auto);

    Wt::WPushButton *ok = new Wt::WPushButton("OK");
    mImpl.footer()->addWidget(std::unique_ptr<Wt::WPushButton>(ok));
    ok->setDefault(true);
    ok->setStyleClass("btn-primary");

    ok->clicked().connect(std::bind([=]() {
        mImpl.accept();
    }));

    mImpl.finished().connect(std::bind([=]() {
        OnFinished();
        delete this;
    }));

    mImpl.setWidth(Wt::WLength("95%"));
    mImpl.setHeight(Wt::WLength("85%"));
}

void Dialog::show()
{
    mImpl.show();
}

void Dialog::stop()
{
    OnFinished();
    delete this;
}