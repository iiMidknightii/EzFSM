@tool extends GraphEdit

const StateIcon := preload("res://addons/EzFsm/icons/State.svg")
const StateEditorScene := preload("res://addons/EzFsm/scenes/state_editor.tscn")
const StateEditor := preload("res://addons/EzFsm/scripts/state_editor.gd")

var machine: StateMachine: set=set_machine

var _clipboard: Array[StringName]

func _init() -> void:
	var hbox: HBoxContainer = get_menu_hbox()

	var add_state_button := Button.new()
	hbox.add_child(add_state_button)
	hbox.move_child(add_state_button, 0)
	add_state_button.icon = StateIcon
	add_state_button.tooltip_text = "Add State"
	add_state_button.pressed.connect(_on_add_state_button_pressed)

	add_valid_left_disconnect_type(0)

	node_selected.connect(_on_node_selected)
	node_deselected.connect(_on_node_deselected)
	connection_request.connect(_on_connection_request)
	disconnection_request.connect(_on_disconnection_request)
	copy_nodes_request.connect(_on_copy_nodes_request)
	paste_nodes_request.connect(_on_paste_nodes_request)
	delete_nodes_request.connect(_on_delete_nodes_request)
	popup_request.connect(_on_popup_request)
	end_node_move.connect(_on_end_node_move)


func _is_node_hover_valid(from_node: StringName, from_port: int, to_node: StringName, to_port: int) -> bool:
	var from: StateEditor = get_node(NodePath(from_node))
	var to: StateEditor = get_node(NodePath(to_node))

	if not from.state or not to.state:
		return false

	if from_port != from.get_new_transition_port():
		return false

	if machine.get_transition_between_states(from.state, to.state):
		return false

	if from.state == to.state and not from.state.transitions_to_self:
		return false

	return true


func set_machine(new_machine: StateMachine) -> void:
	if machine != new_machine:
		if machine:
			machine.default_changed.disconnect(_on_default_changed)
		machine = new_machine
		if machine:
			machine.default_changed.connect(_on_default_changed)
		_build()


func _get_editor_for_state(state: State) -> StateEditor:
	for child: Node in get_children():
		if child is StateEditor and child.state == state:
			return child

	return null


func _build() -> void:
	clear_connections()
	for child: Node in get_children():
		if (child is GraphElement):
			remove_child(child)
			child.queue_free()

	if not machine:
		return

	for state: State in machine.get_all_states():
		var s_editor := StateEditorScene.instantiate()
		s_editor.state = state
		add_child(s_editor)
		s_editor.position_offset = state.node_position
		s_editor.set_default(state == machine.default_state)
		s_editor.default_requested.connect(machine.set_default_state)

	for transition: StateTransition in machine.get_all_transitions():
		if not transition.from_state:
			machine.remove_transition(transition)
			continue

		var f_editor := _get_editor_for_state(transition.from_state)
		f_editor.add_transition(transition)
		var f_idx := f_editor.get_transition_port(transition)

		if transition.to_state:
			var t_editor := _get_editor_for_state(transition.to_state)
			connect_node(f_editor.name, f_idx, t_editor.name, 0)


func _on_node_selected(node: Node) -> void:
	if node is StateEditor and node.state:
		EditorInterface.inspect_object(node.state)


func _on_node_deselected(node: Node) -> void:
	if machine:
		EditorInterface.inspect_object(machine)


func _on_connection_request(from_node: StringName, from_port: int, to_node: StringName, to_port: int) -> void:
	var from: StateEditor = get_node(NodePath(from_node))
	var to: StateEditor= get_node(NodePath(to_node))

	if not from.state or not to.state:
		return

	var new_transition := machine.add_transition(from.state, to.state)
	from.add_transition(new_transition)
	connect_node(from_node, from.get_transition_port(new_transition), to_node, to_port)


func _on_disconnection_request(from_node: StringName, from_port: int, to_node: StringName, to_port: int) -> void:
	var from: StateEditor = get_node(NodePath(from_node))
	var to: StateEditor = get_node(NodePath(to_node))

	disconnect_node(from_node, from_port, to_node, to_port)

	if not from.state or not to.state:
		return

	var transition := machine.get_transition_between_states(from.state, to.state)
	if transition:
		machine.remove_transition(transition)
		from.remove_transition(transition)


func _on_copy_nodes_request() -> void:
	_clipboard.clear()
	for child: Node in get_children():
		if child is StateEditor:
			_clipboard.append(child.name)


func _on_paste_nodes_request() -> void:
	if not machine:
		return

	for child: StringName in _clipboard:
		var node: Node = get_node(NodePath(child))
		if node is StateEditor and node.state:
			var new_state: State = machine.add_state(node.state.state_name)
			new_state.node_position = node.position_offset + Vector2(node.size.x + 4, 4)

	_build()


func _on_delete_nodes_request(nodes: Array[StringName]) -> void:
	if not machine:
		return

	for child: StringName in nodes:
		var node: Node = get_node(NodePath(child))
		if node is StateEditor:
			if node.state:
				var transitions: Array[StateTransition]
				if node.state:
					transitions.append_array(machine.get_transitions_from_state(node.state))
					transitions.append_array(machine.get_transitions_to_state(node.state))

					machine.remove_state(node.state)

				for transition: StateTransition in transitions:
					machine.remove_transition(transition)

			_build()


func _on_popup_request(at_position: Vector2) -> void:
	var menu := PopupMenu.new()
	menu.add_icon_item(StateIcon, "Add State", 0)

	menu.index_pressed.connect(func(index: int) -> void:
		match index:
			0:
				_on_add_state_button_pressed((scroll_offset + at_position) / zoom)
		menu.queue_free()
	)

	add_child(menu)
	menu.popup_on_parent(Rect2(global_position + at_position, Vector2.ZERO))


func _on_end_node_move() -> void:
	for node: Node in get_children():
		if node is StateEditor and node.state:
			node.state.node_position = node.position_offset


func _on_add_state_button_pressed(at_position := Vector2.INF) -> void:
	if not machine:
		return

	var state := machine.add_state(&"NewState")
	if at_position.is_finite():
		state.node_position = at_position
	else:
		state.node_position = scroll_offset/zoom + size/2.0/zoom

	_build()

	var editor := _get_editor_for_state(state)
	editor.position_offset -= editor.size/2.0/zoom


func _on_default_changed(state: State) -> void:
	var editor := _get_editor_for_state(state)
	for child: Node in get_children():
		if child is StateEditor:
			child.set_default(child.state == state)
