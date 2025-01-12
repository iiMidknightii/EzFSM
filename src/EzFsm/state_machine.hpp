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
    void append_state(const Ref<State> &p_state);
    bool has_state(const StringName &p_state) const;
    Ref<State> get_state(const StringName &p_state) const;
    TypedArray<State> get_all_states() const;
    void remove_state(Ref<State> p_state);
    StringName increment_state_name(const StringName &p_name) const;
    TypedArray<StringName> get_all_state_names() const;

    void set_default_state(const Ref<State> &p_state);
    Ref<State> get_default_state() const;

    Ref<State> get_active_state() const;

    Ref<StateTransition> add_transition_between(const Ref<State> &p_from, const Ref<State> &p_to);
    TypedArray<StateTransition> get_transitions_from(const Ref<State> &p_from) const;
    TypedArray<StateTransition> get_transitions_to(const Ref<State> &p_to) const;
    Ref<StateTransition> get_transition_between(const Ref<State> &p_from, const Ref<State> &p_to) const;
    TypedArray<StateTransition> get_all_transitions() const;
    void remove_transition(Ref<StateTransition> p_transition);

    void start(StringName p_state = StringName(), Ref<StateInput> p_input = Ref<StateInput>());
    bool transition_to(StringName p_state, Ref<StateInput> p_input = Ref<StateInput>());
    void stop();

    virtual PackedStringArray _get_configuration_warnings() const override;
    virtual void _input(const Ref<InputEvent> &p_event) override;
    virtual void _shortcut_input(const Ref<InputEvent> &p_event) override;
    virtual void _unhandled_input(const Ref<InputEvent> &p_event) override;
    virtual void _unhandled_key_input(const Ref<InputEvent> &p_event) override;

    GDVIRTUAL2(_start, Ref<State>, Ref<StateInput>)
    GDVIRTUAL2R(bool, _transition, Ref<State>, Ref<StateInput>)
    GDVIRTUAL0(_stop)

    StateMachine();
    ~StateMachine();

protected:
    static void _bind_methods();
    void _notification(int p_what);
    void _get_property_list(List<PropertyInfo> *p_list) const;
    bool _property_can_revert(const StringName &p_name) const;
    bool _property_get_revert(const StringName &p_name, Variant &r_value);
    bool _set(const StringName &p_name, const Variant &p_value);
    bool _get(const StringName &p_name, Variant &r_ret) const;

private:
    bool auto_start = true;
    bool running = false;
    bool locked_out = false;

    Vector<Ref<State>> states;
    StringName default_state_name;
    uint64_t active_state_idx;

    Node *context = nullptr;

    Ref<State> _get_state(uint64_t p_idx) const;
    void _activate_state(Ref<State> p_state, Ref<StateInput> p_input);
    void _deactivate_state();

    void _ready_transition_input(Ref<StateInput> p_input);
};

}

#endif