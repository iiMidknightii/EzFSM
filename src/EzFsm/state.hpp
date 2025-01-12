#ifndef __GDSTATE_H__
#define __GDSTATE_H__

#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/gdvirtual.gen.inc>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/input_event.hpp>

#include "state_input.hpp"

namespace godot::ez_fsm {

class StateMachine;
class StateTransition;

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

    Ref<StateTransition> add_transition_to(const Ref<State> &p_to);
    void append_transition(Ref<StateTransition> p_transition);
    Ref<StateTransition> get_transition_to(const Ref<State> &p_to) const;
    TypedArray<StateTransition> get_all_transitions() const;
    int64_t get_transition_priority(Ref<StateTransition> p_transition) const;
    void move_transition_priority(Ref<StateTransition> p_transition, uint64_t p_priority);
    void remove_transition(Ref<StateTransition> p_transition);

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

    State();
    ~State();

protected:
    static void _bind_methods();
    void _get_property_list(List<PropertyInfo> *p_list) const;
    bool _set(const StringName &p_name, const Variant &p_value);
    bool _get(const StringName &p_name, Variant &r_ret) const;

private:
    StringName state_name;
    bool enabled { true };
    bool transitions_to_self { false };

    Vector<Ref<StateTransition>> transitions;
    StateMachine *machine = nullptr;

    void _set_state_machine(StateMachine *p_machine);
    Ref<StateTransition> _get_transition(uint64_t p_idx) const;

#ifdef DEBUG_ENABLED
    Color node_color = Color::get_named_color(Color::find_named_color("DARK_GRAY"));
    Vector2 node_position;
#endif

};

}

#endif