// $Id$

/**
 * Initialize each tinyMCE configuration.
 *
 * This function needs to be called before the page is fully loaded, as
 * calling tinyMCE.init() after the page is loaded breaks in IE 6.
 */
Drupal.tinymceInit = function () {
  for (var theme in Drupal.settings.tinymce.configs) {
    // Clone so we are not passing by reference. Otherwise the
    // settings will get overwritten.
    var config = Drupal.tinymceCloneObject(Drupal.settings.tinymce.configs[theme]);
    tinyMCE.init(config);
  }
}

/**
 * Attach tinyMCE to textareas.
 *
 * This function can be called to process AJAX-loaded content.
 */
Drupal.tinymceAttach = function () {
  for (var theme in Drupal.settings.tinymce.configs) {
    var config = Drupal.tinymceCloneObject(Drupal.settings.tinymce.configs[theme]);
    // Set configuration options for this theme.
    for (var setting in config) {
      tinyMCE.settings[setting] = config[setting];
    }
    $('textarea.tinymce-' + theme + ':not(.tinymce-processed)').each(function () {
      // Show toggle link if set.
      if (Drupal.settings.tinymce.showToggle) {
        Drupal.tinymceAttachToggleLink(this, theme);
      }
      // Attach tinyMCE control if default is on.
      if (Drupal.settings.tinymce.status) {
        tinyMCE.execCommand('mceAddControl', true, this.id);
      }
      $(this).addClass('tinymce-processed');
    });
  }
}

/**
 * Toggle the tinyMCE control and related link text for a textarea.
 */
Drupal.tinymceToggle = function (id, theme) {
  if (tinyMCE.getEditorId(id) == null) {
    var config = Drupal.tinymceCloneObject(Drupal.settings.tinymce.configs[theme]);
    // Set configuration options for this theme.
    for (var setting in config) {
      tinyMCE.settings[setting] = config[setting];
    }
    tinyMCE.addMCEControl($('#' + id).get(0), id);
    $('#wysiwyg4' + id)
      .html(Drupal.settings.tinymce.disable)
      .blur();
  }
  else {
    tinyMCE.removeMCEControl(tinyMCE.getEditorId(id));
    $('#wysiwyg4' + id)
      .html(Drupal.settings.tinymce.enable)
      .blur();
  }
}

/**
 * Append toggle link to textarea.
 */
Drupal.tinymceAttachToggleLink = function (elt, theme) {
  if (typeof(document.execCommand) == 'undefined') {
    $(elt).after('<div style="font-size:x-small">' + Drupal.settings.tinymce.noWysiwyg + '</div>');
  }
  else {
    var text = document.createTextNode(Drupal.settings.tinymce.status ? Drupal.settings.tinymce.disable : Drupal.settings.tinymce.enable);
    var a = document.createElement('a');
    $(a)
      .click(function() {
        Drupal.tinymceToggle(elt.id, theme);
      })
      .attr('id', 'wysiwyg4' + elt.id)
      .css('cursor', 'pointer')
      .append(text);
    var div = document.createElement('div');
    $(div).append(a);
    $(elt).after(div);
  }
}

Drupal.tinymceCloneObject = function (obj) {
  var clone = {};
  for (i in obj) {
    if ((typeof obj[i] == 'object') || (typeof obj[i] == 'array')) {
      clone[i] = Drupal.tinymceCloneObject(obj[i]);
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
  $(document).ready(Drupal.tinymceAttach);
}

/**
 * Call tinyMCE initialization.
 */
Drupal.tinymceInit();
