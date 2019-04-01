#pragma once
#include <boost/signals2/signal.hpp>
#include <Wt/WDialog.h>

class Dialog
{
public:
    boost::signals2::signal<void()> OnFinished;

    Dialog(const std::string& name, std::unique_ptr<Wt::WWidget>&& primaryWidget);

    void show();
    void stop();

private:
    Wt::WDialog mImpl;
};