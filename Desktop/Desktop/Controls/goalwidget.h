#ifndef GOALWIDGET_H
#define GOALWIDGET_H

#include <QWidget>
#include <MateriaEmu/materiaclient.h>

class GoalWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GoalWidget(QWidget *parent, const materia::Goal& g);

signals:

public slots:
};

#endif // GOALWIDGET_H
