#ifndef INTERPOLATIONS_HPP_
#define INTERPOLATIONS_HPP_

#include <vector>

#include "object_3d.hpp"

class AnimatedInterpolation {
public:
    AnimatedInterpolation(std::vector<std::string> files, std::string name);
    ~AnimatedInterpolation();

    void draw();
    void animate();

    void set_meshes(std::vector<std::string> files, std::string name);
    void set_active(int index);

protected:
    void wait();
    bool has_waited_enough();
    void go_down();
    void go_up();
    void reset_wait_step();
    void advance_animation();

private:
    std::vector<Object3D> meshes;

    int num_meshes;
    int current_active;
    int increment;

    int MAX_WAIT_STEPS = 5;
    int wait_step = 0;

};


#endif // INTERPOLATIONS_HPP_
