#ifndef CATS2_TWO_COLORS_TAGS_TRACKING_WIDGET_HPP
#define CATS2_TWO_COLORS_TAGS_TRACKING_WIDGET_HPP

#include "TrackerPointerTypes.hpp"

#include <QtWidgets/QWidget>

namespace Ui {
class TwoColorsTagsTrackingWidget;
}

/*!
 * The widget to set parameters of the two colors tags tracking widget.
 */
class TwoColorsTagsTrackingWidget : public QWidget
{
    Q_OBJECT

public:
    //! Constructor.
    explicit TwoColorsTagsTrackingWidget(TrackingRoutinePtr routine,
                                         QWidget *parent = nullptr);
    //! Destructor.
    virtual ~TwoColorsTagsTrackingWidget() final;

protected slots:
    //! Triggered when the settings are modified.
    void updateSettings();

private:
    //! The gui form.
    Ui::TwoColorsTagsTrackingWidget *m_ui;
    //! The tracking routine.
    TrackingRoutinePtr m_routine;
    //! The back tag color.
    QColor m_backColor;
    //! The front tag color.
    QColor m_frontColor;
};

#endif // CATS2_TWO_COLORS_TAGS_TRACKING_WIDGET_HPP
