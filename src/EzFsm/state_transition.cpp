#include <godot_cpp/variant/utility_functions.hpp>
#include "state_transition.hpp"
#include "state.hpp"
#include "state_machine.hpp"

using namespace godot;
using namespace ez_fsm;

void StateTransition::_set_from_state(Ref<State> p_state) {
    if (p_state != from_state) {
        from_state = p_state;
        emit_changed();
    }
}

Ref<State> StateTransition::get_from_state() const {
    return from_state;
}

void StateTransition::set_to_state(Ref<State> p_state) {
    StringName new_name;
    if (p_state.is_valid()) {
        new_name = p_state->get_state_name();
    }

    _set_to_state(new_name);
}

void StateTransition::_set_to_state(StringName p_name) {
    if (p_name != to_state_name) {
        to_state_name = p_name;
        emit_changed();
    }
}

Ref<State> StateTransition::get_to_state() const {
    StateMachine *machine = get_state_machine();
    if (nullptr == machine) {
        return Ref<State>();
    }

    return machine->get_state(to_state_name);
}

StringName StateTransition::_get_to_state() const {
    return to_state_name;
}


void StateTransition::set_state_input(Ref<StateInput> p_state_input) {
    if (p_state_input != input) {
        input = p_state_input;
        emit_changed();
    }
}

void StateTransition::set_context(Node *p_context) {
    StateMachine *machine = get_state_machine();

    if (nullptr == machine) {
        return;
    }

    machine->set_context(p_context);
}

Node *StateTransition::get_context() const {
    StateMachine *machine = get_state_machine();

    if (nullptr == machine) {
        return nullptr;
    } else {
        return machine->get_context();
    }
}

StateMachine *StateTransition::get_state_machine() const {
    StateMachine *machine = nullptr;

    if (from_state.is_valid()) {
        machine = from_state->get_state_machine();
    }

    return machine;
}

bool StateTransition::request_transition() {
    StateMachine *machine = get_state_machine();

    if (nullptr == machine) {
        return false;
    } else {
        return machine->transition_to(to_state_name, input);
    }
}

void StateTransition::_validate_property(PropertyInfo &p_prop) const {
    if (
            p_prop.name == StringName("_from_state") || 
            p_prop.name == StringName("_to_state")
    ) {
        StateMachine *machine = get_state_machine();

        PropertyHint hint = PROPERTY_HINT_NONE;
        String hint_string;
        uint64_t usage = PROPERTY_USAGE_INTERNAL;

        if (nullptr != machine) {
            hint = PROPERTY_HINT_ENUM;
            hint_string = String(",").join(machine->get_all_state_names());
            usage |= PROPERTY_USAGE_DEFAULT;
        }

        p_prop.hint = hint;
        p_prop.hint_string = hint_string;
        p_prop.usage = usage;
        p_prop.hint = hint;
        p_prop.hint_string = hint_string;
        p_prop.usage = usage;
    }
}

void StateTransition::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_set_to_state", "state_name"), &StateTransition::_set_to_state);
    ClassDB::bind_method(D_METHOD("_get_to_state"), &StateTransition::_get_to_state);

    ClassDB::bind_method(D_METHOD("get_from_state"), &StateTransition::get_from_state);
    ClassDB::bind_method(D_METHOD("set_to_state", "state"), &StateTransition::set_to_state);
    ClassDB::bind_method(D_METHOD("get_to_state"), &StateTransition::get_to_state);

    ClassDB::bind_method(D_METHOD("set_state_input", "state_input"), &StateTransition::set_state_input);

    ClassDB::bind_method(D_METHOD("set_context", "context"), &StateTransition::set_context);
    ClassDB::bind_method(D_METHOD("get_context"), &StateTransition::get_context);

    ClassDB::bind_method(D_METHOD("get_state_machine"), &StateTransition::get_state_machine);
    ClassDB::bind_method(D_METHOD("request_transition"), &StateTransition::request_transition);

    GDVIRTUAL_BIND(_process, "delta");
    GDVIRTUAL_BIND(_physics_process, "delta");
    GDVIRTUAL_BIND(_input, "input_event");
    GDVIRTUAL_BIND(_unhandled_input, "input_event");
    GDVIRTUAL_BIND(_shortcut_input, "input_event");
    GDVIRTUAL_BIND(_unhandled_key_input, "input_event");

    ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "_to_state", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_INTERNAL), "_set_to_state", "_get_to_state");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "from_state", PROPERTY_HINT_RESOURCE_TYPE, "State", PROPERTY_USAGE_NONE), "", "get_from_state");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "to_state", PROPERTY_HINT_RESOURCE_TYPE, "State", PROPERTY_USAGE_NONE), "set_to_state", "get_to_state");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "context", PROPERTY_HINT_NODE_TYPE, "", PROPERTY_USAGE_NONE, "Node"), "set_context", "get_context");
}