#include "goalwidget.h"
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QBitmap>
#include <QDialog>
#include <QGuiApplication>
#include <QScreen>
#include <QLineEdit>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QPushButton>

GoalWidget::GoalWidget(const materia::Goal &g, IconManager iconManager) : QLabel(nullptr)
{
    mGoal = g;
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMinimumSize(75, 75);
    setMaximumSize(75, 75);
    setToolTip(QString("<u>%1</u>").arg(g.name.c_str()));

    mIcon = iconManager.get(g.iconId);

    if(g.achieved)
    {
        QPainter painter(mIcon.get());
        auto achIm = iconManager.get({"achieved"});
        painter.drawPixmap(mIcon->width() - achIm->width(), mIcon->height() - achIm->height(), *achIm);
    }
    else if(g.focused)
    {

    }
    else
    {
        auto op = new QGraphicsOpacityEffect(this);
        op->setOpacity(0.2);
        setGraphicsEffect(op);
        setAutoFillBackground(true);
    }

    setPixmap(mIcon->scaled(75, 75, Qt::IgnoreAspectRatio, Qt::FastTransformation));
}

void GoalWidget::attachDetails(const std::vector<materia::Task> tasks, const std::vector<materia::Objective> objectives)
{

}

void GoalWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QDialog dialog (this);
    dialog.setWindowTitle("Goal properties");
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();
    dialog.resize(screenGeometry.width() * 0.9, screenGeometry.height() * 0.9);

    QLabel* iconLabel = new QLabel(&dialog);
    iconLabel->setPixmap(mIcon->scaled(100, 100, Qt::IgnoreAspectRatio, Qt::FastTransformation));
    iconLabel->move(20, 20);

    QLineEdit* title = new QLineEdit(&dialog);
    title->setText(QString(mGoal.name.c_str()));
    title->move(iconLabel->rect().right() + 40, 25 + iconLabel->height() / 2);
    title->resize(dialog.width() - title->pos().x() - 20, title->height() * 2);
    auto f = title->font();
    f.setPointSize(25);
    title->setFont(f);

    QTextEdit* notes = new QTextEdit(&dialog);
    notes->setText(QString(mGoal.notes.c_str()));
    notes->move(20, 140);
    notes->resize(dialog.width() - 40, dialog.height() - 220);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    buttonBox->move(dialog.width() - 20 - buttonBox->width() * 1.5, notes->geometry().bottom() + 20);

    dialog.connect(buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, &dialog, &QDialog::accept);
    dialog.connect(buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, &dialog, &QDialog::reject);
    dialog.connect(&dialog, &QDialog::accepted, [&] () -> void
    {
        mGoal.name = title->text().toStdString();
        mGoal.notes = notes->toPlainText().toStdString();
    });

    dialog.exec();
}
