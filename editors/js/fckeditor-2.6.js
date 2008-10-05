// $Id$

Drupal.wysiwyg = Drupal.wysiwyg || { 'init': {}, 'attach': {}, 'detach': {}, 'toggle': {} };

/**
 * Attach FCKeditor to textareas, using the theme specified in CSS class names.
 *
 * @param editorSettings
 *   An object containing editor settings for each enabled editor theme.
 */
Drupal.wysiwyg.attach.fckeditor = function(context, editorSettings) {
  for (var theme in editorSettings) {
    $('textarea.wysiwyg-' + theme + ':not(.wysiwyg-processed)', context).each(function() {
      // @todo Convert string into variable name w/o overwriting string?
      //   workaround: build object via editors[this.id] = new ...
      var oFCK_1 = new FCKeditor(this.id);
      // Clone, so original settings are not overwritten.
      var config = Drupal.wysiwyg.clone(editorSettings[theme]);
      // Configure settings for this theme.
      oFCK_1.BasePath = config.BasePath;
      for (var setting in config) {
        oFCK_1.Config[setting] = config[setting];
      }
      // Attach Wysiwyg Editor control if default is on.
      if (Drupal.settings.wysiwygEditor.status) {
        oFCK_1.ReplaceTextarea();
      }
      $(this).addClass('wysiwyg-processed');
    });
  }
}

/**
 * Detach all FCKeditor editors.
 *
 * @todo Context support required to remove only certain editors (think AHAH/AJAX).
 */
Drupal.wysiwyg.detach.fckeditor = function(context) {
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
Drupal.wysiwyg.toggle.fckeditor = function(element, theme) {
  var instance = FCKeditorAPI.GetInstance(element.id);
  if ($(element).css('display') != 'none' || instance == null) {
    instance.SetHTML($(element).hide().val());
    $('#' + element.id + '___Frame').show();
    return true;
  }
  else {
    $('#' + element.id).val(instance.GetXHTML()).show();
    $('#' + element.id + '___Frame').hide();
    return false;
  }
}

