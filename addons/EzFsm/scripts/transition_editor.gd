@tool extends HBoxContainer

signal rebuild_requested

const ScriptAttacher := preload("res://addons/EzFsm/scripts/script_attacher.gd")
const StateTransitionIcon := preload("res://addons/EzFsm/icons/StateTransition.svg")
const StateTransitionWarningIcon := preload("res://addons/EzFsm/icons/StateTransitionWarning.svg")

@export var transition: StateTransition: set=set_transition

@onready var _label_button: Button = $LabelButton
@onready var _attacher: ScriptAttacher = $ScriptAttacher
@onready var _move_up_button: Button = $VBoxContainer/MoveUpButton
@onready var _move_down_button: Button = $VBoxContainer/MoveDownButton

func _ready() -> void:
	_attacher.base_class = "StateTransition"
	_label_button.pressed.connect(_on_label_button_pressed)
	_move_up_button.pressed.connect(_on_move_button_pressed.bind(-1))
	_move_down_button.pressed.connect(_on_move_button_pressed.bind(1))


func _process(delta: float) -> void:
	if transition:
		var is_warn := false
		var tooltip: Array[String] = []
		if not transition.get_script():
			is_warn = true
			tooltip.push_back("No script attached, no transition will occur.")

		if not transition.from_state:
			is_warn = true
			tooltip.push_back("No state to transition from.")

		if not transition.to_state:
			is_warn = true
			tooltip.push_back("No state to transition to.")
			_label_button.text = "..."
		else:
			_label_button.text = transition.to_state.state_name

		var has_both := transition.from_state and transition.to_state
		if has_both and transition.from_state == transition.to_state and \
				not transition.from_state.transitions_to_self:
			is_warn = true
			tooltip.push_back("Transition is from a state to itself when that state disallows self transitions.")

		if has_both and not is_warn:
			tooltip.push_back("Transition logic between %s and %s" % [
				transition.from_state.state_name, transition.to_state.state_name])

		if is_warn:
			_label_button.icon = StateTransitionWarningIcon
		else:
			_label_button.icon = StateTransitionIcon

		_label_button.tooltip_text = String("\n").join(tooltip)


func set_transition(new_transition: StateTransition) -> void:
	if transition == new_transition:
		return

	transition = new_transition

	if not is_node_ready():
		await ready

	_attacher.object = transition

	if transition and transition.from_state:
		var idx := transition.from_state.get_transition_priority(transition)
		_move_up_button.visible = idx != 0
		_move_down_button.visible = idx != transition.from_state.get_all_transitions().size() - 1



func _on_label_button_pressed() -> void:
	EditorInterface.inspect_object(transition)


func _on_move_button_pressed(by: int) -> void:
	if transition and transition.from_state:
		var prio := transition.from_state.get_transition_priority(transition)
		prio = clampi(prio + by, 0, transition.from_state.get_all_transitions().size() - 1)
		transition.from_state.move_transition_priority(transition, prio)
		rebuild_requested.emit()
