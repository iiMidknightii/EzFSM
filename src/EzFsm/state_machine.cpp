#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "state_machine.hpp"
#include "state_transition.hpp"
#include "state_input.hpp"

using namespace godot;
using namespace godot::ez_fsm;

// macro that runs the appropriate virtual methods on all states then checks for transitions
#define EVALUATE_STATES(p_method, ...)                                                                          \
    Ref<State> active_state = get_active_state();                                                               \
\
    for (uint64_t idx = 0; idx < states.size(); ++idx) {                                                        \
        Ref<State> state = _get_state(idx);                                                                     \
        if (!state->is_enabled()) {                                                                             \
            continue;                                                                                           \
        }                                                                                                       \
\
        if (state == active_state) {                                                                            \
            GDVIRTUAL_CALL_PTR(state, _active##p_method##, __VA_ARGS__);                                        \
        } else {                                                                                                \
            GDVIRTUAL_CALL_PTR(state, _inactive##p_method##, __VA_ARGS__);                                      \
        }                                                                                                       \
    }                                                                                                           \
\
    if (active_state.is_valid()) {                                                                              \
        TypedArray<StateTransition> active_transitions = active_state->get_all_transitions();                   \
        for (uint64_t idx = 0; idx < active_transitions.size(); ++idx) {                                        \
            Ref<StateTransition> transition = cast_to<StateTransition>(active_transitions[idx]);                \
            bool do_transition = false;                                                                         \
            GDVIRTUAL_CALL_PTR(transition, ##p_method##, __VA_ARGS__, do_transition);                           \
            if (do_transition) {                                                                                \
                bool success = transition_to(transition->to_state_name, transition->input);                     \
                if (success) {                                                                                  \
                    break;                                                                                      \
                }                                                                                               \
            }                                                                                                   \
        }                                                                                                       \
    }

void StateMachine::_validate_property(PropertyInfo &p_prop) const {
    if (p_prop.name == StringName("_default_state")) {
        p_prop.hint = PROPERTY_HINT_ENUM;
        p_prop.hint_string = String(",").join(get_all_state_names());
    }
}

PackedStringArray StateMachine::_get_configuration_warnings() const {
    PackedStringArray out;

    if (get_all_states().size() == 0) {
        out.append("No states added.  Add states to enable functionality.");
    }
    if (get_default_state().is_null()) {
        out.append("Invalid default state provided.  The state machine will be unable to start.");
    }

    return out;
}

void StateMachine::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_READY: {
            if (!Engine::get_singleton()->is_editor_hint() && auto_start) {
                start();
            }
        } break;

        case NOTIFICATION_INTERNAL_PROCESS: {
            if (!Engine::get_singleton()->is_editor_hint() && running) {
                double delta = get_process_delta_time();
                EVALUATE_STATES(_process, delta)
            }
        } break;

        case NOTIFICATION_INTERNAL_PHYSICS_PROCESS: {
            if (!Engine::get_singleton()->is_editor_hint() && running) {
                double delta = get_physics_process_delta_time();
                EVALUATE_STATES(_physics_process, delta)
            }
        } break;
    }
}

void StateMachine::_input(const Ref<InputEvent> &p_event) {
    if (Engine::get_singleton()->is_editor_hint() || !running) {
        return;
    }

    EVALUATE_STATES(_input, p_event)
}

void StateMachine::_shortcut_input(const Ref<InputEvent> &p_event) {
    if (Engine::get_singleton()->is_editor_hint() || !running) {
        return;
    }

    EVALUATE_STATES(_shortcut_input, p_event)
}

void StateMachine::_unhandled_input(const Ref<InputEvent> &p_event) {
    if (Engine::get_singleton()->is_editor_hint() || !running) {
        return;
    }

    EVALUATE_STATES(_unhandled_input, p_event)
}

void StateMachine::_unhandled_key_input(const Ref<InputEvent> &p_event) {
    if (Engine::get_singleton()->is_editor_hint() || !running) {
        return;
    }

    EVALUATE_STATES(_unhandled_key_input, p_event)
}

TypedArray<StringName> StateMachine::get_all_state_names() const {
    TypedArray<StringName> out;
    for (uint64_t idx = 0; idx < states.size(); ++idx) {
        Ref<State> state = _get_state(idx);
        out.push_back(state->get_state_name());
    }
    return out;
}

TypedArray<State> StateMachine::get_all_states() const {
    return states;
}

void StateMachine::_set_all_states(const TypedArray<State> &p_states) {
    if (running) {
        stop();
    }

    default_state_idx = 0;
    active_state_idx = 0;

    for (uint64_t idx = 0; idx < states.size(); ++idx) {
        Ref<State> state = _get_state(idx);
        _remove_state(state);
    }
    states.clear();

    states.append_array(p_states);
    for (uint64_t idx = 0; idx < states.size(); ++idx) {
        Ref<State> state = _get_state(idx);
        _add_state(state);
    }
}

bool StateMachine::has_state(const StringName &p_state) const {
    for (uint64_t idx = 0; idx < states.size(); ++idx) {
        Ref<State> state = _get_state(idx);
        if (state->get_state_name() == p_state) {
            return true;
        }
    }

    return false;
}

Ref<State> StateMachine::get_state(const StringName &p_state) const {
    for (uint64_t idx = 0; idx < states.size(); ++idx) {
        Ref<State> state = _get_state(idx);
        if (state->get_state_name() == p_state) {
            return state;
        }
    }

    return Ref<State>();
}

StringName StateMachine::increment_state_name(const StringName &p_name) const {
    String out = p_name;

    bool has_num = false;
    uint64_t num = -1;
    for (uint64_t idx = 0; idx < out.length(); ++idx) {
        String tmp = out.right(-idx);
        if (tmp.is_valid_int()) {
            num = tmp.to_int();
            has_num = true;
        } else {
            break;
        }
    }

    if (!has_num) {
        num = 2;
    }

    while (has_state(out)) {
        if (has_num) {
            out = out.trim_suffix(itos(num));
        } else {
            has_num = true;
        }
        out = out + itos(++num);
    }

    return out;
}

void StateMachine::_set_default_state(StringName p_name) {
    for (uint64_t idx = 0; idx < states.size(); ++idx) {
        Ref<State> state = _get_state(idx);
        if (state->get_state_name() == p_name) {
            default_state_idx = idx;
            emit_signal("default_changed", get_default_state());
            update_configuration_warnings();
            return;
        }
    }

    update_configuration_warnings();
    ERR_FAIL_MSG("Could not find state name to set default.");
}

StringName StateMachine::_get_default_state() const {
    Ref<State> state = get_default_state();
    if (state.is_valid()) {
        return state->get_state_name();
    } else {
        return StringName();
    }
}

Ref<State> StateMachine::get_default_state() const {
    return _get_state(default_state_idx);
}

void StateMachine::set_default_state(const Ref<State> &p_state) {
    ERR_FAIL_NULL(p_state);
    ERR_FAIL_COND_MSG(!states.has(p_state), "State was not added before attempting to set to default.");

    _set_default_state(p_state->get_state_name());
}

Ref<State> StateMachine::get_active_state() const {
    if (running) {
        return _get_state(active_state_idx);
    } else {
        return Ref<State>();
    }
}

Ref<State> StateMachine::add_state(const StringName &p_name) {
    bool is_default = states.is_empty();
    Ref<State> state;
    state.instantiate();
    states.append(state);
    state->set_state_name(increment_state_name(p_name));
    _add_state(state);
    if (is_default) {
        set_default_state(state);
    }
    update_configuration_warnings();
    emit_signal("state_added", state);

    return state;
}

void StateMachine::remove_state(Ref<State> p_state) {
    if (p_state.is_valid() && states.has(p_state)) {
        if (p_state == get_active_state()) {
            stop();
        }
        if (p_state == get_default_state()) {
            default_state_idx = 0;
            update_configuration_warnings();
        }
        states.erase(p_state);
        p_state->_set_state_machine(nullptr);
        update_configuration_warnings();
        emit_signal("state_removed", p_state);
    }
}

Ref<StateTransition> StateMachine::add_transition_between(const Ref<State> &p_from, const Ref<State> &p_to) {
    ERR_FAIL_NULL_V(p_from, nullptr);
    ERR_FAIL_NULL_V(p_to, nullptr);

    Ref<StateTransition> transition = p_from->add_transition_to(p_to);
    return transition;
}

void StateMachine::remove_transition(Ref<StateTransition> p_transition) {
    ERR_FAIL_NULL(p_transition);

    for (uint64_t idx = 0; idx < states.size(); ++idx) {
        Ref<State> state = _get_state(idx);
        if (state == p_transition->get_from_state()) {
            state->remove_transition(p_transition);
            return;
        }
    }
}

TypedArray<StateTransition> StateMachine::get_transitions_from(const Ref<State> &p_from) const {
    ERR_FAIL_NULL_V(p_from, TypedArray<StateTransition>());

    return p_from->get_all_transitions();
}

TypedArray<StateTransition> StateMachine::get_transitions_to(const Ref<State> &p_to) const {
    ERR_FAIL_NULL_V(p_to, TypedArray<StateTransition>());

    TypedArray<StateTransition> out;
    for (uint64_t idx = 0; idx < states.size(); ++idx) {
        Ref<State> state = _get_state(idx);
        Ref<StateTransition> transition = state->get_transition_to(p_to);
        if (transition.is_valid()) {
            out.push_back(transition);
        }
    }

    return out;
}

Ref<StateTransition> StateMachine::get_transition_between(const Ref<State> &p_from, const Ref<State> &p_to) const {
    ERR_FAIL_NULL_V(p_from, nullptr);
    ERR_FAIL_NULL_V(p_to, nullptr);

    return p_from->get_transition_to(p_to);
}

TypedArray<StateTransition> StateMachine::get_all_transitions() const {
    TypedArray<StateTransition> out;
    for (uint64_t idx = 0; idx < states.size(); ++idx) {
        out.append_array(_get_state(idx)->get_all_transitions());
    }
    return out;
}

Node *StateMachine::get_context() const {
    return context;
}

void StateMachine::set_context(Node *p_context) {
    if (context == p_context) {
        return;
    }

    context = p_context;

    emit_signal("context_changed", get_context());
    for (uint64_t idx = 0; idx < states.size(); ++idx) {
        Ref<State> state = _get_state(idx);
        state->emit_changed();
        TypedArray<StateTransition> transitions = state->get_all_transitions();
        for (uint64_t idx = 0; idx < transitions.size(); ++idx) {
            cast_to<StateTransition>(transitions[idx])->emit_changed();
        }
    }
}

void StateMachine::set_auto_start(bool p_auto_start) {
    auto_start = p_auto_start;
}

bool StateMachine::will_auto_start() const {
    return auto_start;
}

bool StateMachine::is_running() const {
    return running;
}

void StateMachine::start(StringName p_state, Ref<StateInput> p_input) {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    Ref<State> starting_state = get_default_state();
    ERR_FAIL_COND_MSG(locked_out, "State machine cannot restart while transition is ongoing.");
    ERR_FAIL_NULL_MSG(starting_state, "No default state provided, cannot start state machine.");

    if (running) {
        stop();
    }

    locked_out = true;
    GDVIRTUAL_CALL(_start, starting_state, p_input);
    GDVIRTUAL_CALL_PTR(starting_state, _start, p_input);
    running = true;
    _activate_state(starting_state, p_input);
    locked_out = false;

    emit_signal("started", starting_state, p_input);

    set_process_internal(true);
    set_physics_process_internal(true);
    set_process_input(true);
    set_process_shortcut_input(true);
    set_process_unhandled_input(true);
    set_process_unhandled_key_input(true);
}

bool StateMachine::transition_to(StringName p_state, Ref<StateInput> p_input) {
    if (Engine::get_singleton()->is_editor_hint()) {
        return false;
    }

    ERR_FAIL_COND_V_MSG(locked_out, false, "State machine will not transition while another transition is ongoing.");
    ERR_FAIL_COND_V_MSG(!running, false, "State machine must be started before it can transition.");

    Ref<State> next_state = get_state(p_state);
    ERR_FAIL_NULL_V_MSG(next_state, false, "Invalid state name passed in.");
    if (!next_state->is_enabled()) {
        return false;
    }

    Ref<State> cur_state = get_active_state();
    ERR_FAIL_COND_V_MSG(
        cur_state.is_valid() && !cur_state->can_transition_to_self() && cur_state == next_state,
        false, "State requested to transition to itself, but was disallowed from doing so.");

    locked_out = true;
    _ready_transition_input(p_input);

    bool cont_with_transition = true;
    GDVIRTUAL_CALL(_transition, next_state, p_input, cont_with_transition);
    if (!cont_with_transition) { // when machine script virtual method request a transition abort
        locked_out = false;
        return false;
    }

    GDVIRTUAL_CALL_PTR(next_state, _can_activate, p_input, cont_with_transition);
    if (!cont_with_transition) { // when state virtual method says transition is invalid
        locked_out = false;
        return false;
    }

    Ref<State> prev_state = cur_state;
    if (prev_state.is_valid()) {
        _deactivate_state();
    }

    _activate_state(next_state, p_input);
    locked_out = false;

    emit_signal("transitioned", prev_state, next_state, p_input);
    return true;
}

void StateMachine::stop() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    ERR_FAIL_COND_MSG(locked_out, "State machine cannot be stopped while transition ongoing.");
    ERR_FAIL_COND_MSG(!running, "State machine must be started before it can stop.");

    locked_out = true;
    GDVIRTUAL_CALL(_stop);

    Ref<State> stopped_state = get_active_state();
    if (stopped_state.is_valid()) {
        _deactivate_state();
        GDVIRTUAL_CALL_PTR(stopped_state, _stop);
    }

    locked_out = false;
    running = false;

    emit_signal("stopped", stopped_state);

    set_process_internal(false);
    set_physics_process_internal(false);
    set_process_input(false);
    set_process_shortcut_input(false);
    set_process_unhandled_input(false);
    set_process_unhandled_key_input(false);
}

