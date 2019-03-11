set master_project_path=c:/dev/engine

::call cmake -D/CMAKE_RUNTIME_OUTPUT_DIRECTORY=%master_project_path%/bin -D/CMAKE_LIBRARY_OUTPUT_DIRECTORY=%master_project_path%/bin -H%master_project_path%/vendor/assimp -B%master_project_path%/vendor/assimp
::call cmake --build %master_project_path%/vendor/assimp --target assimp --config Debug

::call cmake -D/CMAKE_LIBRARY_OUTPUT_DIRECTORY=%master_project_path%/bin -H%master_project_path%/vendor/libdevil -B%master_project_path%/vendor/libdevil
::call cmake --build %master_project_path%/vendor/libdevil --target libdevil --config Debug

::call cmake -D/CMAKE_RUNTIME_OUTPUT_DIRECTORY=%master_project_path%/bin -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=%master_project_path%/bin -H%master_project_path%/vendor/openal-soft -B%master_project_path%/vendor/openal-soft
::call cmake --build %master_project_path%/vendor/openal-soft --target OpenAL --config Debug

:: FREEALUT REQUIRES OpenAL
::call cmake -D/CMAKE_LIBRARY_OUTPUT_DIRECTORY=%master_project_path%/bin -H%master_project_path%/vendor/freealut -B%master_project_path%/vendor/freealut
::call cmake --build %master_project_path%/vendor/freealut --target freealut --config Debug


