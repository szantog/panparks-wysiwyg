<?php
// $Id$


/**
 * @file
 * Registers TinyMCE plugins for the Drupal TinyMCE module. Note these settings
 * can be overridden by each Drupal theme.
 */

/**
 * Implementation of hook_wysiwyg_plugin().
 */
function wysiwyg_editor_wysiwyg_plugin($editor) {
  $path_wysiwyg = drupal_get_path('module', 'wysiwyg_editor');
  switch ($editor) {
    case 'tinymce':
      $path_editor = $path_wysiwyg .'/tinymce/jscripts/tiny_mce';
      return array(
        'default' => array(
          'path' => $path_editor .'/themes/advanced',
          'buttons' => array('bold' => t('Bold'), 'italic' => t('Italic'), 'underline' => t('Underline'), 'strikethrough' => t('Strike-through'), 'justifyleft' => t('Justify left'), 'justifycenter' => t('Justify center'), 'justifyright' => t('Justify right'), 'justifyfull' => t('Justify full'), 'bullist' => t('Bullet list'), 'numlist' => t('Numbered list'), 'outdent' => t('Outdent'), 'indent' => t('Indent'), 'undo' => t('Undo'), 'redo' => t('Redo'), 'link' => t('Link'), 'unlink' => t('Unlink'), 'anchor' => t('Anchor'), 'image' => t('Image'), 'cleanup' => t('Clean-up'), 'forecolor' => t('Forecolor'), 'backcolor' => t('Backcolor'), 'sup' => t('Sup'), 'sub' => t('Sub'), 'code' => t('Code'), 'hr' => t('Horizontal rule'), 'cut' => t('Cut'), 'copy' => t('Copy'), 'paste' => t('Paste'), 'visualaid' => t('Visual aid'), 'removeformat' => t('Remove format'), 'charmap' => t('Char map'), 'help' => t('Help')),
          'internal' => TRUE,
        ),
        'advhr' => array(
          'path' => $path_editor .'/plugins/advhr',
          'buttons' => array('advhr' => t('Advanced horizontal rule')),
          'extended_valid_elements' => array('hr[class|width|size|noshade]'),
          'url' => 'http://wiki.moxiecode.com/index.php/TinyMCE:Plugins/advhr',
          'internal' => TRUE,
          'load' => TRUE,
        ),
        'advimage' => array(
          'path' => $path_editor .'/plugins/advimage',
          'extensions' => array('advimage' => t('Advanced image')),
          'extended_valid_elements' => array('img[class|src|alt|title|hspace|vspace|width|height|align|onmouseover|onmouseout|name]'),
          'url' => 'http://wiki.moxiecode.com/index.php/TinyMCE:Plugins/advimage',
          'internal' => TRUE,
          'load' => TRUE,
        ),
        'advlink' => array(
          'path' => $path_editor .'/plugins/advlink',
          'extensions' => array('advlink' => t('Advanced link')),
          'extended_valid_elements' => array('a[name|href|target|title|onclick]'),
          'url' => 'http://wiki.moxiecode.com/index.php/TinyMCE:Plugins/advlink',
          'internal' => TRUE,
          'load' => TRUE,
        ),
        'autosave' => array(
          'path' => $path_editor .'/plugins/autosave',
          'extensions' => array('autosave' => t('Auto save')),
          'url' => 'http://wiki.moxiecode.com/index.php/TinyMCE:Plugins/autosave',
          'internal' => TRUE,
          'load' => TRUE,
        ),
        'contextmenu' => array(
          'path' => $path_editor .'/plugins/contextmenu',
          'extensions' => array('contextmenu' => t('Context menu')),
          'url' => 'http://wiki.moxiecode.com/index.php/TinyMCE:Plugins/contextmenu',
          'internal' => TRUE,
        ),
        'directionality' => array(
          'path' => $path_editor .'/plugins/directionality',
          'buttons' => array('ltr' => t('Left-to-right'), 'rtl' => t('Right-to-left')),
          'url' => 'http://wiki.moxiecode.com/index.php/TinyMCE:Plugins/directionality',
          'internal' => TRUE,
          'load' => TRUE,
        ),
        'emotions' => array(
          'path' => $path_editor .'/plugins/emotions',
          'buttons' => array('emotions' => t('Emotions')),
          'url' => 'http://wiki.moxiecode.com/index.php/TinyMCE:Plugins/emotions',
          'internal' => TRUE,
          'load' => TRUE,
        ),
//        'filemanager' => array(
//          'path' => $path_editor .'/plugins/filemanager',
//          'extensions' => array('filemanager' => t('File manager')),
//          'url' => 'http://wiki.moxiecode.com/index.php/TinyMCE:Plugins/filemanager',
//          'internal' => TRUE,
//        ),
        'flash' => array(
          'path' => $path_editor .'/plugins/flash',
          'buttons' => array('flash' => t('Flash')),
          'extended_valid_elements' => array('img[class|src|alt|title|hspace|vspace|width|height|align|onmouseover|onmouseout|name|obj|param|embed]'),
          'url' => 'http://wiki.moxiecode.com/index.php/TinyMCE:Plugins/flash',
          'internal' => TRUE,
          'load' => TRUE,
        ),
        'font' => array(
          'path' => $path_editor .'/plugins/font',
          'buttons' => array('formatselect' => t('Font format'), 'fontselect' => t('Font'), 'fontsizeselect' => t('Font size'), 'styleselect' => t('Font style')),
          'extended_valid_elements' => array('font[face|size|color|style],span[class|align|style]'),
          'url' => 'http://wiki.moxiecode.com/index.php/TinyMCE:Plugins/font',
          'internal' => TRUE,
        ),
        'fullscreen' => array(
          'path' => $path_editor .'/plugins/fullscreen',
          'buttons' => array('fullscreen' => t('Fullscreen')),
          'url' => 'http://wiki.moxiecode.com/index.php/TinyMCE:Plugins/fullscreen',
          'internal' => TRUE,
          'load' => TRUE,
        ),
//        'imagemanager' => array(
//          'path' => $path_editor .'/plugins/imagemanager',
//          'extensions' => array('imagemanager' => t('Image manager')),
//          'url' => 'http://wiki.moxiecode.com/index.php/TinyMCE:Plugins/imagemanager',
//          'internal' => TRUE,
//        ),
        'inlinepopups' => array(
          'path' => $path_editor .'/plugins/inlinepopups',
          'extensions' => array('inlinepopups' => t('Inline popups')),
          'options' => array(
            'dialog_type' => array('modal'),
          ),
          'url' => 'http://wiki.moxiecode.com/index.php/TinyMCE:Plugins/inlinepopups',
          'internal' => TRUE,
          'load' => TRUE,
        ),
        'insertdatetime' => array(
          'path' => $path_editor .'/plugins/insertdatetime',
          'buttons' => array('insertdate' => t('Insert date'), 'inserttime' => t('Insert time')),
          'options' => array(
            'plugin_insertdate_dateFormat' => array('%Y-%m-%d'),
            'plugin_insertdate_timeFormat' => array('%H:%M:%S'),
          ),
          'url' => 'http://wiki.moxiecode.com/index.php/TinyMCE:Plugins/insertdatetime',
          'internal' => TRUE,
          'load' => TRUE,
        ),
        'layer' => array(
          'path' => $path_editor .'/plugins/layer',
          'buttons' => array('insertlayer' => t('Insert layer'), 'moveforward' => t('Move forward'), 'movebackward' => t('Move backward'), 'absolute' => t('Absolute')),
          'url' => 'http://wiki.moxiecode.com/index.php/TinyMCE:Plugins/layer',
          'internal' => TRUE,
          'load' => TRUE,
        ),
        'paste' => array(
          'path' => $path_editor .'/plugins/paste',
          'buttons' => array('pastetext' => t('Paste text'), 'pasteword' => t('Paste from Word'), 'selectall' => t('Select all')),
          'url' => 'http://wiki.moxiecode.com/index.php/TinyMCE:Plugins/paste',
          'internal' => TRUE,
          'load' => TRUE,
        ),
        'preview' => array(
          'path' => $path_editor .'/plugins/preview',
          'buttons' => array('preview' => t('Preview')),
          'url' => 'http://wiki.moxiecode.com/index.php/TinyMCE:Plugins/preview',
          'internal' => TRUE,
          'load' => TRUE,
        ),
        'print' => array(
          'path' => $path_editor .'/plugins/print',
          'buttons' => array('print' => t('Print')),
          'url' => 'http://wiki.moxiecode.com/index.php/TinyMCE:Plugins/print',
          'internal' => TRUE,
          'load' => TRUE,
        ),
        'searchreplace' => array(
          'path' => $path_editor .'/plugins/searchreplace',
          'buttons' => array('search' => t('Search'), 'replace' => t('Replace')),
          'url' => 'http://wiki.moxiecode.com/index.php/TinyMCE:Plugins/searchreplace',
          'internal' => TRUE,
          'load' => TRUE,
        ),
        'style' => array(
          'path' => $path_editor .'/plugins/style',
          'buttons' => array('styleprops' => t('Style properties')),
          'url' => 'http://wiki.moxiecode.com/index.php/TinyMCE:Plugins/style',
          'internal' => TRUE,
          'load' => TRUE,
        ),
        'table' => array(
          'path' => $path_editor .'/plugins/table',
          'buttons' => array('tablecontrols' => t('Table')),
          'url' => 'http://wiki.moxiecode.com/index.php/TinyMCE:Plugins/table',
          'internal' => TRUE,
          'load' => TRUE,
        ),
        'wysiwyg' => array(
          'path' => $path_wysiwyg .'/plugins/break',
          'buttons' => array('break' => t('Teaser break')),
          'url' => 'http://drupal.org/project/wysiwyg',
        ),
      );
  }
}

