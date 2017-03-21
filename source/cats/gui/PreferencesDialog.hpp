#ifndef CATS2_PREFERENCES_DIALOG_HPP
#define CATS2_PREFERENCES_DIALOG_HPP

#include <QDialog>

namespace Ui {
class PreferencesDialog;
}

/*!
 * The application preferences.
 */
class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    //! Constructor.
    explicit PreferencesDialog(QWidget *parent = nullptr);
    //! Destructor.
    virtual ~PreferencesDialog() override;

private:
    //! The form.
    Ui::PreferencesDialog *m_ui;
};

#endif // CATS2_PREFERENCES_DIALOG_HPP
