<?php
// $Id$


/**
 * @file
 * Integrate Wysiwyg editors into Drupal.
 */

/**
 * Callback handler for admin pages; menu callback.
 *
 * @todo Move into hook_menu(), resp. FAPI functions.
 */
function wysiwyg_editor_admin($arg = '') {
  $edit = $_POST;
  $op = isset($_POST['op']) ? $_POST['op'] : '';
  $op = ($arg && !$op ? $arg : $op);
  switch ($op) {
    case 'add':
      $breadcrumb[] = l(t('Home'), NULL);
      $breadcrumb[] = l(t('Administer'), 'admin');
      $breadcrumb[] = l(t('Site configuration'), 'admin/settings');
      $breadcrumb[] = l(t('Wysiwyg'), 'admin/settings/wysiwyg');
      $breadcrumb[] = l(t('Wysiwyg Profiles'), 'admin/settings/wysiwyg/profile');
      drupal_set_breadcrumb($breadcrumb);
      $output = drupal_get_form('wysiwyg_editor_profile_form', $edit);
      break;

    case 'edit':
      drupal_set_title(t('Edit Wysiwyg Editor profile'));
      $output = drupal_get_form('wysiwyg_editor_profile_form', wysiwyg_editor_profile_load(urldecode(arg(5))));
      break;

    case 'delete':
      wysiwyg_editor_profile_delete(urldecode(arg(5)));
      drupal_set_message(t('Deleted profile'));
      drupal_goto('admin/settings/wysiwyg/profile');
      break;

    case t('Create profile'):
    case t('Update profile'):
      $output = drupal_get_form('wysiwyg_editor_profile_form', $edit);
      break;

    default:
      $output = wysiwyg_editor_profile_overview();
  }

  return $output;
}

/**
 * Return an HTML form for profile configuration.
 */
