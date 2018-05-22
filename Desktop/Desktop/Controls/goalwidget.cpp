#include "goalwidget.h"
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>

GoalWidget::GoalWidget(const materia::Goal &g, IconManager iconManager) : QLabel(nullptr)
{
    /*if(g.achieved)
    {
        setStyleSheet(QString ("QLabel { background-color: #009900; }"));
    }
    else if(g.focused)
    {
        setStyleSheet(QString ("QLabel { background-color: #FFFF00; }"));
    }
    else
    {
        setStyleSheet(QString ("QLabel { background-color: #666666; }"));
    }*/

    auto op = new QGraphicsOpacityEffect(this);
    op->setOpacity(0.2);
    setGraphicsEffect(op);
    setAutoFillBackground(true);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMinimumSize(75, 75);
    setMaximumSize(75, 75);
    setToolTip(QString("<u>%1</u>").arg(g.name.c_str()));

    auto im = iconManager.get(g.iconId);
    setPixmap(im->scaled(75, 75, Qt::IgnoreAspectRatio, Qt::FastTransformation));
}

void GoalWidget::attachDetails(const std::vector<materia::Task> tasks, const std::vector<materia::Objective> objectives)
{
    setGraphicsEffect(nullptr);
}
