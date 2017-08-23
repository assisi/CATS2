#include "TestAgentState.hpp"

#include "AgentState.hpp"

/*!
 * Tests the PositionMeters::distance2dToSegment method.
 */
void TestAgentState::positionMetersDistance2dToSegment()
{
    PositionMeters p1(0,0);
    PositionMeters p2(1,1);

    // point projects to a segment extremity
    PositionMeters p(2,0);
    double distance = p.distance2dToSegment(p1, p2);
    QVERIFY(qFuzzyCompare(distance, qSqrt(2)));

    // point projects inside of the segment
    p.setX(1);
    distance = p.distance2dToSegment(p1, p2);
    QVERIFY(qFuzzyCompare(distance, qSqrt(2)/2));

    // point projects outside of the segment
    p.setX(3);
    distance = p.distance2dToSegment(p1, p2);
    QVERIFY(qFuzzyCompare(distance, qSqrt(5)));
}

/*!
 * Tests the WorldPolygon::distance2dTo method.
 */
void TestAgentState::worldPolygonDistance2dTo()
{
    WorldPolygon polygon;
    polygon.append(PositionMeters(0, 0));
    polygon.append(PositionMeters(1, 0));
    polygon.append(PositionMeters(1, 2));
    polygon.append(PositionMeters(0, 2));

    PositionMeters p(0.5, 1);
    QVERIFY(qFuzzyCompare(polygon.distance2dTo(p), 0.5));

    p.setX(0.4);
    QVERIFY(qFuzzyCompare(polygon.distance2dTo(p), 0.4));

    p.setY(1.8);
    QVERIFY(qFuzzyCompare(polygon.distance2dTo(p), 0.2));
}

QTEST_MAIN(TestAgentState)
