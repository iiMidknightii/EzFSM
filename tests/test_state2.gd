extends State


func _active_physics_process(_delta: float) -> void:
	if context:
		context.count -= 1
