#include "state.hpp"
#include "state_transition.hpp"
#include "state_machine.hpp"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>


using namespace godot;
using namespace godot::ez_fsm;

StringName State::get_state_name() const {
    return state_name;
}

void State::set_state_name(StringName p_name) {
    if (p_name == state_name) {
        return;
    }

    if (nullptr != machine) {
        p_name = machine->increment_state_name(p_name);
    }
    state_name = p_name;
    emit_changed();
}

bool State::is_enabled() const {
    return enabled;
}

void State::set_enabled(bool p_enabled) {
    if (p_enabled != enabled) {
        enabled = p_enabled;
        emit_changed();
    }
}

bool State::can_transition_to_self() const {
    return transitions_to_self;
}

void State::allow_transition_to_self(bool p_allow) {
    if (p_allow != transitions_to_self) {
        transitions_to_self = p_allow;
        emit_changed();
    }
}

Ref<StateTransition> State::add_transition_to(const Ref<State> &p_to) {
    ERR_FAIL_NULL_V(p_to, nullptr);

    Ref<StateTransition> transition = get_transition_to(p_to);
    if (transition.is_null()) {
        transition.instantiate();
        transition->_set_from_state(this);
        transition->_set_to_state(p_to->get_state_name());
        transitions.push_back(transition);
        emit_changed();
    }
    return transition;
}

Ref<StateTransition> State::get_transition_to(const Ref<State> &p_to) const {
    for (uint64_t idx = 0; idx < transitions.size(); ++idx) {
        Ref<StateTransition> transition = _get_transition(idx);
        if (transition->get_to_state() == p_to) {
            return transition;
        }
    }

    return Ref<StateTransition>();
}

TypedArray<StateTransition> State::get_all_transitions() const {
    return transitions;
}

void State::_set_all_transitions(TypedArray<StateTransition> p_transitions) {
    for (uint64_t idx = 0; idx < transitions.size(); ++idx) {
        Ref<StateTransition> transition = _get_transition(idx);
        transition->_set_from_state(nullptr);
    }
    transitions.clear();

    transitions = p_transitions;
    for (uint64_t idx = 0; idx < transitions.size(); ++idx) {
        Ref<StateTransition> transition = _get_transition(idx);
        transition->_set_from_state(this);
    }
}

int64_t State::get_transition_priority(Ref<StateTransition> p_transition) const {
    return transitions.find(p_transition);
}

void State::move_transition_priority(Ref<StateTransition> p_transition, uint64_t p_priority) {
    ERR_FAIL_COND(p_priority < 0 || p_priority >= transitions.size());
    ERR_FAIL_COND(!transitions.has(p_transition));
    int64_t idx = get_transition_priority(p_transition);
    if (idx != p_priority) {
        transitions.remove_at(idx);
        transitions.insert(p_priority, p_transition);
        emit_changed();
    }
}

void State::remove_transition(Ref<StateTransition> p_transition) {
    ERR_FAIL_COND(!transitions.has(p_transition));
    transitions.erase(p_transition);
    p_transition->_set_from_state(nullptr);
    emit_changed();
}

bool State::has_sibling(StringName const &p_name) const {
    if (nullptr == machine) {
        return false;
    } else {
        return machine->get_all_state_names().has(p_name);
    }
}

Ref<State> State::get_sibling(StringName const &p_name) const {
    if (nullptr == machine) {
        return nullptr;
    } else {
        return machine->get_state(p_name);
    }
}

TypedArray<State> State::get_all_siblings() const {
    TypedArray<State> out;

    if (nullptr != machine) {
        TypedArray<State> in = machine->get_all_states();
        out.resize(in.size() - 1);
        int k = 0;
        for (int idx = 0; idx < in.size(); ++idx) {
            if (cast_to<State>(in[idx]) != this) {
                out[k++] = in[idx];
            }
        }
    }

    return out;
}

void State::_set_state_machine(StateMachine *p_machine) {
    machine = p_machine;
}

StateMachine *State::get_state_machine() const {
    return machine;
}

Node *State::get_context() const {
    if (nullptr == machine) {
        return nullptr;
    } else {
        return machine->get_context();
    }
}

void State::set_context(Node *p_context) {
    if (nullptr == machine) {
        return;
    }
    
    machine->set_context(p_context);
}

Ref<StateTransition> State::_get_transition(uint64_t p_idx) const {
    ERR_FAIL_COND_V(p_idx < 0 || p_idx >= transitions.size(), Ref<StateTransition>());
    return Ref<StateTransition>(transitions[p_idx]);
}

#ifdef DEBUG_ENABLED
void State::set_node_color(Color p_node_color) {
    node_color = p_node_color;
}

Color State::get_node_color() const {
    return node_color;
}

void State::set_node_position(Vector2 p_node_position) {
    node_position = p_node_position;
}

Vector2 State::get_node_position() const {
    return node_position;
}
#endif

