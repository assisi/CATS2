#include "PreferencesDialog.hpp"
#include "ui_PreferencesDialog.h"

#include "settings/Registry.hpp"

#include <QtWidgets/QFileDialog>
#include <QtCore/QDebug>

/*!
 * Constructor.
 */
PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::PreferencesDialog)
{
    m_ui->setupUi(this);

    // set the logging path in the persistent memory
    m_ui->loggingPathEdit->setText(Registry::get().dataLoggingPath());
    connect(m_ui->loggingPathButton, &QPushButton::clicked,
            [=]() {
                QString path = QFileDialog::
                        getExistingDirectory(this, tr("Logging path"),
                                             m_ui->loggingPathEdit->text(),
                                             QFileDialog::ShowDirsOnly |
                                             QFileDialog::DontResolveSymlinks);
                if (!path.isEmpty()) {
                    m_ui->loggingPathEdit->setText(path);
                    Registry::get().setDataLoggingPath(path);
                }
            });
}

/*!
 * Destructor.
 */
PreferencesDialog::~PreferencesDialog()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";    
    delete m_ui;
}
