#ifndef GOALWIDGET_H
#define GOALWIDGET_H

#include <QLabel>
#include <MateriaEmu/materiaclient.h>
#include "iconmanager.h"

class GoalWidget : public QLabel
{
    Q_OBJECT
public:
    explicit GoalWidget(const materia::Goal& g, IconManager iconManager);

    void attachDetails(const std::vector<materia::Task> tasks, const std::vector<materia::Objective> objectives);

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;

signals:

public slots:

private:
    materia::Goal mGoal;
    std::shared_ptr<QPixmap> mIcon;
};

#endif // GOALWIDGET_H
