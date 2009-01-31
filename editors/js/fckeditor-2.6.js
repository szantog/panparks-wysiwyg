// $Id$

/**
 * Attach this editor to a target element.
 */
Drupal.wysiwyg.editor.attach.fckeditor = function(context, params, settings) {
  var FCKinstance = new FCKeditor(params.field, settings['Width'], settings['Height']);
  // Apply editor instance settings.
  FCKinstance.BasePath = settings.EditorPath;
  // Apply 'Wysiwyg' toolbar, if defined.
  if (settings.buttons) {
    FCKinstance.ToolbarSet = settings.ToolbarSet;
  }

  // Apply input format configuration.
  FCKinstance.Config.format = params.format;
  delete settings.buttons;
  for (var setting in settings) {
    FCKinstance.Config[setting] = settings[setting];
  }
  // Attach editor.
  FCKinstance.ReplaceTextarea();
};

/**
 * Detach a single or all editors.
 */
Drupal.wysiwyg.editor.detach.fckeditor = function(context, params) {
  if (typeof params != 'undefined' && typeof FCKeditorAPI != 'undefined') {
    var editor = FCKeditorAPI.GetInstance(params.field);
    if (editor) {
      $('#' + params.field).val(editor.GetXHTML()).show();
      $('#' + params.field + '___Config').remove();
      $('#' + params.field + '___Frame').remove();
      delete FCKeditorAPI.__Instances[params.field];
    }
  }
  else {
    for (instance in FCKeditorAPI.__Instances) {
      $('#' + instance).val(editor.GetXHTML()).show();
      $('#' + instance + '___Config').remove();
      $('#' + instance + '___Frame').remove();
      delete FCKeditorAPI.__Instances[instance];
    }
  }
};

