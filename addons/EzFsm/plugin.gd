@tool
extends EditorPlugin

const StateMachineEditorScene := preload("res://addons/EzFsm/scenes/state_machine_editor.tscn")
const StateMachineEditor := preload("res://addons/EzFsm/scripts/state_machine_editor.gd")

var _editor_button: Button
var _state_machine_editor: StateMachineEditor

func _enter_tree():
	_state_machine_editor = StateMachineEditorScene.instantiate()
	_editor_button = add_control_to_bottom_panel(_state_machine_editor, "StateMachine")
	_editor_button.hide()


func _exit_tree():
	remove_control_from_bottom_panel(_state_machine_editor)
	_state_machine_editor.queue_free()


func _clear() -> void:
	if _state_machine_editor:
		_state_machine_editor.machine = null


func _handles(object: Object) -> bool:
	return object is StateMachine


func _edit(object: Object) -> void:
	if _state_machine_editor and object is StateMachine:
		_state_machine_editor.machine = object
		_editor_button.button_pressed = true


func _make_visible(visible: bool) -> void:
	if (visible):
		_editor_button.show()
		make_bottom_panel_item_visible(_state_machine_editor)
	else:
		if _state_machine_editor.is_visible_in_tree():
			_state_machine_editor.hide()
			hide_bottom_panel()
		_editor_button.hide()
