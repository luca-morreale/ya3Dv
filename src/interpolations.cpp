
#include "interpolations.hpp"


AnimatedInterpolation::AnimatedInterpolation(std::vector<std::string> files, std::string name)
{
    this->set_meshes(files, name);
    this->go_up();
}

AnimatedInterpolation::~AnimatedInterpolation()
{
    this->meshes.clear();
}

void AnimatedInterpolation::draw()
{
    this->meshes[this->current_active].draw();
}

void AnimatedInterpolation::animate()
{
    this->draw();

    if (this->has_waited_enough()) {
        this->advance_animation();
    } else {
        this->wait();
    }
}

void AnimatedInterpolation::wait()
{
    this->wait_step++;
}

bool AnimatedInterpolation::has_waited_enough()
{
    return this->wait_step == MAX_WAIT_STEPS;
}

void AnimatedInterpolation::go_down()
{
    this->increment = -1;
}

void AnimatedInterpolation::go_up()
{
    this->increment = +1;
}

void AnimatedInterpolation::reset_wait_step()
{
    this->wait_step = 0;
}

void AnimatedInterpolation::advance_animation()
{
    this->current_active += increment;

    if (this->current_active == this->num_meshes-1) {
        this->go_down();
    } else if (this->current_active == 0) {
        this->go_up();
    }

    this->reset_wait_step();
}

void AnimatedInterpolation::set_meshes(std::vector<std::string> files, std::string name)
{
    for (uint i = 0; i < files.size(); i++) {
        std::string file = files[i];
        this->meshes.push_back(Object3D(file, name));
    }

    auto rand_colors = this->meshes[0].get_vertices_random_colors();

    for (uint i = 0; i < files.size(); i++) {
        this->meshes[i].set_vertices_random_colors(rand_colors);
    }

    this->current_active = 0;
    this->go_up();
    this->reset_wait_step();
    this->num_meshes = this->meshes.size();
}

void AnimatedInterpolation::set_active(int index)
{
    this->current_active = index;
    this->reset_wait_step();
}
