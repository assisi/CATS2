#ifndef CATS2_BLOB_DETECTOR_WIDGET_HPP
#define CATS2_BLOB_DETECTOR_WIDGET_HPP

#include <QtWidgets/QWidget>

namespace Ui {
class BlobDetectorWidget;
}

class BlobDetector;

class BlobDetectorWidget : public QWidget
{
    Q_OBJECT

public:
    //! Constructor.
    explicit BlobDetectorWidget(BlobDetector* routine, QWidget *parent = nullptr);
    //! Destructor.
    virtual ~BlobDetectorWidget() final;

protected slots:
    //! Triggered when the settings are modified.
    void updateSettings();

private:
    //! The gui form.
    Ui::BlobDetectorWidget *m_ui;
    //! The tracking routine.
    BlobDetector* m_routine;
};

#endif // CATS2_BLOB_DETECTOR_WIDGET_HPP
