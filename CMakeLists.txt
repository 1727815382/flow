cmake_minimum_required(VERSION 3.5)

project(qt_task_framework LANGUAGES CXX)

# ===== C++11（VS2015/MinGW 兼容）=====
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ===== Qt 自动 MOC（因为库里有 Q_OBJECT）=====
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)

# 可选：数据库模块、是否构建示例 APP
option(BUILD_DB          "Enable QtSql-based DB service (optional)" OFF)
option(BUILD_DEMO_APP    "Build demo app that links to the framework" ON)

if (MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS -DNOMINMAX -DWIN32_LEAN_AND_MEAN)
  add_compile_options(/utf-8)
else()
  find_package(Threads REQUIRED)
endif()

# ===== 查找 Qt5（最低 5.6；推荐 5.9.9）=====
find_package(Qt5 5.6 REQUIRED COMPONENTS Core Widgets)
if (BUILD_DB)
  find_package(Qt5 5.6 REQUIRED COMPONENTS Sql)
endif()

# ===== 库源文件（去掉 app/main.cpp，仅保留框架代码）=====
set(FRAMEWORK_SRCS
  core/message_bus.cpp
  core/task_base.cpp
  core/task_manager.cpp
  core/scheduler_task.cpp
  core/flow_task.cpp
  core/supervisor.cpp

  modules/log_service/log_service.cpp
  modules/udp_service/udp_task.cpp
  modules/udp_service/udp_socket.cpp
  modules/metrics_service/metrics_task.cpp
  modules/replay_service/replay_task.cpp
  modules/datahub/datahub_task.cpp
  # 协议引擎
  modules/proto_engine/protocol_engine_task.cpp
  modules/proto_engine/expr_eval.cpp
  modules/proto_engine/field_codec.cpp
  modules/proto_engine/bt.cpp

  # UI（入口窗口、内置视图、UI 适配器）
  ui/mainwindow.cpp
  ui/shell_window.cpp
  ui/ui_bridge.cpp
  ui/ui_adapter_task.cpp
  ui/builtin_views/view_supervisor.cpp
  ui/builtin_views/view_udp_console.cpp
  ui/builtin_views/view_protocol_console.cpp

  # 如需：modules/db_service/db_service.cpp
)

if (BUILD_DB)
  list(APPEND FRAMEWORK_SRCS modules/db_service/db_service.cpp)
endif()

# ===== 定义静态库（默认静态，避免 Windows 下导出符号麻烦）=====
add_library(qt_task_framework STATIC ${FRAMEWORK_SRCS})

# 头文件包含路径对外可见：上层程序可以直接 #include "core/xxx.hpp"
target_include_directories(qt_task_framework
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}      # 让使用者以工程根为包含前缀
)

# 链接依赖（PUBLIC -> 上层可见）
target_link_libraries(qt_task_framework
  PUBLIC
    Qt5::Core
    Qt5::Widgets
)

if (BUILD_DB)
  target_link_libraries(qt_task_framework PUBLIC Qt5::Sql)
endif()
if (NOT MSVC)
  target_link_libraries(qt_task_framework PUBLIC Threads::Threads)
endif()
if (WIN32)
  # UDP socket 依赖
  target_link_libraries(qt_task_framework PUBLIC ws2_32)
endif()

# ===== 可选：安装（将库与头文件安装到前缀下）=====
# install(TARGETS qt_task_framework ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin)
# install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ DESTINATION include/qt_task_framework
#         FILES_MATCHING PATTERN "*.hpp")

# ===== 示例程序（可关掉 BUILD_DEMO_APP）=====
if (BUILD_DEMO_APP)
  # 演示 main，
  set(DEMO_MAIN app/main.cpp)

  if (WIN32)
    add_executable(qt_task_demo WIN32 ${DEMO_MAIN})
    # Qt5::WinMain 只给可执行程序链接
    target_link_libraries(qt_task_demo PRIVATE Qt5::WinMain)
  else()
    add_executable(qt_task_demo ${DEMO_MAIN})
  endif()

  # 链接我们的框架库（其依赖会透传）
  target_link_libraries(qt_task_demo PRIVATE qt_task_framework)

  target_include_directories(qt_task_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
  )

  # 运行目录需要 configs（把源目录下的 configs 复制到 build）
  add_custom_target(copy_configs ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/configs
            ${CMAKE_CURRENT_BINARY_DIR}/configs)
  add_dependencies(qt_task_demo copy_configs)
endif()

message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Qt5 Core/Widgets found")
message(STATUS "Build DB: ${BUILD_DB}")
message(STATUS "Build demo app: ${BUILD_DEMO_APP}")