void StateMachine::_ready_transition_input(Ref<StateInput> p_input) {
    if (p_input.is_null()) {
        p_input.instantiate();
    }
    Ref<State> active_state = get_active_state();
    if (active_state.is_valid()) {
        p_input->previous_state = active_state->get_state_name();
    } else {
        p_input->previous_state = StringName();
    }
}

void StateMachine::_activate_state(Ref<State> p_state, Ref<StateInput> p_input) {
    ERR_FAIL_NULL(p_state);   
    
    GDVIRTUAL_CALL_PTR(p_state, _activate, p_input);
    for (uint64_t idx = 0; idx < states.size(); ++idx) {
        if (p_state == _get_state(idx)) {
            active_state_idx = idx;
            break;
        }
    }
}

void StateMachine::_deactivate_state() {
    Ref<State> prev_state = get_active_state();
    ERR_FAIL_NULL(prev_state);

    GDVIRTUAL_CALL_PTR(prev_state, _deactivate);
    prev_state.unref();
}

void StateMachine::_add_state(Ref<State> p_state) {
    ERR_FAIL_NULL(p_state);
    p_state->_set_state_machine(this);
}

void StateMachine::_remove_state(Ref<State> p_state) {
    ERR_FAIL_NULL(p_state);
    p_state->_set_state_machine(nullptr);
}

