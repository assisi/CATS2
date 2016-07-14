#ifndef TRACKINGROUTINEWIDGET_HPP
#define TRACKINGROUTINEWIDGET_HPP

#include <QWidget>

namespace Ui {
class TrackingRoutineWidget;
}

class TrackingRoutineWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TrackingRoutineWidget(QWidget *parent = 0);
    ~TrackingRoutineWidget();

private:
    Ui::TrackingRoutineWidget *ui;
};

#endif // TRACKINGROUTINEWIDGET_HPP
