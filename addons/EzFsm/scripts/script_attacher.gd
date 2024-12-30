@tool extends HBoxContainer

const ScriptIcon := preload("res://addons/EzFsm/icons/Script.svg")
const ScriptExtendIcon := preload("res://addons/EzFsm/icons/ScriptExtend.svg")
const ScriptCreateIcon := preload("res://addons/EzFsm/icons/ScriptCreate.svg")
const ScriptRemoveIcon := preload("res://addons/EzFsm/icons/ScriptRemove.svg")

var object: Object: set=set_object
var base_class: String

var _script_button: Button
var _script_extend_button: Button
var _script_remove_button: Button

func _init() -> void:
	_script_button = Button.new()
	_script_button.pressed.connect(_on_script_button_pressed)
	add_child(_script_button)
	_script_button.size_flags_horizontal = Control.SIZE_SHRINK_END
	_script_button.icon = ScriptIcon
	_script_button.flat = true

	_script_extend_button = Button.new()
	_script_extend_button.pressed.connect(_on_script_extend_button_pressed)
	add_child(_script_extend_button)
	_script_extend_button.size_flags_horizontal = Control.SIZE_SHRINK_END
	_script_extend_button.icon = ScriptExtendIcon
	_script_extend_button.flat = true

	_script_remove_button = Button.new()
	_script_remove_button.pressed.connect(_on_script_remove_button_pressed)
	add_child(_script_remove_button)
	_script_remove_button.size_flags_horizontal = Control.SIZE_SHRINK_END
	_script_remove_button.icon = ScriptRemoveIcon
	_script_remove_button.flat = true


func set_object(new_object: Object) -> void:
	if object:
		object.script_changed.disconnect(_update_script_state)

	object = new_object

	if object:
		object.script_changed.connect(_update_script_state)

	_update_script_state()


func _update_script_state() -> void:
	if object and object.get_script():
		_script_button.show()
		_script_button.icon = ScriptIcon
		var script: Script = object.get_script()
		_script_button.tooltip_text = "Open Script: %s" % [object.get_script().resource_path]

		_script_extend_button.show()
		_script_extend_button.tooltip_text = "Extend the script on the selected %s." % [base_class]
		_script_remove_button.show()
		_script_remove_button.tooltip_text = "Detach the script from the selected %s." % [base_class]

	elif object:
		_script_button.show()
		_script_button.icon = ScriptCreateIcon
		_script_button.tooltip_text = "Attach a new or existing script to the selected %s." % [base_class]

		_script_extend_button.hide()
		_script_remove_button.hide()

	else:
		_script_button.hide()
		_script_extend_button.hide()
		_script_remove_button.hide()


func _popup_script_dialog(base_name: String, base_path: String) -> void:
	base_path = EditorInterface.get_edited_scene_root().scene_file_path + base_path
	var dialog := ScriptCreateDialog.new()
	dialog.config(base_class, base_path)
	dialog.script_created.connect(_on_script_created)
	add_child(dialog)
	dialog.popup_centered()


func _on_script_button_pressed() -> void:
	if object and object.get_script():
		EditorInterface.edit_script(object.get_script())
	else:
		_popup_script_dialog(base_class, base_class.to_snake_case())


func _on_script_extend_button_pressed() -> void:
	if object and object.get_script():
		var base_script: Script = object.get_script()
		var base_name: String
		var base_path: String
		if base_script.get_global_name().is_empty():
			base_name = "\"" + base_script.resource_path + "\""
			base_path = "extended_" + base_class
		else:
			base_name = base_script.get_global_name()
			base_path = base_script.get_global_name().to_snake_case()

		_popup_script_dialog(base_name, base_path)


func _on_script_remove_button_pressed() -> void:
	if object:
		object.set_script(null)


func _on_script_created(script: Script) -> void:
	object.set_script(script)
	EditorInterface.edit_script(object.get_script())
