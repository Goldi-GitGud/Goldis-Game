cmake_minimum_required(VERSION 3.16)

project(GoldisGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(start
    src/start.cpp
    src/Game.cpp
    src/Menu.cpp
    src/Manager.cpp
    src/Combat.cpp
    src/Player.cpp
    src/Enemy.cpp
    src/Item.cpp
    src/Weapon.cpp
    src/Bag.cpp
    src/Utils.cpp
)

target_include_directories(start PRIVATE src)

if(MSVC)
    target_compile_options(start PRIVATE /W4)
else()
    target_compile_options(start PRIVATE -Wall -Wextra)
endif()

# The game loads stats from data/*_data.json relative to the working
# directory, so copy data/ next to the binary after every build. Run the
# game from its own directory (e.g. build/start) and saves
# (save_slot_*.json, git-ignored) land there too.
add_custom_command(TARGET start POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data
        $<TARGET_FILE_DIR:start>/data
)

# Install rules: `cmake --install build --prefix dist` produces a playable
# folder with the binary + data/ side by side. Zip the prefix folder and
# share it; your friends just unzip and run start (saves land next to it).
install(TARGETS start
    RUNTIME DESTINATION .
)
install(DIRECTORY data
    DESTINATION .
)
