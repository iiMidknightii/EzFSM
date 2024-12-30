@tool extends GraphNode

signal rebuild_requested

const StateIcon := preload("res://addons/EzFsm/icons/State.svg")
const DefaultStateIcon := preload("res://addons/EzFsm/icons/DefaultState.svg")
const SelfConnectIcon := preload("res://addons/EzFsm/icons/SelfConnect.svg")

const ScriptAttacher := preload("res://addons/EzFsm/scripts/script_attacher.gd")
const TransitionEditor := preload("res://addons/EzFsm/scripts/transition_editor.gd")
const TransitionEditorScene := preload("res://addons/EzFsm/scenes/transition_editor.tscn")

const DefaultColor = Color.INDIAN_RED

@export var state: State: set=set_state

var _attacher: ScriptAttacher

@onready var _connection_port: Control = $ConnectionPort
@onready var _default_button: Button = $ConnectionPort/DefaultButton
@onready var _disabled_button: Button = $ConnectionPort/DisabledButton
@onready var _self_button: Button = $ConnectionPort/SelfConnectButton
@onready var _color_button: Button = $ConnectionPort/ColorButton
@onready var _color_picker: ColorPicker = $ColorPicker
@onready var _transitions: Array[TransitionEditor]

var _color_time := 0.5

func _init() -> void:
	var hbox := get_titlebar_hbox()

	var label: Label = hbox.get_child(0)
	label.add_theme_color_override("font_outline_color", Color.BLACK)
	label.add_theme_constant_override("outline_size", 4)
	label.add_theme_font_size_override("font_size", 24)

	var spacer := Control.new()
	spacer.mouse_filter = Control.MOUSE_FILTER_PASS
	spacer.custom_minimum_size = Vector2(24.0, 0.0)

	hbox.add_child(spacer)

	_attacher = ScriptAttacher.new()
	_attacher.base_class = "State"
	hbox.add_child(_attacher)


func _ready() -> void:
	_default_button.pressed.connect(_on_default_requested)
	_disabled_button.pressed.connect(_on_disabled_button_pressed)
	_self_button.pressed.connect(_on_self_button_pressed)
	_color_button.toggled.connect(_on_color_button_toggled)
	_color_picker.color_changed.connect(_on_color_changed)

	var settings := EditorInterface.get_editor_settings()
	_color_picker.color_mode = settings.get_setting("interface/inspector/default_color_picker_mode")
	_color_picker.picker_shape = settings.get_setting("interface/inspector/default_color_picker_shape")


func _process(delta: float) -> void:
	var color: Color
	if state:
		color = state._node_color
	else:
		color = Color.BLACK

	get_theme_stylebox("titlebar").bg_color = color
	get_theme_stylebox("titlebar_selected").bg_color = color


func _get_minimum_size() -> Vector2:
	return Vector2(_color_picker.size.x, 0.0)


func set_state(new_state: State) -> void:
	if not is_node_ready():
		await ready

	if state:
		state.changed.disconnect(_on_state_changed)

	state = new_state
	_attacher.object = state

	if state:
		state.changed.connect(_on_state_changed)
		var color = state._node_color
		color.a = 1.0
		_color_picker.color = color
		_color_button.modulate = color
	_on_state_changed()


func set_default(def: bool) -> void:
	_default_button.button_pressed = def


func add_transition(transition: StateTransition) -> void:
	var t_editor := TransitionEditorScene.instantiate()
	t_editor.transition = transition
	add_child(t_editor)
	move_child(t_editor, -3)
	move_child(_connection_port, -2)
	move_child(_color_picker, -1)
	set_slot(t_editor.get_index(), false, 0, Color.WHITE, true, 0, Color.SLATE_BLUE)
	set_slot(get_new_transition_port(), true, 0, Color.WHITE, true, 0, Color.WHITE)
	clear_slot(_color_picker.get_index())
	_transitions.append(t_editor)
	t_editor.rebuild_requested.connect(rebuild_requested.emit)


func remove_transition(transition: StateTransition) -> void:
	var t_editor := _get_editor_for_transition(transition)
	_transitions.erase(t_editor)
	clear_slot(t_editor.get_index())
	remove_child(t_editor)
	t_editor.queue_free()


func get_transition_port(transition: StateTransition) -> int:
	return _get_editor_for_transition(transition).get_index()


func get_new_transition_port() -> int:
	return _connection_port.get_index()


func _get_editor_for_transition(transition: StateTransition) -> TransitionEditor:
	for t_editor: TransitionEditor in _transitions:
		if (t_editor.transition == transition):
			return t_editor

	return null


func _on_color_changed(color: Color) -> void:
	color.a = 1.0
	_color_button.modulate = color

	color.a = 0.9
	if state:
		state._node_color = color


func _on_color_button_toggled(toggle: bool) -> void:
	_color_picker.visible = toggle
	size = Vector2.ZERO

func _on_default_requested() -> void:
	if state:
		state.get_state_machine().default_state = state
	rebuild_requested.emit()


func _on_disabled_button_pressed() -> void:
	if state:
		state.enabled = not _disabled_button.button_pressed


func _on_self_button_pressed() -> void:
	if state:
		state.transitions_to_self = _self_button.button_pressed


func _on_state_changed() -> void:
	if not is_node_ready():
		await ready

	if state:
		if state.state_name.is_empty():
			title = "State1"
		else:
			title = state.state_name

		_disabled_button.button_pressed = not state.enabled
		_self_button.button_pressed = state.transitions_to_self

	else:
		title = ""

		_default_button.button_pressed = false
		_disabled_button.button_pressed = false
		_self_button.button_pressed = false
