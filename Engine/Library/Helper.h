#include "vulkan/vulkan/vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

#include <filesystem>
#include <cstdio>
#include <iostream>
#include <ctime>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <future>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <cstdint>
#include <limits>
#include <optional>
#include <fstream>
#include <set>
#include <map>
#include <list>
#include <sys/types.h>
#include <direct.h>
#include <sys/stat.h>
#include <stdio.h>

namespace gll
{
    float randomiseFloat(float LO, float HI);
    int randomiseInt(int LO, int HI);
};
