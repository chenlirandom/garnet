if (Vulkan_FOUND)
    add_executable(GNtestVulkan main.cpp)
    target_link_libraries(GNtestVulkan GNcore Vulkan::Vulkan)
endif()
