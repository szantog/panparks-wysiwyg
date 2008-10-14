<?php
// $Id$


/**
 * Implementation of hook_wysiwyg_plugin().
 */
function wysiwyg_editor_wysiwyg_plugin($editor, $version) {
  switch ($editor) {
    case 'tinymce':
      if ($version < 3) {
        return array(
          'wysiwyg' => array(
            'path' => drupal_get_path('module', 'wysiwyg_editor') .'/plugins/break/editor_plugin.js',
            'buttons' => array('break' => t('Teaser break')),
            'url' => 'http://drupal.org/project/wysiwyg',
          ),
        );
      }
      break;
  }
}

