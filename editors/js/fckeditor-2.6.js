// $Id$

/**
 * Attach this editor to a target element.
 */
Drupal.wysiwyg.editor.attach.fckeditor = function(context, params, settings) {
  var FCKinstance = new FCKeditor(params.field, settings.Width, settings.Height, settings.ToolbarSet);
  // Apply editor instance settings.
  FCKinstance.BasePath = settings.EditorPath;
  // Apply 'Wysiwyg' toolbar, if defined.
  if (settings.CustomConfigurationsPath) {
    FCKinstance.Config.wysiwygFormat = params.format;
    FCKinstance.Config.CustomConfigurationsPath = settings.CustomConfigurationsPath;
  }

  // Load Drupal plugins and apply format specific settings.
  // @see fckeditor.config.js

  // Attach editor.
  FCKinstance.ReplaceTextarea();
};

/**
 * Detach a single or all editors.
 */
Drupal.wysiwyg.editor.detach.fckeditor = function(context, params) {
  var instances = [];
  if (typeof params != 'undefined' && typeof FCKeditorAPI != 'undefined') {
    var instance = FCKeditorAPI.GetInstance(params.field);
    if (instance) {
      instances[params.field] = instance;
    }
  }
  else {
    instances = FCKeditorAPI.__Instances;
  }

  for (var instanceName in instances) {
    // Shut down the instance and give plugins a chance to detach by changing status first.
    instances[instanceName].SetStatus(FCK_STATUS_NOTLOADED);
    instances[instanceName].UpdateLinkedField();
    $('#' + instanceName).show();
    $('#' + instanceName + '___Config').remove();
    $('#' + instanceName + '___Frame').remove();
    delete FCKeditorAPI.__Instances[instanceName];
  }
};

Drupal.wysiwyg.editor.instance.fckeditor = {
  addPlugin: function(plugin, settings, pluginSettings, instance) {
    if (typeof Drupal.wysiwyg.plugins[plugin] != 'object') {
      return;
    }

    if (Drupal.settings.wysiwyg.plugins[instance.wysiwygFormat].drupal[plugin].css) {
      instance.FCKConfig.EditorAreaCSS += ',' + Drupal.settings.wysiwyg.plugins[instance.wysiwygFormat].drupal[plugin].css;
    }

    // @see fckcommands.js, fck_othercommands.js, fckpastewordcommand.js
    instance.FCKCommands.RegisterCommand(plugin, {
      // Invoke the plugin's button.
      Execute: function () {
        if (typeof Drupal.wysiwyg.plugins[plugin].invoke == 'function') {
          var data = { format: 'html', node: instance.FCKSelection.GetParentElement() };
          // @todo This is NOT the same as data.node.
          data.content = data.node.innerHTML;
          Drupal.wysiwyg.plugins[plugin].invoke(data, pluginSettings, instance.FCK.Name);
        }
      },

      // isNode: Return whether the plugin button should be enabled for the
      // current selection.
      // @see FCKUnlinkCommand.prototype.GetState()
      GetState: function () {
        // Disabled if not in WYSIWYG mode.
        if (instance.FCK.EditMode != FCK_EDITMODE_WYSIWYG) {
          return FCK_TRISTATE_DISABLED;
        }

        var state = instance.FCK.GetNamedCommandState(this.Name);
        if (state == FCK_TRISTATE_OFF && instance.FCK.EditMode == FCK_EDITMODE_WYSIWYG) {
          if (typeof Drupal.wysiwyg.plugins[plugin].isNode == 'function') {
            var node = instance.FCKSelection.GetSelectedElement();
            state = Drupal.wysiwyg.plugins[plugin].isNode(node) ? FCK_TRISTATE_ON : FCK_TRISTATE_OFF;
          }
        }
        return state;
      },

      /**
       * Return information about the plugin as a name/value array.
       */
      Name: plugin
    });

    // Register the plugin button.
    // var FCKToolbarButton = function(commandName, label, tooltip, style, sourceView, contextSensitive, icon)
    instance.FCKToolbarItems.RegisterItem(plugin, new instance.FCKToolbarButton(plugin, settings.iconTitle, settings.iconTitle, null, false, true, settings.icon));
  },

  openDialog: function(dialog, params) {
    // @todo Implement open dialog.
  },

  closeDialog: function(dialog) {
    // @todo Implement close dialog.
  },

  prepareContent: function(content) {
    // @todo Not needed for FCKeditor?
    return content;
  },

  insert: function(content) {
    var instance = FCKeditorAPI.GetInstance(this.field);
    // @see FCK.InsertHtml(), FCK.InsertElement()
    instance.InsertHtml(content);
  }
};