void State::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_state_name", "name"), &State::set_state_name);
    ClassDB::bind_method(D_METHOD("get_state_name"), &State::get_state_name);

    ClassDB::bind_method(D_METHOD("is_enabled"), &State::is_enabled);
    ClassDB::bind_method(D_METHOD("set_enabled", "enabled"), &State::set_enabled);

    ClassDB::bind_method(D_METHOD("can_transition_to_self"), &State::can_transition_to_self);
    ClassDB::bind_method(D_METHOD("allow_transition_to_self", "allow"), &State::allow_transition_to_self);

    ClassDB::bind_method(D_METHOD("add_transition_to", "to_state"), &State::add_transition_to);
    ClassDB::bind_method(D_METHOD("get_transition_to", "to_state"), &State::get_transition_to);
    ClassDB::bind_method(D_METHOD("_set_all_transitions", "transitions"), &State::_set_all_transitions);
    ClassDB::bind_method(D_METHOD("get_all_transitions"), &State::get_all_transitions);
    ClassDB::bind_method(D_METHOD("get_transition_priority", "transition"), &State::get_transition_priority);
    ClassDB::bind_method(D_METHOD("move_transition_priority", "transition", "priority"), &State::move_transition_priority);
    ClassDB::bind_method(D_METHOD("remove_transition", "transition"), &State::remove_transition);

    ClassDB::bind_method(D_METHOD("has_sibling", "sibling_name"), &State::has_sibling);
    ClassDB::bind_method(D_METHOD("get_sibling", "sibling_name"), &State::get_sibling);

    ClassDB::bind_method(D_METHOD("set_context", "context"), &State::set_context);
    ClassDB::bind_method(D_METHOD("get_context"), &State::get_context);

    ClassDB::bind_method(D_METHOD("get_state_machine"), &State::get_state_machine);

    GDVIRTUAL_BIND(_start, "input");
    GDVIRTUAL_BIND(_can_activate, "input");
    GDVIRTUAL_BIND(_activate, "input");
    GDVIRTUAL_BIND(_deactivate);
    GDVIRTUAL_BIND(_stop);

    GDVIRTUAL_BIND(_active_process, "delta");
    GDVIRTUAL_BIND(_active_physics_process, "delta");
    GDVIRTUAL_BIND(_active_input, "event");
    GDVIRTUAL_BIND(_active_shortcut_input, "event");
    GDVIRTUAL_BIND(_active_unhandled_input, "event");
    GDVIRTUAL_BIND(_active_unhandled_key_input, "event");

    GDVIRTUAL_BIND(_inactive_process, "delta");
    GDVIRTUAL_BIND(_inactive_physics_process, "delta");
    GDVIRTUAL_BIND(_inactive_input, "event");
    GDVIRTUAL_BIND(_inactive_shortcut_input, "event");
    GDVIRTUAL_BIND(_inactive_unhandled_input, "event");
    GDVIRTUAL_BIND(_inactive_unhandled_key_input, "event");

    ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "state_name"), "set_state_name", "get_state_name");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enabled"), "set_enabled", "is_enabled");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "transitions_to_self"), "allow_transition_to_self", "can_transition_to_self");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "transitions", PROPERTY_HINT_ARRAY_TYPE, "StateTranstition", PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_INTERNAL), "_set_all_transitions", "get_all_transitions");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_NODE_TYPE, "", PROPERTY_USAGE_NONE, "Node"), "set_context", "get_context");
    
    ADD_SIGNAL(MethodInfo("transition_added", 
        PropertyInfo(Variant::OBJECT, "transition", PROPERTY_HINT_RESOURCE_TYPE, "StateTransition")));
    ADD_SIGNAL(MethodInfo("transition_removed",
        PropertyInfo(Variant::OBJECT, "transition", PROPERTY_HINT_RESOURCE_TYPE, "StateTransition")));
    ADD_SIGNAL(MethodInfo("started", 
        PropertyInfo(Variant::OBJECT, "state_input", PROPERTY_HINT_RESOURCE_TYPE, "StateInput")));
    ADD_SIGNAL(MethodInfo("activated", 
        PropertyInfo(Variant::OBJECT, "state_input", PROPERTY_HINT_RESOURCE_TYPE, "StateInput")));
    ADD_SIGNAL(MethodInfo("deactivated"));
    ADD_SIGNAL(MethodInfo("stopped"));

#ifdef DEBUG_ENABLED
    ClassDB::bind_method(D_METHOD("_set_node_color", "color"), &State::set_node_color);
    ClassDB::bind_method(D_METHOD("_get_node_color"), &State::get_node_color);
    ClassDB::bind_method(D_METHOD("_set_node_position", "pos"), &State::set_node_position);
    ClassDB::bind_method(D_METHOD("_get_node_position"), &State::get_node_position);
    ADD_PROPERTY(PropertyInfo(Variant::COLOR, "_node_color", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_INTERNAL), "_set_node_color", "_get_node_color");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "_node_position", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_INTERNAL), "_set_node_position", "_get_node_position");
#endif
}