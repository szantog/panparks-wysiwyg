// $Id$

Drupal.wysiwyg = Drupal.wysiwyg || { 'init': {}, 'attach': {}, 'detach': {} };

/**
 * Initialize editor instances.
 *
 * This function needs to be called before the page is fully loaded, as
 * calling tinyMCE.init() after the page is loaded breaks IE6.
 *
 * @param editorSettings
 *   An object containing editor settings for each enabled editor theme.
 */
Drupal.wysiwyg.init.tinymce = function(editorSettings) {
  // If JS compression is enabled, TinyMCE is unable to find its own base path
  // and exec mode, hence we need to define it manually.
  // @todo Move global library settings somewhere else.
  tinyMCE.baseURL = Drupal.settings.wysiwygEditor.editorBasePath;
  tinyMCE.srcMode = (Drupal.settings.wysiwygEditor.execMode == 'src' ? '_src' : '');
  tinyMCE.gzipMode = (Drupal.settings.wysiwygEditor.execMode == 'gzip');

  for (var theme in editorSettings) {
    // Clone, so original settings are not overwritten.
    var config = Drupal.wysiwyg.clone(editorSettings[theme]);
    tinyMCE.init(config);
  }
  // @todo Move into global library settings.
  for (var plugin in Drupal.settings.wysiwygEditor.plugins.tinymce) {
    tinyMCE.loadPlugin(plugin, Drupal.settings.wysiwygEditor.plugins.tinymce[plugin]);
  }
}

/**
 * Attach this editor to a target element.
 *
 * See Drupal.wysiwyg.attach.none() for a full desciption of this hook.
 */
Drupal.wysiwyg.attach.tinymce = function(context, params, editorSettings) {
  // Configure settings for this theme.
  for (var setting in editorSettings[params.theme]) {
    tinyMCE.settings[setting] = editorSettings[params.theme][setting];
  }
  // Attach editor control if default is on.
  if (Drupal.settings.wysiwygEditor.status) {
    tinyMCE.execCommand('mceAddControl', true, params.field);
  }
}

/**
 * Detach a single or all editors.
 *
 * See Drupal.wysiwyg.detach.none() for a full desciption of this hook.
 */
Drupal.wysiwyg.detach.tinymce = function(context, params) {
  if (typeof params != 'undefined') {
    tinyMCE.removeMCEControl(tinyMCE.getEditorId(params.field));
    $('#' + params.field).removeAttr('style');
  }
//  else if (tinyMCE.activeEditor) {
//    tinyMCE.triggerSave();
//    tinyMCE.activeEditor.remove();
//  }
}

