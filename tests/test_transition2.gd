extends StateTransition

func _physics_process(_delta: float) -> bool:
	if context:
		if context.count <= 0:
			var input := StateInput.new()
			input.set_meta("TestMetaInput", 42)
			set_state_input(input)
			return true
		else:
			return false
	else:
		return false
