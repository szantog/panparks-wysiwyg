// $Id$

/**
 * Initialize editor libraries.
 *
 * Some editors need to be initialized before the DOM is fully loaded. The
 * init hook gives them a chance to do so.
 */
Drupal.wysiwygInit = function() {
  jQuery.each(Drupal.wysiwyg.editor.init, function(editor) {
    // Clone, so original settings are not overwritten.
    this(Drupal.wysiwyg.clone(Drupal.settings.wysiwyg.configs[editor]));
  });
}

/**
 * Attach editors to input formats and target elements (f.e. textareas).
 *
 * This behavior searches for input format selectors and formatting guidelines
 * that have been preprocessed by Wysiwyg API. All CSS classes of those elements
 * with the prefix 'wysiwyg-' are parsed into input format parameters, defining
 * the input format, configured editor, target element id, and variable other
 * properties, which are passed to the attach/detach hooks of the corresponding
 * editor.
 *
 * Furthermore, an "enable/disable rich-text" toggle link is added after the
 * target element to allow users to alter its contents in plain text.
 *
 * This is executed once, while editor attach/detach hooks can be invoked
 * multiple times.
 *
 * @param context
 *   A DOM element, supplied by Drupal.attachBehaviors().
 */
Drupal.behaviors.attachWysiwyg = function(context) {
  $('.wysiwyg:not(.wysiwyg-processed)', context).each(function() {
    var params = Drupal.wysiwyg.getParams(this);
    $this = $(this);
    // Directly attach this editor, if the input format is enabled or there is
    // only one input format at all.
    if (($this.is(':input') && $this.is(':checked')) || $this.is('div')) {
      Drupal.wysiwygAttachToggleLink(context, params);
      Drupal.wysiwygAttach(context, params);
    }
    // Attach onChange handlers to input format selector elements.
    if ($this.is(':input')) {
      $this.change(function() {
        // If not disabled, detach the current and attach a new editor.
        if (Drupal.wysiwyg.instances[params.field].status) {
          Drupal.wysiwygDetach(context, params);
          Drupal.wysiwygAttach(context, params);
        }
      });
    }
    $this.addClass('wysiwyg-processed');
  });
}

/**
 * Attach an editor to a target element.
 *
 * This tests whether the passed in editor implements the attach hook and
 * invokes it if available. Editor profile settings are cloned first, so they
 * cannot be overridden. After attaching the editor, the toggle link is shown
 * again, except in case we are attaching no editor.
 *
 * @param context
 *   A DOM element, supplied by Drupal.attachBehaviors().
 * @param params
 *   An object containing input format parameters.
 */
Drupal.wysiwygAttach = function(context, params) {
  if (typeof Drupal.wysiwyg.editor.attach[params.editor] == 'function') {
    // Attach editor.
    Drupal.wysiwyg.editor.attach[params.editor](context, params, (Drupal.settings.wysiwyg.configs[params.editor] ? Drupal.wysiwyg.clone(Drupal.settings.wysiwyg.configs[params.editor][params.format]) : {}));
    // (Re-)initialize field instance.
    Drupal.wysiwyg.instances[params.field] = {};
    // Store new editor name and status for this field.
    Drupal.wysiwyg.instances[params.field].editor = params.editor;
    Drupal.wysiwyg.instances[params.field].status = true;
    // Display toggle link.
    $('#wysiwyg-toggle-' + params.field).show();
  }
  // Hide toggle link in case no editor is attached.
  if (params.editor == 'none') {
    $('#wysiwyg-toggle-' + params.field).hide();
  }
}

/**
 * Detach all editors from a target element.
 *
 * @param context
 *   A DOM element, supplied by Drupal.attachBehaviors().
 * @param params
 *   An object containing input format parameters.
 */
Drupal.wysiwygDetach = function(context, params) {
  var editor = Drupal.wysiwyg.instances[params.field].editor;
  if (jQuery.isFunction(Drupal.wysiwyg.editor.detach[editor])) {
    Drupal.wysiwyg.editor.detach[editor](context, params);
  }
}

/**
 * Append a editor toggle link to a target element.
 *
 * @param context
 *   A DOM element, supplied by Drupal.attachBehaviors().
 * @param params
 *   An object containing input format parameters.
 */
Drupal.wysiwygAttachToggleLink = function(context, params) {
  var text = document.createTextNode(Drupal.settings.wysiwyg.status ? Drupal.settings.wysiwyg.disable : Drupal.settings.wysiwyg.enable);
  var a = document.createElement('a');
  $(a).click(function() {
    if (Drupal.wysiwyg.instances[params.field].status) {
      Drupal.wysiwygDetach(context, params);
      Drupal.wysiwyg.instances[params.field].status = false;
      $('#wysiwyg-toggle-' + params.field).html(Drupal.settings.wysiwyg.enable).blur();
      // After disabling the editor, re-attach default behaviors.
      Drupal.wysiwyg.editor.attach.none(context, params);
    }
    else {
      // Before enabling the editor, detach default behaviors.
      Drupal.wysiwyg.editor.detach.none(context, params);
      // Attach the editor using parameters of the currently selected input format.
      Drupal.wysiwygAttach(context, Drupal.wysiwyg.getParams($('.wysiwyg-field-' + params.field + ':checked', context).get(0)));
      $('#wysiwyg-toggle-' + params.field).html(Drupal.settings.wysiwyg.disable).blur();
    }
  })
    .attr('id', 'wysiwyg-toggle-' + params.field)
    .attr('href', 'javascript:void(0);')
    .append(text);
  var div = document.createElement('div');
  $(div).append(a);
  $('#' + params.field).after(div);
}

/**
 * Parse the CSS classes of an input format DOM element into parameters.
 *
 * Format is "wysiwyg-name-value".
 */
Drupal.wysiwyg.getParams = function(element) {
  var classes = element.className.split(' ');
  var params = {};
  for (var i in classes) {
    if (classes[i].substr(0, 8) == 'wysiwyg-') {
      var parts = classes[i].split('-');
      var value = parts.slice(2).join('-');
      params[parts[1]] = value;
    }
  }
  // Convert format id into string.
  params.format = 'format' + params.format;
  return params;
}

/**
 * Clone a configuration object recursively.
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
 * Allow certain editor libraries to initialize before the DOM is loaded.
 */
Drupal.wysiwygInit();

