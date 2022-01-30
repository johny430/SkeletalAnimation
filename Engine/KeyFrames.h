#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

struct KeyPosition
{
	glm::vec3 position;
	float time_stamp;
};

struct KeyRotation
{
	glm::quat rotation;
	float time_stamp;
};

struct KeyScale
{
	glm::vec3 scale;
	float time_stamp;
};

class KeyFrames
{
public:
	KeyFrames(std::vector<KeyPosition>&& positions,std::vector<KeyRotation>&& rotations,std::vector<KeyScale>&& scales);
	KeyFrames() {};
	~KeyFrames() {};
	glm::mat4 InterpolateFramesAt(float time);
private:
	std::vector<KeyPosition> m_KeyPositions;
	std::vector<KeyRotation> m_KeyRotations;
	std::vector<KeyScale> m_KeyScales;

	float _prev_animation_time = -1;
	int _prev_position_index = -1;
	int _prev_rotation_index = -1;
	int _prev_scale_index = -1;
private:
	glm::mat4 InterpolatePosition(float time);
	glm::mat4 InterpolateRotation(float time);
	glm::mat4 InterpolateScale(float time);

	int GetPositionIndex(float time);
	int GetRotationIndex(float time);
	int GetScaleIndex(float time);
private:
    template<typename Key>
    struct KeyTimeCompare
    {
        bool operator()(const Key& lhs, float animation_time) const noexcept
        {
            return (lhs.time_stamp < animation_time);
        }
    };

    template<typename Key>
    static int GetFrameIndex(const std::vector<Key>& frames, float animation_time, unsigned start_offset, unsigned end_offset)
    {
        auto it = std::lower_bound(frames.cbegin() + start_offset, frames.cbegin() + end_offset, animation_time, KeyTimeCompare<Key>{});
        if (it == frames.cbegin())
        {
            it = frames.cbegin() + 1;
        }
        const int index = (int(std::distance(frames.cbegin(), it)) - 1);
        return index;
    }

    template<typename Key>
    static int UpdateFrameIndex(const std::vector<Key>& frames, float animation_time, int prev_known_index, float prev_animation_time)
    {
        if (prev_known_index < 0)
        {
            return GetFrameIndex(frames, animation_time, 0, unsigned(frames.size()));
        }
        if (animation_time >= prev_animation_time)
        {
            return GetFrameIndex(frames, animation_time, prev_known_index, unsigned(frames.size()));
        }
        return GetFrameIndex(frames, animation_time, 0, prev_known_index);
    }

    static float GetScaleFactor(float prev_time_stamp, float next_time_stamp, float animation_time)
    {
        const float progress = animation_time - prev_time_stamp;
        const float total = next_time_stamp - prev_time_stamp;
        return progress / total;
    }
	friend class Importer;
};