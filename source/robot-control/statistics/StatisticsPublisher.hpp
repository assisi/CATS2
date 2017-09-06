#ifndef CATS2_STATISTICS_PUBLISHER_HPP
#define CATS2_STATISTICS_PUBLISHER_HPP

#include "StatisticsSubscriber.hpp"

#include <QtCore/QObject>
#include <QtCore/QSharedPointer>
#include <QtCore/QTimer>
#include <QtCore/QMap>

/*!
 * Class-signleton that provides upon request the robots' statistics.
 */
class StatisticsPublisher : public QObject
{
    Q_OBJECT
public:
    //! The singleton getter.
    static StatisticsPublisher& get();

    // delete copy and move constructors and assign operators
    //! Copy constructor.
    StatisticsPublisher(StatisticsPublisher const&) = delete;
    //! Move constructor.
    StatisticsPublisher(StatisticsPublisher&&) = delete;
    //! Copy assignment.
    StatisticsPublisher& operator=(StatisticsPublisher const&) = delete;
    //! Move assignment.
    StatisticsPublisher& operator=(StatisticsPublisher&&) = delete;

public:
    //! Registers new statistics.
    void addStatistics(QString id);

public slots:
    //! Processes input get statistics messages.
    void onGetStatisticsReceived();
    //! Processes input post statistics requests.
    void onPostStatisticsReceived(QStringList statisticsIdsList);

    //! Updates the statistics value.
    void updateStatistics(QString id, double value);

private:
    //! Constructor.
    explicit StatisticsPublisher();
    //! Destructor.
    virtual ~StatisticsPublisher();

private slots:
    //! Posts the statistics.
    void publishStatistics();

private:
    //! Sends a message.
    void sendMessage(std::string& name, std::string& device,
                     std::string& desc, std::string& data);

private:
    //! The map of statistics and corresponding values.
    QMap<QString, double> m_statistics;

    //! The list of statistics to post.
    QStringList m_statisticsToPost;

    //! The publisher timer.
    QTimer m_updateTimer;

private:
    //! The zmq context, one should use exactly one context in a process.
    zmq::context_t m_context;
    //! The publisher socket.
    zmq::socket_t m_publisher;
    //! The class receiving the data from the remote system.
    StaticsticsSubscriberPtr m_subscriber;

    // TODO : put these data to settings
    //! The receiver address.
    static constexpr char SubscriberAddress[] = "tcp://*:5559";
    //! The sender address.
    static constexpr char PublisherAddress[] = "tcp://*:5560";
};

#endif // CATS2_STATISTICS_PUBLISHER_HPP
