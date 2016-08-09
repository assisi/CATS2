#include "AgentData.hpp"

/*!
 * Constructor.
 */
AgentDataWorld::AgentDataWorld(unsigned char id, AgentType type, StateWorld stateWorld) :
    m_id(id),
    m_type(type),
    m_stateWorld(stateWorld),
    m_timestamp(std::chrono::milliseconds())
{

}

/*!
 * Constructor.
 */
AgentDataImage::AgentDataImage(unsigned char id, AgentType type, StateImage stateImage) :
    m_id(id),
    m_type(type),
    m_stateImage(stateImage),
    m_timestamp(std::chrono::milliseconds())
{

}

/*!
 * Set the position and orientation.
 */
void AgentDataImage::setState(cv::Point2f center, float direction)
{
    m_stateImage.setPosition(PositionPixels(center.x, center.y));
    m_stateImage.setOrientation(OrientationRad(direction));
}

/*!
 * Set the position, the orientation is considered unknown.
 */
void AgentDataImage::setState(cv::Point2f center)
{
    m_stateImage.setPosition(PositionPixels(center.x, center.y));
    m_stateImage.setOrientation(OrientationRad(0, false));
}

/*!
 * Set the state values invalid.
 */
void AgentDataImage::invalidateState()
{
    PositionPixels position = m_stateImage.position();
    position.setValid(false);
    m_stateImage.setPosition(position);

    OrientationRad orientation = m_stateImage.orientation();
    orientation.setValid(false);
    m_stateImage.setOrientation(orientation);
}
