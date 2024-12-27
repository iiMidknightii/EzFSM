@tool extends HBoxContainer

const ScriptAttacher := preload("res://addons/EzFsm/scripts/script_attacher.gd")
const StateTransitionIcon := preload("res://addons/EzFsm/icons/StateTransition.svg")
const StateTransitionWarningIcon := preload("res://addons/EzFsm/icons/StateTransitionWarning.svg")

@export var transition: StateTransition: set=set_transition

@onready var _label_button: Button = $LabelButton
@onready var _attacher: ScriptAttacher = $ScriptAttacher

func _ready() -> void:
	_attacher.base_class = "StateTransition"
	_label_button.pressed.connect(_on_label_button_pressed)


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


func _on_label_button_pressed() -> void:
	EditorInterface.inspect_object(transition)
