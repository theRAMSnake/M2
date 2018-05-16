#ifndef STRATEGYVIEW_H
#define STRATEGYVIEW_H

#include <QWidget>
#include <QHBoxLayout>
#include "Models/strategydatamodel.h"

class StrategyView : public QWidget
{
    Q_OBJECT
public:
    explicit StrategyView(QWidget *parent, StrategyDataModel &strategyDataModel);

signals:

public slots:
    void onGoalUpdated(const materia::Goal g);

private:
    QHBoxLayout* mLayout;
};

#endif // STRATEGYVIEW_H
