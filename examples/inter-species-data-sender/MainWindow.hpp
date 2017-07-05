/*
	Class of the applications' main window
*/

#ifndef CATS2_EXAMPLE_MAIN_WINDOW_HPP
#define CATS2_EXAMPLE_MAIN_WINDOW_HPP

#include <InterSpeciesDataManager.hpp>

#include <QtCore>
#include <QtGui>
#include <QtWidgets/QMainWindow>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    //! Constructor.
    explicit MainWindow (QWidget *parent = 0);
    //! Destructor.
    virtual ~MainWindow() override;

private:
    //! The form.
    Ui::MainWindow* m_ui;

    zmq::context_t m_context;
    zmq::socket_t m_publisher;

    //! The timer.
    QTimer m_timer;
};

#endif // CATS2_EXAMPLE_MAIN_WINDOW_HPP
