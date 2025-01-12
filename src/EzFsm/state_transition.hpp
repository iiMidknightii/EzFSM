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

    GDVIRTUAL1R(bool, _process, double)
    GDVIRTUAL1R(bool, _physics_process, double)
    GDVIRTUAL1R(bool, _input, const Ref<InputEvent> &)
    GDVIRTUAL1R(bool, _shortcut_input,const Ref<InputEvent> &)
    GDVIRTUAL1R(bool, _unhandled_input, const Ref<InputEvent> &)
    GDVIRTUAL1R(bool, _unhandled_key_input, const Ref<InputEvent> &)

    StateTransition();
    ~StateTransition();

protected:
    static void _bind_methods();
    void _get_property_list(List<PropertyInfo> *p_list) const;
    bool _property_can_revert(StringName p_name) const;
    bool _property_get_revert(StringName p_name, Variant &r_value) const;
    bool _set(StringName p_name, const Variant &p_value);
    bool _get(StringName p_name, Variant &r_value) const;

private:
    Ref<State> from_state;
    StringName to_state_name;
    Ref<StateInput> input;

    void _set_from_state(Ref<State> p_state);
};

}

#endif