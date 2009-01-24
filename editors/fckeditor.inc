<?php
// $Id$

/**
 * @file
 * Editor integration functions for FCKeditor.
 */


/**
 * Plugin implementation of hook_editor().
 */
function wysiwyg_fckeditor_editor() {
  $editor = array();
  $editor['fckeditor'] = array(
    'title' => 'FCKeditor',
    'vendor url' => 'http://www.fckeditor.net',
    'download url' => 'http://www.fckeditor.net/download',
    'library path' => wysiwyg_get_path('fckeditor'),
    'libraries' => array(
      '' => array(
        'title' => 'Default',
        'files' => array('fckeditor.js'),
      ),
    ),
    'version callback' => 'wysiwyg_fckeditor_version',
    'themes callback' => 'wysiwyg_fckeditor_themes',
    'settings callback' => 'wysiwyg_fckeditor_settings',
    'plugin callback' => 'wysiwyg_fckeditor_plugins',
    'versions' => array(
      2.6 => array(
        'js files' => array('fckeditor-2.6.js'),
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
function wysiwyg_fckeditor_version($editor) {
  $library = $editor['library path'] .'/fckeditor.js';
  $library = fopen($library, 'r');
  $max_lines = 100;
  while ($max_lines && $line = fgets($library, 60)) {
    if (preg_match('@^FCKeditor.prototype.Version\s*= \'([\d\.]+)@', $line, $version)) {
      fclose($library);
      return $version[1];
    }
    $max_lines--;
  }
  fclose($library);
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
function wysiwyg_fckeditor_settings($editor, $config, $theme) {
  $settings = array(
    'EditorPath' => base_path() . $editor['library path'] .'/',
    'SkinPath' => base_path() . $editor['library path'] .'/editor/skins/default/',
    'Width' => '100%',
    'Height' => 420,
    'DefaultLanguage' => 'en',
    'AutoDetectLanguage' => TRUE,
    'ToolbarStartExpanded' => TRUE,
    'EnterMode' => 'p',
    'ShiftEnterMode' => 'br',
    'LinkBrowser' => FALSE,
    'LinkUpload' => FALSE,
    'ImageBrowser' => FALSE,
    'ImageUpload' => FALSE,
    'FlashBrowser' => FALSE,
    'FlashUpload' => FALSE,
  );
  if (isset($config['block_formats'])) {
    $settings['FontFormats'] = strtr($config['block_formats'], array(',' => ';'));
  }
  if (isset($config['apply_source_formatting'])) {
    $settings['FormatSource'] = $config['apply_source_formatting'];
  }
  if (isset($config['preformatted'])) {
    $settings['FormatOutput'] = $config['preformatted'];
  }

  if (isset($config['css_setting'])) {
    if ($config['css_setting'] == 'theme') {
      $css = path_to_theme() .'/style.css';
      if (file_exists($css)) {
        $settings['EditorAreaCSS'] = base_path() . $css;
      }
    }
    else if ($config['css_setting'] == 'self' && isset($config['css_path'])) {
      $settings['EditorAreaCSS'] = strtr($config['css_path'], array('%b' => base_path(), '%t' => path_to_theme()));
    }
  }

  return $settings;
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
function wysiwyg_fckeditor_themes($editor, $profile) {
  return array('default', 'office2003', 'silver');
}

/**
 * Return internal plugins for FCKeditor; semi-implementation of hook_wysiwyg_plugin().
 */
function wysiwyg_fckeditor_plugins($editor) {
  return array(
  );
}

