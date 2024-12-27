#include "state.hpp"
#include "state_machine.hpp"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>


using namespace godot;
using namespace godot::ez_fsm;

StringName State::get_state_name() const {
    return state_name;
}

void State::set_state_name(StringName p_name) {
    state_name = p_name;
    emit_changed();
}

bool State::is_enabled() const {
    return enabled;
}

void State::set_enabled(bool p_enabled) {
    enabled = p_enabled;
    emit_changed();
}

bool State::can_transition_to_self() const {
    return transitions_to_self;
}

void State::allow_transition_to_self(bool p_allow) {
    transitions_to_self = p_allow;
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
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_NODE_TYPE, "", PROPERTY_USAGE_NONE, "Node"), "set_context", "get_context");
    
    ADD_SIGNAL(MethodInfo("started", 
        PropertyInfo(Variant::OBJECT, "state_input", PROPERTY_HINT_RESOURCE_TYPE, "StateInput")));
    ADD_SIGNAL(MethodInfo("activated", 
        PropertyInfo(Variant::OBJECT, "state_input", PROPERTY_HINT_RESOURCE_TYPE, "StateInput")));
    ADD_SIGNAL(MethodInfo("deactivated"));
    ADD_SIGNAL(MethodInfo("stopped"));

#ifdef DEBUG_ENABLED
    ClassDB::bind_method(D_METHOD("set_node_color", "color"), &State::set_node_color);
    ClassDB::bind_method(D_METHOD("get_node_color"), &State::get_node_color);
    ClassDB::bind_method(D_METHOD("set_node_position", "pos"), &State::set_node_position);
    ClassDB::bind_method(D_METHOD("get_node_position"), &State::get_node_position);
    ADD_PROPERTY(PropertyInfo(Variant::COLOR, "node_color", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_INTERNAL), "set_node_color", "get_node_color");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "node_position", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_INTERNAL), "set_node_position", "get_node_position");
#endif
}