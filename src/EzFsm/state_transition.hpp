#ifndef __GDSTATETRANSITION_H__
#define __GDSTATETRANSITION_H__

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/core/gdvirtual.gen.inc>

namespace godot::ez_fsm {

class State;
class StateMachine;
class StateInput;

class StateTransition : public Resource {
    GDCLASS(StateTransition, Resource)

friend class StateMachine;
friend class State;

public:
    Ref<State> get_from_state() const;

    void set_to_state(Ref<State> p_state); 
    Ref<State> get_to_state() const;

    void set_state_input(Ref<StateInput> p_state_input);

    void set_context(Node *p_context);
    Node *get_context() const;

    StateMachine *get_state_machine() const;

    bool request_transition();

    void _validate_property(PropertyInfo &p_prop) const;

    GDVIRTUAL1R(bool, _process, double)
    GDVIRTUAL1R(bool, _physics_process, double)
    GDVIRTUAL1R(bool, _input, const Ref<InputEvent> &)
    GDVIRTUAL1R(bool, _shortcut_input,const Ref<InputEvent> &)
    GDVIRTUAL1R(bool, _unhandled_input, const Ref<InputEvent> &)
    GDVIRTUAL1R(bool, _unhandled_key_input, const Ref<InputEvent> &)

protected:
    static void _bind_methods();

private:
    Ref<State> from_state;
    StringName to_state_name;
    Ref<StateInput> input;

    void _set_from_state(Ref<State> p_state);
    void _set_to_state(StringName p_name);
    StringName _get_to_state() const;

};

}

#endif