Ref<State> StateMachine::_get_state(uint64_t p_idx) const {
    if (p_idx < 0 || p_idx >= states.size()) {
        return nullptr;
    } else {
        return Ref<State>(states[p_idx]);
    }
}

void StateMachine::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_auto_start", "auto_start"), &StateMachine::set_auto_start);
    ClassDB::bind_method(D_METHOD("will_auto_start"), &StateMachine::will_auto_start);

    ClassDB::bind_method(D_METHOD("add_state", "name"), &StateMachine::add_state);
    ClassDB::bind_method(D_METHOD("has_state", "name"), &StateMachine::has_state);
    ClassDB::bind_method(D_METHOD("get_state", "name"), &StateMachine::get_state);
    ClassDB::bind_method(D_METHOD("_set_all_states", "states"), &StateMachine::_set_all_states);
    ClassDB::bind_method(D_METHOD("get_all_states"), &StateMachine::get_all_states);
    ClassDB::bind_method(D_METHOD("remove_state", "state"), &StateMachine::remove_state);
    ClassDB::bind_method(D_METHOD("increment_state_name", "name"), &StateMachine::increment_state_name);
    ClassDB::bind_method(D_METHOD("get_all_state_names"), &StateMachine::get_all_state_names);

    ClassDB::bind_method(D_METHOD("_set_default_state", "idx"), &StateMachine::_set_default_state);
    ClassDB::bind_method(D_METHOD("_get_default_state"), &StateMachine::_get_default_state);
    ClassDB::bind_method(D_METHOD("set_default_state"), &StateMachine::set_default_state);
    ClassDB::bind_method(D_METHOD("get_default_state"), &StateMachine::get_default_state);
    ClassDB::bind_method(D_METHOD("get_active_state"), &StateMachine::get_active_state);

    ClassDB::bind_method(D_METHOD("get_context"), &StateMachine::get_context);
    ClassDB::bind_method(D_METHOD("set_context", "context"), &StateMachine::set_context);

    ClassDB::bind_method(D_METHOD("add_transition_between", "from_state", "to_state"), &StateMachine::add_transition_between);
    ClassDB::bind_method(D_METHOD("remove_transition", "transition"), &StateMachine::remove_transition);
    ClassDB::bind_method(D_METHOD("get_transitions_from", "state"), &StateMachine::get_transitions_from);
    ClassDB::bind_method(D_METHOD("get_transitions_to", "state"), &StateMachine::get_transitions_to);
    ClassDB::bind_method(D_METHOD("get_transition_between", "from_state", "to_state"), &StateMachine::get_transition_between);
    ClassDB::bind_method(D_METHOD("get_all_transitions"), &StateMachine::get_all_transitions);

    ClassDB::bind_method(D_METHOD("is_running"), &StateMachine::is_running);
    ClassDB::bind_method(D_METHOD("start", "state", "state_input"), &StateMachine::start);
    ClassDB::bind_method(D_METHOD("transition_to", "state", "state_input"), &StateMachine::transition_to, DEFVAL(Ref<StateInput>()));
    ClassDB::bind_method(D_METHOD("stop"), &StateMachine::stop);

    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "auto_start"), "set_auto_start", "will_auto_start");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "_states", PROPERTY_HINT_ARRAY_TYPE, "State", PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_INTERNAL), "_set_all_states", "get_all_states");
    ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "_default_state", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_INTERNAL), "_set_default_state", "_get_default_state");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "default_state", PROPERTY_HINT_RESOURCE_TYPE, "State", PROPERTY_USAGE_NONE), "set_default_state", "get_default_state");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "active_state", PROPERTY_HINT_RESOURCE_TYPE, "State", PROPERTY_USAGE_NONE), "", "get_active_state");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_NODE_TYPE, "", PROPERTY_USAGE_DEFAULT, "Node"), "set_context", "get_context");

    GDVIRTUAL_BIND(_start, "state", "state_input");
    GDVIRTUAL_BIND(_transition, "state", "state_input");
    GDVIRTUAL_BIND(_stop);

    ADD_SIGNAL(MethodInfo("state_added",
        PropertyInfo(Variant::OBJECT, "state", PROPERTY_HINT_RESOURCE_TYPE, "State")));
    ADD_SIGNAL(MethodInfo("state_removed",
        PropertyInfo(Variant::OBJECT, "state", PROPERTY_HINT_RESOURCE_TYPE, "State")));
    ADD_SIGNAL(MethodInfo("default_changed", 
        PropertyInfo(Variant::OBJECT, "state", PROPERTY_HINT_RESOURCE_TYPE, "State")));
    ADD_SIGNAL(MethodInfo("context_changed",
        PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_NODE_TYPE, "", PROPERTY_USAGE_DEFAULT, "Node")));

    ADD_SIGNAL(MethodInfo("started", 
        PropertyInfo(Variant::OBJECT, "starting_state", PROPERTY_HINT_RESOURCE_TYPE, "State"), 
        PropertyInfo(Variant::OBJECT, "starting_input", PROPERTY_HINT_RESOURCE_TYPE, "StateInput")));
    ADD_SIGNAL(MethodInfo("transitioned", 
        PropertyInfo(Variant::OBJECT, "outgoing_state", PROPERTY_HINT_RESOURCE_TYPE, "State"), 
        PropertyInfo(Variant::OBJECT, "incoming_state", PROPERTY_HINT_RESOURCE_TYPE, "State"), 
        PropertyInfo(Variant::OBJECT, "state_input", PROPERTY_HINT_RESOURCE_TYPE, "StateInput")));
    ADD_SIGNAL(MethodInfo("stopped", 
        PropertyInfo(Variant::OBJECT, "stopping_state", PROPERTY_HINT_RESOURCE_TYPE, "State")));
    
}