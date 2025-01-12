extends StateMachine


func _transition(state: State, _state_input: StateInput) -> bool:
	print(get_path(), " transitioning to ", state.state_name)
	return true
