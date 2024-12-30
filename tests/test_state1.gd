extends State


func _start(_input: StateInput) -> void:
	print(state_name, " started")


func _can_activate(input: StateInput) -> bool:
	print(state_name, " tried to activate with ", input)
	print("Has input meta: ", input.has_meta("TestMetaInput"), " from ", input.get_previous_state())
	return true


func _activate(_input: StateInput) -> void:
	print(state_name, " activated")


func _active_process(_delta: float) -> void:
	context.count += 1


func _inactive_process(_delta: float) -> void:
	if context.count % 25 == 0:
		print("test1 inactive process")


func _active_physics_process(_delta: float) -> void:
	if context.count % 20 == 0:
		print("test1 active physics")


func _inactive_physics_process(_delta: float) -> void:
	if context.count % 15 == 0:
		print("test 1 inactive physics")


func _active_input(event: InputEvent) -> void:
	print("test1 active input event %s" % [event])


func _inactive_input(event: InputEvent) -> void:
	print("test1 inactive input event %s" % [event])
