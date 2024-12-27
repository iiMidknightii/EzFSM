extends State


func _active_physics_process(delta: float) -> void:
	if context:
		context.count -= 1
