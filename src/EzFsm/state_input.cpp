#include "state_input.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/gdvirtual.gen.inc>

using namespace godot;
using namespace godot::ez_fsm;

StringName StateInput::get_previous_state() const {
    return previous_state;
}

void StateInput::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_previous_state"), &StateInput::get_previous_state);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "previous_state", PROPERTY_HINT_RESOURCE_TYPE, "State", PROPERTY_USAGE_NONE), "", "get_previous_state");
}