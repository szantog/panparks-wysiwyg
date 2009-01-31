// $Id$

/**
 * Apply custom Wysiwyg API toolbar for input format.
 *
 * For whatever reason, our custom 'format' property is not available in
 * FCKConfig.format, but in FCKConfig.PageConfig.format instead.
 */
FCKConfig.ToolbarSets['Wysiwyg'] = window.parent.Drupal.settings.wysiwyg.configs.fckeditor[FCKConfig.PageConfig.format].buttons;

