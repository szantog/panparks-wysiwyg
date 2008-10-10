// $Id$

Drupal.wysiwyg = Drupal.wysiwyg || { 'init': {}, 'attach': {}, 'detach': {} };

/**
 * Attach this editor to a target element.
 *
 * See Drupal.wysiwyg.attach.none() for a full desciption of this hook.
 */
Drupal.wysiwyg.attach.fckeditor = function(context, params, settings) {
  // @todo Convert string into variable name w/o overwriting string?
  //   workaround: build object via editors[this.id] = new ...
  var FCKinstance = new FCKeditor(params.field);
  // Configure settings for this theme.
  FCKinstance.BasePath = settings[params.theme].BasePath;
  for (var setting in settings[params.theme]) {
    FCKinstance.Config[setting] = settings[params.theme][setting];
  }
  // Attach editor control if default is on.
  if (Drupal.settings.wysiwygEditor.status) {
    FCKinstance.ReplaceTextarea();
  }
}

/**
 * Detach a single or all editors.
 *
 * See Drupal.wysiwyg.detach.none() for a full desciption of this hook.
 */
Drupal.wysiwyg.detach.fckeditor = function(context, params) {
  if (typeof params != 'undefined') {
    var editor = FCKeditorAPI.GetInstance(params.field);
    if (editor) {
      $('#' + params.field).val(editor.GetXHTML()).show();
      $('#' + params.field + '___Config').remove();
      $('#' + params.field + '___Frame').remove();
      delete FCKeditorAPI.__Instances[params.field];
    }
  }
//  else {
//    tinyMCE.triggerSave();
//    tinyMCE.remove();
//  }
}

