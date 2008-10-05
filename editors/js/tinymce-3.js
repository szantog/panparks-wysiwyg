// $Id$

Drupal.wysiwyg = Drupal.wysiwyg || { 'init': {}, 'attach': {}, 'detach': {}, 'toggle': {} };

/**
 * Initialize TinyMCE instances.
 *
 * @todo Is the following note still valid for 3.x?
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
  // @todo Plugin architecture is completely different in 3.x.
//  for (var plugin in Drupal.settings.wysiwygEditor.plugins.tinymce) {
//    tinyMCE.loadPlugin(plugin, Drupal.settings.wysiwygEditor.plugins.tinymce[plugin]);
//  }
}

/**
 * Attach TinyMCE to textareas, using the theme specified in CSS class names.
 *
 * @param editorSettings
 *   An object containing editor settings for each enabled editor theme.
 */
Drupal.wysiwyg.attach.tinymce = function(context, editorSettings) {
  for (var theme in editorSettings) {
    // Clone, so original settings are not overwritten.
    var config = Drupal.wysiwyg.clone(editorSettings[theme]);
    // Configure settings for this theme.
    for (var setting in config) {
      tinyMCE.settings[setting] = config[setting];
    }
    $('textarea.wysiwyg-' + theme + ':not(.wysiwyg-processed)', context).each(function() {
      // Attach Wysiwyg Editor control if default is on.
      if (Drupal.settings.wysiwygEditor.status) {
        tinyMCE.execCommand('mceAddControl', true, this.id);
      }
      $(this).addClass('wysiwyg-processed');
    });
  }
}

/**
 * Detach all TinyMCE editors.
 *
 * @todo Context support required to remove only certain editors (think AHAH/AJAX).
 */
Drupal.wysiwyg.detach.tinymce = function(context) {
  if (tinyMCE.activeEditor) {
    tinyMCE.triggerSave();
    tinyMCE.activeEditor.remove();
  }
}

/**
 * Toggle editor and return new state.
 *
 * @param element
 *   The DOM element to toggle the editor for.
 * @param theme
 *   The editor theme assigned to the element.
 *
 * @return
 *   A boolean value indicating whether the editor has been enabled.
 */
Drupal.wysiwyg.toggle.tinymce = function(element, theme) {
  tinyMCE.execCommand('mceToggleEditor', false, element.id);
  return !(tinyMCE.get(element.id).isHidden());
}

