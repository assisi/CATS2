#ifndef CATS2_MAIN_WINDOW_HPP
#define CATS2_MAIN_WINDOW_HPP

#include <TrackerPointerTypes.hpp>
#include <ViewerPointerTypes.hpp>
#include <SetupType.hpp>

#include <QtCore>
#include <QtGui>
#include <QtWidgets/QMainWindow>

namespace Ui
{
class MainWindow;
}

/*!
 * The main window of the application.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    //! Constructor.
    explicit MainWindow (QWidget *parent = nullptr);
    //! Destructor.
    virtual ~MainWindow() override;

private:
    //! Creates the tracking setup and corresponding viewers of requested type.
    void createSetup(SetupType::Enum setupType);

    //! Sets the primary viewer.
    void setPrimaryView(SetupType::Enum setupType);
    //! Sets the secondary viewer on the left side of the window.
    void setSecondaryView(SetupType::Enum setupType);

    //! Adds the tracking settings to the tab on the left.
    void addTrackingSettingsWidget(SetupType::Enum setupType);
    //! Connects the signals and slots of the primary view.
    void connectPrimaryView();
    //! Disconnects all the signals and slots of the primary view.
    void disconnectPrimaryView();

private:
    //! The form.
    Ui::MainWindow* m_ui;

    //! The primary setup view to be shown in the center of the window.
    SetupType::Enum m_primarySetupType;
    //! The secondary setup view to be shown on the side of the window.
    SetupType::Enum m_secondarySetupType;

    //! The tracking setups.
    QMap<SetupType::Enum, TrackingSetupPtr> m_trackingSetups;
    //! The viewers for the corresponding setups.
    QMap<SetupType::Enum, ViewerHandlerPtr> m_viewerHandlers;

    //! The tracking data manager.
    TrackingDataManagerPtr m_trackingDataManager;
};

#endif // CATS2_MAIN_WINDOW_HPP
