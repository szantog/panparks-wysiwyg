// $Id$

Drupal.wysiwyg = Drupal.wysiwyg || { 'init': {}, 'attach': {}, 'detach': {}, 'toggle': {} };

/**
 * Initialize TinyMCE instances.
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
  if (tinyMCE.getEditorId(element.id) == null) {
    // Clone, so original settings are not overwritten.
    var config = Drupal.wysiwyg.clone(Drupal.settings.wysiwygEditor.configs.tinymce[theme]);
    // Set configuration options for this theme.
    for (var setting in config) {
      tinyMCE.settings[setting] = config[setting];
    }
    tinyMCE.addMCEControl(element, element.id);
    return true;
  }
  else {
    tinyMCE.removeMCEControl(tinyMCE.getEditorId(element.id));
    return false;
  }
}

