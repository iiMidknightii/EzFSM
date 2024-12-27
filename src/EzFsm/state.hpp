#ifndef __GDSTATE_H__
#define __GDSTATE_H__

#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/gdvirtual.gen.inc>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/input_event.hpp>

#include "state_input.hpp"

namespace godot::ez_fsm {

class StateMachine;

class State : public Resource {
    GDCLASS(State, Resource)

friend class StateMachine;

public:
    StringName get_state_name() const;
    void set_state_name(StringName p_name);

    bool is_enabled() const;
    void set_enabled(bool p_enabled);

    bool can_transition_to_self() const;
    void allow_transition_to_self(bool p_allow);

    bool has_sibling(StringName const &p_name) const;
    Ref<State> get_sibling(StringName const &p_name) const;
    TypedArray<State> get_all_siblings() const;

    Node *get_context() const;
    void set_context(Node *p_context);

    StateMachine *get_state_machine() const;
    
#ifdef DEBUG_ENABLED
    void set_node_color(Color p_node_color);
    Color get_node_color() const;
    void set_node_position(Vector2 p_node_position);
    Vector2 get_node_position() const;
#endif

    GDVIRTUAL1(_start, Ref<StateInput>)
    GDVIRTUAL1R(bool, _can_activate, Ref<StateInput>)
    GDVIRTUAL1(_activate, Ref<StateInput>)
    GDVIRTUAL0(_deactivate)
    GDVIRTUAL0(_stop)

    GDVIRTUAL1(_active_process, double)
    GDVIRTUAL1(_active_physics_process, double)
	GDVIRTUAL1(_active_input, const Ref<InputEvent> &)
	GDVIRTUAL1(_active_shortcut_input, const Ref<InputEvent> &)
	GDVIRTUAL1(_active_unhandled_input, const Ref<InputEvent> &)
	GDVIRTUAL1(_active_unhandled_key_input, const Ref<InputEvent> &)

    GDVIRTUAL1(_inactive_process, double)
    GDVIRTUAL1(_inactive_physics_process, double)
	GDVIRTUAL1(_inactive_input, const Ref<InputEvent> &)
	GDVIRTUAL1(_inactive_shortcut_input, const Ref<InputEvent> &)
	GDVIRTUAL1(_inactive_unhandled_input, const Ref<InputEvent> &)
	GDVIRTUAL1(_inactive_unhandled_key_input, const Ref<InputEvent> &)

protected:
    static void _bind_methods();

private:
    StringName state_name;
    bool enabled { true };
    bool transitions_to_self { false };

    StateMachine *machine = nullptr;

    void _set_state_machine(StateMachine *p_machine);

#ifdef DEBUG_ENABLED
    Color node_color = Color::get_named_color(Color::find_named_color("DARK_GRAY"));
    Vector2 node_position;
#endif

};

}

#endif