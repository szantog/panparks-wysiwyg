<?php
// $Id$

/**
 * @file
 * Editor integration functions for CKeditor.
 */

/**
 * Plugin implementation of hook_editor().
 */
function wysiwyg_ckeditor_editor() {
  $editor['ckeditor'] = array(
    'title' => 'CKeditor',
    'vendor url' => 'http://ckeditor.com',
    'download url' => 'http://ckeditor.com',
    'libraries' => array(
      '' => array(
        'title' => 'Default',
        'files' => array('ckeditor.js'),
      ),
      'src' => array(
        'title' => 'Source',
        'files' => array('ckeditor_source.js'),
      ),
    ),
    'version callback' => 'wysiwyg_ckeditor_version',
    'themes callback' => 'wysiwyg_ckeditor_themes',
    'settings callback' => 'wysiwyg_ckeditor_settings',
    'plugin callback' => 'wysiwyg_ckeditor_plugins',
    'plugin settings callback' => 'wysiwyg_ckeditor_plugin_settings',
    'versions' => array(
      '3.0.3665' => array(
        'js files' => array('ckeditor-3.0.js'),
      ),
    ),
  );
  return $editor;
}

/**
 * Detect editor version.
 *
 * @param $editor
 *   An array containing editor properties as returned from hook_editor().
 *
 * @return
 *   The installed editor version.
 */
function wysiwyg_ckeditor_version($editor) {
  $library = $editor['library path'] . '/ckeditor.js';
  $library = fopen($library, 'r');
  $max_lines = 8;
  while ($max_lines && $line = fgets($library, 140)) {
    // version:'CKEditor 3.0 SVN',revision:'3665'
    if (preg_match('@version:\'CKEditor ([\d\.]+)(?: .+revision:\'([\d]+))?@', $line, $version)) {
      fclose($library);
      return $version[1] . '.' . $version[2];
    }
    $max_lines--;
  }
  fclose($library);
}

/**
 * Determine available editor themes or check/reset a given one.
 *
 * @param $editor
 *   A processed hook_editor() array of editor properties.
 * @param $profile
 *   A wysiwyg editor profile.
 *
 * @return
 *   An array of theme names. The first returned name should be the default
 *   theme name.
 */
function wysiwyg_ckeditor_themes($editor, $profile) {
  return array('default');
}

/**
 * Return runtime editor settings for a given wysiwyg profile.
 *
 * @param $editor
 *   A processed hook_editor() array of editor properties.
 * @param $config
 *   An array containing wysiwyg editor profile settings.
 * @param $theme
 *   The name of a theme/GUI/skin to use.
 *
 * @return
 *   A settings array to be populated in
 *   Drupal.settings.wysiwyg.configs.{editor}
 */
function wysiwyg_ckeditor_settings($editor, $config, $theme) {
  $settings = array(
    'basePath' => base_path() . $editor['library path'] . '/',
    'SkinPath' => base_path() . $editor['library path'] . '/editor/skins/' . $theme . '/',
    'Width' => '100%',
    'Height' => 420,
    // By default, CKeditor converts most characters into HTML entities. Since
    // it does not support a custom definition, but Drupal supports Unicode, we
    // disable at least the additional character sets. CKeditor always converts
    // XML default characters '&', '<', '>'.
    // @todo Check whether completely disabling ProcessHTMLEntities is an option.
    'IncludeLatinEntities' => FALSE,
    'IncludeGreekEntities' => FALSE,
  );
  if (isset($config['block_formats'])) {
    $settings['FontFormats'] = strtr($config['block_formats'], array(',' => ';'));
  }
  if (isset($config['apply_source_formatting'])) {
    $settings['FormatOutput'] = $settings['FormatSource'] = $config['apply_source_formatting'];
  }

  if (isset($config['css_setting'])) {
    if ($config['css_setting'] == 'theme') {
      // CKeditor only supports one CSS file currently.
      $settings['contentsCss'] = reset(wysiwyg_get_css());
    }
    else if ($config['css_setting'] == 'self' && isset($config['css_path'])) {
      $settings['contentsCss'] = strtr($config['css_path'], array('%b' => base_path(), '%t' => path_to_theme()));
    }
  }

  if (!empty($config['buttons'])) {
    $settings['toolbar'] = array();
    $plugins = wysiwyg_get_plugins($editor['name']);
    foreach ($config['buttons'] as $plugin => $buttons) {
      foreach ($buttons as $button => $enabled) {
        // Iterate separately over buttons and extensions properties.
        foreach (array('buttons', 'extensions') as $type) {
          // Skip unavailable plugins.
          if (!isset($plugins[$plugin][$type][$button])) {
            continue;
          }
          // Add buttons.
          if ($type == 'buttons') {
            $settings['toolbar'][] = $button;
          }
          // Allow plugins to add or override global configuration settings.
          if (!empty($plugins[$plugin]['options'])) {
            $settings = array_merge($settings, $plugins[$plugin]['options']);
          }
        }
      }
    }
    // For now, all buttons are placed into one row.
    if (!empty($settings['toolbar'])) {
      $settings['toolbar'] = array($settings['toolbar']);
    }
  }

  return $settings;
}

