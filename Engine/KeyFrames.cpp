#include "KeyFrames.h"

KeyFrames::KeyFrames(std::vector<KeyPosition>&& positions,std::vector<KeyRotation>&& rotations,std::vector<KeyScale>&& scales)
    : m_KeyPositions(std::move(positions)), m_KeyRotations(std::move(rotations)), m_KeyScales(std::move(scales))
{
}

glm::mat4 KeyFrames::InterpolateFramesAt(float time)
{
    const glm::mat4 translation = InterpolatePosition(time);
    const glm::mat4 rotation = InterpolateRotation(time);
    const glm::mat4 scale = InterpolateScale(time);
    _prev_animation_time = time;
    return (translation * rotation * scale);
}

glm::mat4 KeyFrames::InterpolatePosition(float time)
{
    const int p0 = UpdateFrameIndex(m_KeyPositions, time, _prev_position_index, _prev_animation_time);
    _prev_position_index = p0;
    const KeyPosition& prev = m_KeyPositions[p0];
    const KeyPosition& next = m_KeyPositions[p0 + 1];
    const float scale_factor = GetScaleFactor(prev.time_stamp, next.time_stamp, time);
    const glm::vec3 position = glm::mix(prev.position, next.position, scale_factor);
    return glm::translate(glm::mat4(1.0f), position);
}

glm::mat4 KeyFrames::InterpolateRotation(float time)
{
    const int p0 = UpdateFrameIndex(m_KeyRotations, time, _prev_rotation_index, _prev_animation_time);
    _prev_rotation_index = p0;
    const KeyRotation& prev = m_KeyRotations[p0];
    const KeyRotation& next = m_KeyRotations[p0 + 1];
    const float scale_factor = GetScaleFactor(prev.time_stamp, next.time_stamp, time);
    const glm::quat rotation = glm::normalize(glm::slerp(
        prev.rotation, next.rotation, scale_factor));
    return glm::toMat4(rotation);
}

glm::mat4 KeyFrames::InterpolateScale(float time)
{
    const int p0 = UpdateFrameIndex(m_KeyScales, time, _prev_scale_index, _prev_animation_time);
    _prev_scale_index = p0;
    const KeyScale& prev = m_KeyScales[p0];
    const KeyScale& next = m_KeyScales[p0 + 1];
    const float scale_factor = GetScaleFactor(prev.time_stamp, next.time_stamp, time);
    const glm::vec3 scale = glm::mix(prev.scale, next.scale, scale_factor);
    return glm::scale(glm::mat4(1.0f), scale);
}


int KeyFrames::GetPositionIndex(float time)
{
    auto it = std::lower_bound(m_KeyPositions.cbegin(), m_KeyPositions.cend(), time, [](const KeyPosition& rhs,float animation_time){return (animation_time < rhs.time_stamp); });
    if (it == m_KeyPositions.cbegin())
    {
        it = m_KeyPositions.cbegin() + 1;
    }
    const int index = (int(std::distance(m_KeyPositions.cbegin(), it)) - 1);
    return index;
}

int KeyFrames::GetRotationIndex(float time)
{
    auto it = std::lower_bound(m_KeyRotations.cbegin(), m_KeyRotations.cend(), time, [](const KeyRotation& rhs,float animation_time) {return (animation_time < rhs.time_stamp); });
    if (it == m_KeyRotations.cbegin())
    {
        it = m_KeyRotations.cbegin() + 1;
    }
    const int index = (int(std::distance(m_KeyRotations.cbegin(), it)) - 1);
    return index;
}

int KeyFrames::GetScaleIndex(float time)
{
    auto it = std::lower_bound(m_KeyScales.cbegin(), m_KeyScales.cend(), time, [](const KeyScale& rhs,float animation_time) {return (rhs.time_stamp < animation_time); });
    if (it == m_KeyScales.cbegin())
    {
        it = m_KeyScales.cbegin() + 1;
    }
    const int index = (int(std::distance(m_KeyScales.cbegin(), it)) - 1);
    return index;
}