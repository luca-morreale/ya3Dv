#! /bin/sh

POLYSCOPE_PATH=/home/luca/projects/libraries/polyscope

mkdir include
rm include/*

ln -s ${POLYSCOPE_PATH}/deps/args/args include/args
ln -s ${POLYSCOPE_PATH}/deps/glad include/glad
ln -s ${POLYSCOPE_PATH}/deps/glfw include/glfw
ln -s ${POLYSCOPE_PATH}/deps/glm include/glm
ln -s ${POLYSCOPE_PATH}/deps/happly include/happly
ln -s ${POLYSCOPE_PATH}/deps/imgui/imgui include/imgui
ln -s ${POLYSCOPE_PATH}/deps/json/include/json include/json
ln -s ${POLYSCOPE_PATH}/include/polyscope include/polyscope
ln -s ${POLYSCOPE_PATH}/deps/stb include/stb
ln -s /home/luca/projects/libraries/libigl/include/igl include/igl

mkdir libs
rm libs/*

ln -s ${POLYSCOPE_PATH}/build/src/libpolyscope.a libs/
ln -s ${POLYSCOPE_PATH}/build/deps/glad/src/libglad.a libs/
ln -s ${POLYSCOPE_PATH}/build/deps/imgui/libimgui.a libs/
ln -s ${POLYSCOPE_PATH}/build/deps/glfw/src/libglfw3.a libs/
ln -s ${POLYSCOPE_PATH}/build/deps/stb/libstb.a libs/

