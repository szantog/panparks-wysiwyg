// $Id$

Drupal.wysiwyg = Drupal.wysiwyg || { 'init': {}, 'attach': {}, 'detach': {}, 'toggle': {} };

/**
 * Initialize all editor libraries.
 */
Drupal.wysiwygEditorInit = function() {
  jQuery.each(Drupal.wysiwyg.init, function(editor) {
    this(Drupal.settings.wysiwygEditor.configs[editor]);
  });
}

/**
 * Attach editors to fields.
 *
 * This function can be called to process AJAX-loaded content.
 */
Drupal.behaviors.attachWysiwyg = function(context) {
  jQuery.each(Drupal.wysiwyg.attach, function(editor) {
    // Show toggle link if set.
    if (Drupal.settings.wysiwygEditor.showToggle) {
      for (var theme in Drupal.settings.wysiwygEditor.configs[editor]) {
        $('textarea.wysiwyg-' + theme + ':not(.wysiwyg-processed)', context).each(function() {
          Drupal.wysiwygEditorAttachToggleLink(this, editor, theme);
        });
      }
    }
    this(context, Drupal.settings.wysiwygEditor.configs[editor]);
  });
}

/**
 * Append a toggle link to an element.
 *
 * @param element
 *   The DOM element to toggle the editor for.
 * @param editor
 *   The editor name assigned to the element.
 * @param theme
 *   The editor theme assigned to the element.
 */
Drupal.wysiwygEditorAttachToggleLink = function(element, editor, theme) {
  var text = document.createTextNode(Drupal.settings.wysiwygEditor.status ? Drupal.settings.wysiwygEditor.disable : Drupal.settings.wysiwygEditor.enable);
  var a = document.createElement('a');
  $(a)
    .click(function() {
      Drupal.wysiwygEditorToggle(element, editor, theme);
    })
    .attr('id', 'wysiwyg4' + element.id)
    .css('cursor', 'pointer')
    .append(text);
  var div = document.createElement('div');
  $(div).append(a);
  $(element).after(div);
}

/**
 * Enable/disable the editor and change toggle link text accordingly.
 *
 * Toggle implementation functions are expected to return the new state of a
 * toggled editor.
 *
 * @param element
 *   The DOM element to toggle the editor for.
 * @param editor
 *   The editor name assigned to the element.
 * @param theme
 *   The editor theme assigned to the element.
 */
Drupal.wysiwygEditorToggle = function(element, editor, theme) {
  if (typeof Drupal.wysiwyg.toggle[editor] == 'function') {
    var new_state = Drupal.wysiwyg.toggle[editor](element, theme);
  }
  if (new_state) {
    $('#wysiwyg4' + element.id).html(Drupal.settings.wysiwygEditor.disable).blur();
  }
  else {
    $('#wysiwyg4' + element.id).html(Drupal.settings.wysiwygEditor.enable).blur();
  }
}

/**
 * Clone a configuration object recursively; required for certain editors.
 *
 * @param obj
 *   The object to clone.
 *
 * @return
 *   A copy of the passed in object.
 */
Drupal.wysiwyg.clone = function(obj) {
  var clone = {};
  for (i in obj) {
    if ((typeof obj[i] == 'object') || (typeof obj[i] == 'array')) {
      clone[i] = Drupal.wysiwyg.clone(obj[i]);
    }
    else {
      clone[i] = obj[i];
    }
  }
  return clone;
}

/**
 * Initialize editor libraries.
 */
Drupal.wysiwygEditorInit();

