# meta-name: State
# meta-description: State script starting point with all virtual method interfaces implemented.
# meta-default: true
# meta-space-indent: 4

extends _BASE_


func _start(input: StateInput) -> void:
	pass


func _can_activate(input: StateInput) -> bool:
    return true


func _activate(input: StateInput) -> void:
    pass


func _deactivate() -> void:
    pass


func _stop() -> void:
    pass


func _active_process(delta: float) -> void:
    pass


func _active_physics_process(delta: float) -> void:
    pass


func _active_input(event: InputEvent) -> void:
    pass


func _active_shortcut_input(event: InputEvent) -> void:
    pass


func _active_unhandled_input(event: InputEvent) -> void:
    pass


func _active_unhandled_key_input(event: InputEvent) -> void:
    pass


func _inactive_process(delta: float) -> void:
    pass


func _inactive_physics_process(delta: float) -> void:
    pass


func _inactive_input(event: InputEvent) -> void:
    pass


func _inactive_shortcut_input(event: InputEvent) -> void:
    pass


func _inactive_unhandled_input(event: InputEvent) -> void:
    pass


func _inactive_unhandled_key_input(event: InputEvent) -> void:
    pass