function wysiwyg_editor_profile_form($form_state, $edit) {
  // Merge in defaults.
  settype($edit, 'array');
  $edit += array(
    'name' => NULL,
    'rids' => NULL,
    'settings' => array(
      'default' => TRUE,
      'user_choose' => FALSE,
      'show_toggle' => TRUE,
      'theme' => 'advanced',
      'language' => 'en',
      'access' => 1,
      'access_pages' => "node/*\nuser/*\ncomment/*",
      'buttons' => array(),
      'toolbar_loc' => 'top',
      'toolbar_align' => 'left',
      'path_loc' => 'bottom',
      'resizing' => TRUE,
      // Also available, but buggy in TinyMCE 2.x: blockquote,code,dt,dd,samp.
      'block_formats' => 'p,address,pre,h2,h3,h4,h5,h6,div',
      'verify_html' => TRUE,
      'preformatted' => FALSE,
      'convert_fonts_to_spans' => TRUE,
      'remove_linebreaks' => TRUE,
      'apply_source_formatting' => FALSE,
      'paste_auto_cleanup_on_paste' => FALSE,
      'css_setting' => 'theme',
      'css_path' => NULL,
      'css_classes' => NULL,
    ),
  );
  $edit = (object)$edit;

  if (arg(4) == 'add') {
    $btn = t('Create profile');
  }
  else {
    $form['old_name'] = array('#type' => 'hidden', '#value' => $edit->name);
    $btn = t('Update profile');
  }

  $form['basic'] = array(
    '#type' => 'fieldset',
    '#title' => t('Basic setup'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['basic']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Profile name'),
    '#default_value' => $edit->name,
    '#size' => 40,
    '#maxlength' => 128,
    '#description' => t('Enter a name for this profile. This name is only visible within the Wysiwyg Editor administration page.'),
    '#required' => TRUE,
  );

  $form['basic']['rids'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Roles allowed to use this profile'),
    '#default_value' => array_keys((array)$edit->rids),
    '#options' => user_roles(FALSE, 'access wysiwyg editor'),
    '#description' => t('Only roles with the %access permission will be shown here.', array('%access' => 'access wysiwyg editor')),
    '#required' => TRUE,
  );

  $form['basic']['default'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enabled by default'),
    '#default_value' => $edit->settings['default'],
    '#return_value' => 1,
    '#description' => t('The default editor state for users having access to this profile. Users are able to override this state if the next option is enabled.'),
  );

  $form['basic']['user_choose'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow users to choose default'),
    '#default_value' => $edit->settings['user_choose'],
    '#return_value' => 1,
    '#description' => t('If allowed, users will be able to choose their own Wysiwyg Editor default state in their user account settings.'),
  );

  $form['basic']['show_toggle'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show %editor toggle link', array('%editor' => t('enable/disable rich text'))),
    '#default_value' => $edit->settings['show_toggle'],
    '#return_value' => 1,
    '#description' => t('Whether or not to show the %editor toggle link below a textarea. If disabled, the user setting or global default is used (see above).', array('%editor' => t('enable/disable rich text'))),
  );

  $form['basic']['theme'] = array(
    '#type' => 'hidden',
    '#value' => $edit->settings['theme'],
  );

  $form['basic']['language'] = array(
    '#type' => 'select',
    '#title' => t('Language'),
    '#default_value' => $edit->settings['language'],
    '#options' => drupal_map_assoc(array('ar', 'ca', 'cs', 'cy', 'da', 'de', 'el', 'en', 'es', 'fa', 'fi', 'fr', 'fr_ca', 'he', 'hu', 'is', 'it', 'ja', 'ko', 'nb', 'nl', 'nn', 'pl', 'pt', 'pt_br', 'ru', 'ru_KOI8-R', 'ru_UTF-8', 'si', 'sk', 'sv', 'th', 'zh_cn', 'zh_tw', 'zh_tw_utf8')),
    '#description' => t('The language for the Wysiwyg Editor interface. Language codes are based on the <a href="http://www.loc.gov/standards/iso639-2/englangn.html">ISO-639-2</a> format.'),
  );

  $form['visibility'] = array(
    '#type' => 'fieldset',
    '#title' => t('Visibility'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  // If the visibility is set to PHP mode but the user doesn't have this block
  // permission, don't allow them to edit nor see this PHP code
  $access = user_access('use PHP for block visibility');
  if ($edit->settings['access'] == 2 && !$access) {
    $form['visibility'] = array();
    $form['visibility']['access'] = array(
      '#type' => 'value',
      '#value' => 2,
    );
    $form['visibility']['access_pages'] = array(
      '#type' => 'value',
      '#value' => $edit->settings['access_pages'],
    );
  }
  else {
    $options = array(t('Show on every page except the listed pages.'), t('Show on only the listed pages.'));
    $description = t("Enter one page per line as Drupal paths. The '*' character is a wildcard. Example paths are '%blog' for the blog page and %blog-wildcard for every personal blog. %front is the front page.", array('%blog' => 'blog', '%blog-wildcard' => 'blog/*', '%front' => '<front>'));
    if ($access) {
      $options[] = t('Show if the following PHP code returns <code>TRUE</code> (PHP-mode, experts only).');
      $description .= ' '. t('If the PHP-mode is chosen, enter PHP code between %php. Note that executing incorrect PHP-code can break your Drupal site.', array('%php' => '<?php ?>'));
    }
    $form['visibility']['access'] = array(
      '#type' => 'radios',
      '#title' => t('Show Wysiwyg Editor on specific pages'),
      '#default_value' => $edit->settings['access'],
      '#options' => $options,
    );
    $form['visibility']['access_pages'] = array(
      '#type' => 'textarea',
      '#title' => t('Pages'),
      '#default_value' => $edit->settings['access_pages'],
      '#description' => $description,
      '#wysiwyg' => FALSE,
    );
  }

  $form['buttons'] = array(
    '#type' => 'fieldset',
    '#title' => t('Buttons and plugins'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
    '#theme' => 'wysiwyg_editor_admin_button_table',
  );

  $plugins = _wysiwyg_editor_get_buttons(TRUE);
  // Generate the button list.
  foreach ($plugins as $name => $meta) {
    if (isset($meta['buttons']) && is_array($meta['buttons'])) {
      foreach ($meta['buttons'] as $button => $title) {
        $img_src = $meta['path'] ."/images/$name.gif";
        // Handle plugins that have more than one button.
        if (!file_exists($img_src)) {
          $img_src = $meta['path'] ."/images/$button.gif";
        }
        $b = file_exists($img_src) ? '<img src="'. base_path() . $img_src .'" title="'. $button .'" style="border: 1px solid grey; vertical-align: middle;" />' : '';
        if (isset($meta['url'])) {
          $title = $b .' '. l($title, $meta['url'], array('target' => '_blank'));
        }
        else {
          $title = $b .' '. $title;
        }
        $form['buttons'][$name][$button] = array(
          '#type' => 'checkbox',
          '#title' => $title,
          '#default_value' => isset($edit->settings['buttons'][$name][$button]) ? $edit->settings['buttons'][$name][$button] : NULL,
        );
      }
    }
    else if (isset($meta['extensions']) && is_array($meta['extensions'])) {
      foreach ($meta['extensions'] as $extension => $title) {
        if (isset($meta['url'])) {
          $title = l($title, $meta['url'], array('target' => '_blank'));
        }
        else {
          $title = $title;
        }
        $form['buttons'][$name][$extension] = array(
          '#type' => 'checkbox',
          '#title' => $title,
          '#default_value' => isset($edit->settings['buttons'][$name][$extension]) ? $edit->settings['buttons'][$name][$extension] : NULL,
        );
      }
    }
  }

  $form['appearance'] = array(
    '#type' => 'fieldset',
    '#title' => t('Editor appearance'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['appearance']['toolbar_loc'] = array(
    '#type' => 'select',
    '#title' => t('Toolbar location'),
    '#default_value' => $edit->settings['toolbar_loc'],
    '#options' => array('bottom' => t('Bottom'), 'top' => t('Top')),
    '#description' => t('This option controls whether the editor toolbar is displayed above or below the editing area.'),
  );

  $form['appearance']['toolbar_align'] = array(
    '#type' => 'select',
    '#title' => t('Button alignment'),
    '#default_value' => $edit->settings['toolbar_align'],
    '#options' => array('center' => t('Center'), 'left' => t('Left'), 'right' => t('Right')),
    '#description' => t('This option controls the alignment of icons in the editor toolbar.'),
  );

  $form['appearance']['path_loc'] = array(
    '#type' => 'select',
    '#title' => t('Path location'),
    '#default_value' => $edit->settings['path_loc'],
    '#options' => array('none' => t('Hide'), 'top' => t('Top'), 'bottom' => t('Bottom')),
    '#description' => t('Where to display the path to HTML elements (i.e. <code>body > table > tr > td</code>).'),
  );

  $form['appearance']['resizing'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable resizing button'),
    '#default_value' => $edit->settings['resizing'],
    '#return_value' => 1,
    '#description' => t('This option gives you the ability to enable/disable the resizing button. If enabled, the Path location toolbar must be set to "Top" or "Bottom" in order to display the resize icon.'),
  );

  $form['output'] = array(
    '#type' => 'fieldset',
    '#title' => t('Cleanup and output'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['output']['verify_html'] = array(
    '#type' => 'checkbox',
    '#title' => t('Verify HTML'),
    '#default_value' => $edit->settings['verify_html'],
    '#return_value' => 1,
    '#description' => t('If enabled, potentially malicious code like <code>&lt;HEAD&gt;</code> tags will be removed from HTML contents.'),
  );

  $form['output']['preformatted'] = array(
    '#type' => 'checkbox',
    '#title' => t('Preformatted'),
    '#default_value' => $edit->settings['preformatted'],
    '#return_value' => 1,
    '#description' => t('If enabled, the editor will insert TAB characters on tab and preserve other whitespace characters just like a PRE element in HTML does.'),
  );

  $form['output']['convert_fonts_to_spans'] = array(
    '#type' => 'checkbox',
    '#title' => t('Convert &lt;font&gt; tags to styles'),
    '#default_value' => $edit->settings['convert_fonts_to_spans'],
    '#return_value' => 1,
    '#description' => t('If enabled, HTML tags declaring the font size, font family, font color and font background color will be replaced by inline CSS styles.'),
  );

  $form['output']['remove_linebreaks'] = array(
    '#type' => 'checkbox',
    '#title' => t('Remove linebreaks'),
    '#default_value' => $edit->settings['remove_linebreaks'],
    '#return_value' => 1,
    '#description' => t('If enabled, the editor will remove most linebreaks from contents. Disabling this option could avoid conflicts with other input filters.'),
  );

  $form['output']['apply_source_formatting'] = array(
    '#type' => 'checkbox',
    '#title' => t('Apply source formatting'),
    '#default_value' => $edit->settings['apply_source_formatting'],
    '#return_value' => 1,
    '#description' => t('If enabled, the editor will re-format the HTML source code. Disabling this option could avoid conflicts with other input filters.'),
  );

  $form['output']['paste_auto_cleanup_on_paste'] = array(
    '#type' => 'checkbox',
    '#title' => t('Force cleanup on standard paste'),
    '#default_value' => $edit->settings['paste_auto_cleanup_on_paste'],
    '#return_value' => 1,
    '#description' => t('If enabled, the default paste function (CTRL-V or SHIFT-INS) behaves like the "paste from word" plugin function.'),
  );

  $form['css'] = array(
    '#type' => 'fieldset',
    '#title' => t('CSS'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['css']['block_formats'] = array(
    '#type' => 'textfield',
    '#title' => t('Block formats'),
    '#default_value' => $edit->settings['block_formats'],
    '#size' => 40,
    '#maxlength' => 250,
    '#description' => t('Comma separated list of HTML block formats. You can only remove elements, not add.'),
  );

  $form['css']['css_setting'] = array(
    '#type' => 'select',
    '#title' => t('Editor CSS'),
    '#default_value' => $edit->settings['css_setting'],
    '#options' => array('theme' => t('Use theme CSS'), 'self' => t('Define CSS'), 'none' => t('TinyMCE default CSS')),
    '#description' => t('Defines the CSS to be used in the editor area.<br />use theme css - load style.css from current site theme.<br/>define css - enter path for css file below.<br />TinyMCE default - uses default CSS from editor.'),
  );

  $form['css']['css_path'] = array(
    '#type' => 'textfield',
    '#title' => t('CSS path'),
    '#default_value' => $edit->settings['css_path'],
    '#size' => 40,
    '#maxlength' => 255,
    '#description' => t('If "Define CSS" has been selected above, enter path to a CSS file or a list of CSS files seperated by a comma.') .'<br />'. t('Available tokens: %b (base path, f.e.: /), %t (path to theme, f.e.: themes/garland)') .'<br />'. t('Examples:') .' css/editor.css,/themes/garland/style.css,%b%t/style.css,http://example.com/external.css',
  );

  $form['css']['css_classes'] = array(
    '#type' => 'textarea',
    '#title' => t('CSS classes'),
    '#default_value' => $edit->settings['css_classes'],
    '#description' => t('Optionally define CSS classes for the "Font style" dropdown list.<br />Enter one class on each line in the format: !format. Example: !example<br />If left blank, CSS classes are automatically imported from all loaded stylesheet(s).', array('!format' => '<code>[title]=[class]</code>', '!example' => 'My heading=header1')),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => $btn,
  );

  return $form;
}

/**
 * Form submit callback for wysiwyg_editor_profile_form_build().
 *
 * @see wysiwyg_editor_profile_form_build()
 */
function wysiwyg_editor_profile_form_submit($form, &$form_state) {
  // Count enabled plugins for this profile.
  $plugin_count = 0;
  foreach ($form_state['values']['buttons'] as $plugin => $buttons) {
    $form_state['values']['buttons'][$plugin] = array_filter($form_state['values']['buttons'][$plugin]);
    $plugin_count += count($form_state['values']['buttons'][$plugin]);
  }
  // Store only enabled buttons.
  $form_state['values']['buttons'] = array_filter($form_state['values']['buttons']);

  // Filter enabled roles for this profile.
  $form_state['values']['rids'] = array_filter($form_state['values']['rids']);

  // Delete existing profile(s) with the current profile name.
  if (!empty($form_state['values']['old_name'])) {
    db_query("DELETE FROM {wysiwyg_editor_profile} WHERE name = '%s' OR name = '%s'", $form_state['values']['name'], $form_state['values']['old_name']);
    db_query("DELETE FROM {wysiwyg_editor_role} WHERE name = '%s' OR name = '%s'", $form_state['values']['name'], $form_state['values']['old_name']);
  }

  // Remove FAPI values.
  // @see system_settings_form_submit()
  unset($form_state['values']['submit'], $form_state['values']['form_id'], $form_state['values']['op'], $form_state['values']['form_token']);

  // Insert new profile data.
  db_query("INSERT INTO {wysiwyg_editor_profile} (name, settings, plugin_count) VALUES ('%s', '%s', %d)", $form_state['values']['name'], serialize($form_state['values']), $plugin_count);
  foreach ($form_state['values']['rids'] as $rid => $value) {
    db_query("INSERT INTO {wysiwyg_editor_role} (name, rid) VALUES ('%s', %d)", $form_state['values']['name'], $rid);
  }

  if (isset($form_state['values']['old_name'])) {
    drupal_set_message(t('Wysiwyg Editor profile %name has been updated.', array('%name' => $form_state['values']['name'])));
  }
  else {
    drupal_set_message(t('Wysiwyg Editor profile %name has been created.', array('%name' => $form_state['values']['name'])));
  }
  drupal_goto('admin/settings/wysiwyg/profile');
}

/**
 * Layout for the buttons in the Wysiwyg Editor profile form.
 */
function theme_wysiwyg_editor_admin_button_table($form) {
  $buttons = array();

  // Flatten forms array.
  foreach (element_children($form) as $name) {
    foreach (element_children($form[$name]) as $button) {
      $buttons[] = drupal_render($form[$name][$button]);
    }
  }

  // Split checkboxes into rows with 3 columns.
  $total = count($buttons);
  $rows = array();
  for ($i = 0; $i < $total; $i++) {
    $row    = array();
    $row[]  = array('data' => $buttons[$i]);
    $row[]  = array('data' => $buttons[++$i]);
    $row[]  = array('data' => $buttons[++$i]);
    $rows[] = $row;
  }

  $output = theme('table', array(), $rows, array('width' => '100%'));

  return $output;
}

/**
 * Controller for Wysiwyg Editor profiles.
 */
function wysiwyg_editor_profile_overview() {
  $output = '';
  $usable = TRUE;
  // Check if a Wysiwyg Editor is installed.
  // Hard-coded for TinyMCE for now.
  $path_wysiwyg = drupal_get_path('module', 'wysiwyg_editor');
  $tinymce_loc = $path_wysiwyg .'/tinymce/';
  if (!is_dir($tinymce_loc)) {
    drupal_set_message(t('Could not find the TinyMCE engine installed at <strong>!tinymce-directory</strong>. Please <a href="!tinymce-url">download TinyMCE</a>, uncompress it and copy the folder into !tinymce-path.', array('!tinymce-directory' => $tinymce_loc, '!tinymce-url' => 'http://tinymce.moxiecode.com/download.php', '!tinymce-path' => $path_wysiwyg)), 'error');
    $usable = FALSE;
  }
  // Check if at least one role is granted access to Wysiwyg Editor.
  $access_check = user_roles(FALSE, 'access wysiwyg editor');
  if (!$access_check) {
    drupal_set_message(t('You must <a href="!access-control-url">assign</a> at least one role with the \'access wysiwyg editor\' permission before creating a profile.', array('!access-control-url' => url('admin/user/permissions'))), 'error');
    $usable = FALSE;
  }
  if (!$usable) {
    return $output;
  }

  $profiles = wysiwyg_editor_profile_load();
  if ($profiles) {
    $roles = user_roles(FALSE, 'access wysiwyg editor');
    $header = array(t('Profile'), t('Roles'), t('Operations'));
    foreach ($profiles as $p) {
      $rows[] = array(array('data' => $p->name, 'valign' => 'top'), array('data' => implode("<br />\n", $p->rids)), array('data' => l(t('Edit'), 'admin/settings/wysiwyg/profile/edit/'. urlencode($p->name)) .' '. l(t('Delete'), 'admin/settings/wysiwyg/profile/delete/'. urlencode($p->name)), 'valign' => 'top'));
    }
    $output .= theme('table', $header, $rows);
  }
  $output .= '<p>'. t('<a href="!create-profile-url">Create new profile</a>', array('!create-profile-url' => url('admin/settings/wysiwyg/profile/add'))) .'</p>';

  return $output;
}

/**
 * Remove a profile from the database.
 */
function wysiwyg_editor_profile_delete($name) {
  db_query("DELETE FROM {wysiwyg_editor_profile} WHERE name = '%s'", $name);
  db_query("DELETE FROM {wysiwyg_editor_role} WHERE name = '%s'", $name);
}

