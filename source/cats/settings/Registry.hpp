#ifndef CATS2_REGISTRY_HPP
#define CATS2_REGISTRY_HPP

#include <QtCore/QObject>

/*!
 * Class-signleton that is stores persistent settings of the applications.
 */
class Registry : public QObject
{
    Q_OBJECT

public:
    //! The singleton getter.
    static Registry& get();

    // delete copy and move constructors and assign operators
    //! Copy constructor.
    Registry(Registry const&) = delete;
    //! Move constructor.
    Registry(Registry&&) = delete;
    //! Copy assignment.
    Registry& operator=(Registry const&) = delete;
    //! Move assignment.
    Registry& operator=(Registry &&) = delete;

public:
    //! Returns the data loggins path.
    QString dataLoggingPath() const { return m_dataLoggingPath; }
    //! Updates the data loggins path.
    void setDataLoggingPath(QString path);

private:
    //! Constructor. Defining it here prevents construction.
    Registry();
    //! Destructor. Defining it here prevents unwanted destruction.
    ~Registry() {}

private:
    //! The path where we store data logs.
    QString m_dataLoggingPath;
};

#endif // CATS2_REGISTRY_HPP
