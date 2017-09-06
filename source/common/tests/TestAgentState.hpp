#ifndef CATS2_TEST_AGENT_STATE_HPP
#define CATS2_TEST_AGENT_STATE_HPP

#include <QtTest/QtTest>

/*!
* \brief This class tests the icmp decoder.
*/
class TestAgentState : public QObject
{
    Q_OBJECT
private slots:
    //! Tests the PositionMeters::distance2dToSegment method.
    void positionMetersDistance2dToSegment();

    //! Tests the WorldPolygon::distance2dTo method.
    void worldPolygonDistance2dTo();
};

#endif // CATS2_TEST_AGENT_STATE_HPP