/**
 * Build a JS settings array of native external plugins that need to be loaded separately.
 */
function wysiwyg_ckeditor_plugin_settings($editor, $profile, $plugins) {
  $settings = array();
  foreach ($plugins as $name => $plugin) {
    // Register all plugins that need to be loaded.
    if (!empty($plugin['load'])) {
      $settings[$name] = array();
      // Add path for native external plugins; internal ones do not need a path.
      if (empty($plugin['internal']) && isset($plugin['path'])) {
        $settings[$name]['path'] = base_path() . $plugin['path'];
      }
      if (!empty($plugin['languages'])) {
        $settings[$name]['languages'] = $plugin['languages'];
      }
    }
  }
  return $settings;
}

/**
 * Return internal plugins for this editor; semi-implementation of hook_wysiwyg_plugin().
 */
function wysiwyg_ckeditor_plugins($editor) {
  $plugins = array(
    'default' => array(
      'buttons' => array(
        'Bold' => t('Bold'), 'Italic' => t('Italic'), 'Underline' => t('Underline'),
        'StrikeThrough' => t('Strike-through'),
        'JustifyLeft' => t('Align left'), 'JustifyCenter' => t('Align center'), 'JustifyRight' => t('Align right'), 'JustifyFull' => t('Justify'),
        'UnorderedList' => t('Bullet list'), 'OrderedList' => t('Numbered list'),
        'Outdent' => t('Outdent'), 'Indent' => t('Indent'),
        'Undo' => t('Undo'), 'Redo' => t('Redo'),
        'Link' => t('Link'), 'Unlink' => t('Unlink'), 'Anchor' => t('Anchor'),
        'Image' => t('Image'),
        'TextColor' => t('Forecolor'), 'BGColor' => t('Backcolor'),
        'Superscript' => t('Superscript'), 'Subscript' => t('Subscript'),
        'Blockquote' => t('Blockquote'), 'Source' => t('Source code'),
        'Rule' => t('Horizontal rule'),
        'Cut' => t('Cut'), 'Copy' => t('Copy'), 'Paste' => t('Paste'),
        'PasteText' => t('Paste Text'), 'PasteWord' => t('Paste from Word'),
        'ShowBlocks' => t('Show blocks'),
        'RemoveFormat' => t('Remove format'),
        'SpecialChar' => t('Character map'),
        'About' => t('About'),
        'FontFormat' => t('HTML block format'), 'FontName' => t('Font'), 'FontSize' => t('Font size'), 'Style' => t('Font style'),
        'Table' => t('Table'),
        'Find' => t('Search'), 'Replace' => t('Replace'), 'SelectAll' => t('Select all'),
        'CreateDiv' => t('Create DIV container'),
        'Flash' => t('Flash'), 'Smiley' => t('Smiley'),
        'FitWindow' => t('FitWindow'),
        'SpellCheck' => t('Check spelling'),
      ),
      'internal' => TRUE,
    ),
  );
  return $plugins;
}

