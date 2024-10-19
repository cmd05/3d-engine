#include <glm/glm.hpp>

// TODO: create common interface for config values
// which read from a file and initialize values
constexpr float DEFAULT_CAM_VFOV = glm::radians(45.0f);
constexpr float DEFAULT_CAM_NEAR_CLIP = 0.1;
constexpr float DEFAULT_CAM_FAR_CLIP = 1000;

// camera direction vectors
constexpr glm::vec3 DEFAULT_CAM_WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f);
constexpr glm::vec3 DEFAULT_CAM_POS = glm::vec3(0.0f);
constexpr glm::vec3 DEFAULT_CAM_FRONT = glm::vec3(0.0f, 0.0f, -1.0f);
constexpr glm::vec3 DEFAULT_CAM_UP = DEFAULT_CAM_WORLD_UP;

// camera rotation
const float DEFAULT_CAM_YAW = glm::asin(DEFAULT_CAM_FRONT.z); // default yaw corresponding to cam_front
constexpr float DEFAULT_CAM_PITCH = 0;

// camera movement
constexpr float CAMERA_PITCH_LIMIT = glm::radians(89.0f);
constexpr float CAMERA_VFOV_MIN = glm::radians(1.0f);
constexpr float CAMERA_VFOV_MAX = glm::radians(45.0f);

// camera input
constexpr double CAMERA_MOUSE_SENSITIVITY = 0.05;
constexpr double CAMERA_SCROLL_SENSITIVITY = 1.0;
constexpr float CAMERA_SPEED = 90.0f;