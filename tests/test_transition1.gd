extends StateTransition

func _process(delta: float) -> bool:
	if context:
		if context.count >= 100:
			return true
		else:
			return false
	else:
		return false
