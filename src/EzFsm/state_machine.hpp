#ifndef __GDSTATEMACHINE_H__
#define __GDSTATEMACHINE_H__

#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/gdvirtual.gen.inc>
#include <godot_cpp/templates/hash_set.hpp>
#include <godot_cpp/classes/node.hpp>
#include "state.hpp"

namespace godot::ez_fsm {

class StateInput;
class StateTransition;

class StateMachine : public Node {
    GDCLASS(StateMachine, Node)

public:
    void set_auto_start(bool p_auto_start);
    bool will_auto_start() const;
    bool is_running() const;

    Node *get_context() const;
    void set_context(Node *p_context);

    Ref<State> add_state(const StringName &p_name);
    bool has_state(const StringName &p_state) const;
    Ref<State> get_state(const StringName &p_state) const;
    TypedArray<State> get_all_states() const;
    void remove_state(Ref<State> p_state);
    StringName increment_state_name(const StringName &p_name) const;
    TypedArray<StringName> get_all_state_names() const;

    void set_default_state(const Ref<State> &p_state);
    Ref<State> get_default_state() const;
    Ref<State> get_active_state() const;

    Ref<StateTransition> add_transition(const Ref<State> &p_from, const Ref<State> &p_to);
    TypedArray<StateTransition> get_transitions_from_state(const Ref<State> &p_state) const;
    TypedArray<StateTransition> get_transitions_to_state(const Ref<State> &p_state) const;
    Ref<StateTransition> get_transition_between_states(const Ref<State> &p_from, const Ref<State> &p_to) const;
    TypedArray<StateTransition> get_all_transitions() const;
    void remove_transition(Ref<StateTransition> p_transition);

    void start(StringName p_state = StringName(), Ref<StateInput> p_input = Ref<StateInput>());
    bool transition_to(StringName p_state, Ref<StateInput> p_input = Ref<StateInput>());
    void stop();

    virtual PackedStringArray _get_configuration_warnings() const override;
    void _validate_property(PropertyInfo &p_prop) const;
    virtual void _input(const Ref<InputEvent> &p_event) override;
    virtual void _shortcut_input(const Ref<InputEvent> &p_event) override;
    virtual void _unhandled_input(const Ref<InputEvent> &p_event) override;
    virtual void _unhandled_key_input(const Ref<InputEvent> &p_event) override;

    GDVIRTUAL2(_start, Ref<State>, Ref<StateInput>)
    GDVIRTUAL2R(bool, _transition, Ref<State>, Ref<StateInput>)
    GDVIRTUAL0(_stop)

protected:
    static void _bind_methods();
    void _notification(int p_what);

private:
    bool auto_start = true;
    bool running = false;
    bool locked_out = false;

    TypedArray<State> states;
    uint64_t default_state_idx;
    uint64_t active_state_idx;

    TypedArray<StateTransition> transitions;
    TypedArray<uint64_t> active_transitions_idx;
    
    Node *context = nullptr;

    void _set_all_states(const TypedArray<State> &p_states);
    void _add_state(Ref<State> p_state);
    void _connect_state(Ref<State> p_state);
    void _disconnect_state(Ref<State> p_state);
    Ref<State> _get_state(uint64_t p_idx) const;
    void _activate_state(Ref<State> p_state, Ref<StateInput> p_input);
    void _deactivate_state();
    void _on_state_name_changed(Ref<State> p_state, const StringName &p_old_name);
    void _remove_state(Ref<State> p_state);
    
    void _set_default_state(StringName p_name);
    StringName _get_default_state() const;

    void _set_all_transitions(const TypedArray<StateTransition> &p_transitions);
    Ref<StateTransition> _get_transition(uint64_t p_idx) const;
    void _ready_transition_input(Ref<StateInput> p_input);
};

}

#endif