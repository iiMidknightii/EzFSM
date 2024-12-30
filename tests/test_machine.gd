extends StateMachine


func _transition(state: State, _state_input: StateInput) -> bool:
	print("transitioning to ", state.state_name)
	return true
