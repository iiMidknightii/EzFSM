#ifndef __GDSTATEINPUT_H__
#define __GDSTATEINPUT_H__

#include <godot_cpp/classes/ref_counted.hpp>

namespace godot::ez_fsm {

class StateInput : public RefCounted {
    GDCLASS(StateInput, RefCounted)

friend class StateMachine;

public:
    StringName get_previous_state() const;

protected:
    static void _bind_methods();

private:
    StringName previous_state;
};

}


#endif