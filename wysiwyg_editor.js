// $Id$

/**
 * Initialize each Wysiwyg Editor configuration.
 *
 * This function needs to be called before the page is fully loaded, as
 * calling tinyMCE.init() after the page is loaded breaks in IE 6.
 */
Drupal.wysiwygEditorInit = function () {
  for (var theme in Drupal.settings.wysiwygEditor.configs) {
    // Clone so we are not passing by reference. Otherwise the
    // settings will get overwritten.
    var config = Drupal.wysiwygEditorCloneObject(Drupal.settings.wysiwygEditor.configs[theme]);
    tinyMCE.init(config);
  }
  for (var plugin in Drupal.settings.wysiwygEditor.plugins) {
    tinyMCE.loadPlugin(plugin, Drupal.settings.wysiwygEditor.plugins[plugin]);
  }
}

/**
 * Attach Wysiwyg Editor to textareas.
 *
 * This function can be called to process AJAX-loaded content.
 */
Drupal.wysiwygEditorAttach = function () {
  for (var theme in Drupal.settings.wysiwygEditor.configs) {
    var config = Drupal.wysiwygEditorCloneObject(Drupal.settings.wysiwygEditor.configs[theme]);
    // Set configuration options for this theme.
    for (var setting in config) {
      tinyMCE.settings[setting] = config[setting];
    }
    $('textarea.wysiwyg-' + theme + ':not(.wysiwyg-processed)').each(function () {
      // Show toggle link if set.
      if (Drupal.settings.wysiwygEditor.showToggle) {
        Drupal.wysiwygEditorAttachToggleLink(this, theme);
      }
      // Attach Wysiwyg Editor control if default is on.
      if (Drupal.settings.wysiwygEditor.status) {
        tinyMCE.execCommand('mceAddControl', true, this.id);
      }
      $(this).addClass('wysiwyg-processed');
    });
  }
}

/**
 * Toggle the Wysiwyg Editor control and related link text for a textarea.
 */
Drupal.wysiwygEditorToggle = function (id, theme) {
  if (tinyMCE.getEditorId(id) == null) {
    var config = Drupal.wysiwygEditorCloneObject(Drupal.settings.wysiwygEditor.configs[theme]);
    // Set configuration options for this theme.
    for (var setting in config) {
      tinyMCE.settings[setting] = config[setting];
    }
    tinyMCE.addMCEControl($('#' + id).get(0), id);
    $('#wysiwyg4' + id).html(Drupal.settings.wysiwygEditor.disable).blur();
  }
  else {
    tinyMCE.removeMCEControl(tinyMCE.getEditorId(id));
    $('#wysiwyg4' + id).html(Drupal.settings.wysiwygEditor.enable).blur();
  }
}

/**
 * Append toggle link to textarea.
 */
Drupal.wysiwygEditorAttachToggleLink = function (elt, theme) {
  if (typeof(document.execCommand) == 'undefined') {
    $(elt).after('<div style="font-size:x-small">' + Drupal.settings.wysiwygEditor.noWysiwyg + '</div>');
  }
  else {
    var text = document.createTextNode(Drupal.settings.wysiwygEditor.status ? Drupal.settings.wysiwygEditor.disable : Drupal.settings.wysiwygEditor.enable);
    var a = document.createElement('a');
    $(a)
      .click(function() {
        Drupal.wysiwygEditorToggle(elt.id, theme);
      })
      .attr('id', 'wysiwyg4' + elt.id)
      .css('cursor', 'pointer')
      .append(text);
    var div = document.createElement('div');
    $(div).append(a);
    $(elt).after(div);
  }
}

Drupal.wysiwygEditorCloneObject = function (obj) {
  var clone = {};
  for (i in obj) {
    if ((typeof obj[i] == 'object') || (typeof obj[i] == 'array')) {
      clone[i] = Drupal.wysiwygEditorCloneObject(obj[i]);
    }
    else {
      clone[i] = obj[i];
    }
  }
  return clone;
}

/**
 * Global killswitch.
 */
if (Drupal.jsEnabled) {
  $(document).ready(Drupal.wysiwygEditorAttach);
}

/**
 * Initialize Wysiwyg Editor.
 */
Drupal.wysiwygEditorInit();
