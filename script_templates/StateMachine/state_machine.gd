# meta-name: State Machine
# meta-description: StateMachine starting point with virtual methods templated.
# meta-default: true
# meta-space-indent: 4

extends _BASE_


func _start(starting_state: State, input: StateInput) -> void:
	pass


func _transition(next_state: State, input: StateInput) -> bool:
	return true


func _stop() -> void:
	pass
