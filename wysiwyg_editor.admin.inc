<?php
// $Id$


/**
 * @file
 * Integrate Wysiwyg editors into Drupal.
 */

/**
 * Controller for Wysiwyg Editor administrative settings.
 *
 * @todo Move into hook_menu(), resp. FAPI functions.
 */
function _wysiwyg_editor_admin($op = NULL) {
  $edit = $_POST;
  switch ($op) {
    case 'add':
      $breadcrumb[] = array('path' => 'admin', 'title' => t('administer'));
      $breadcrumb[] = array('path' => 'admin/settings/wysiwyg/profile', 'title' => t('Wysiwyg Editor'));
      $breadcrumb[] = array('path' => 'admin/settings/wysiwyg/profile/add', 'title' => t('Add new Wysiwyg Editor profile'));
      menu_set_location($breadcrumb);
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
      drupal_set_title(t('Wysiwyg Editor settings'));
      //Check if Wysiwyg Editor is installed.
      $tinymce_loc = drupal_get_path('module', 'wysiwyg_editor') .'/tinymce/';
      if (!is_dir($tinymce_loc)) {
        drupal_set_message(t('Could not find the TinyMCE engine installed at <strong>!tinymce-directory</strong>. Please <a href="http://tinymce.moxiecode.com/">download TinyMCE</a>, uncompress it and copy the folder into !tinymce-path.', array('!tinymce-path' => drupal_get_path('module', 'wysiwyg_editor'), '!tinymce-directory' => $tinymce_loc)), 'error');
      }
      $output = wysiwyg_editor_profile_overview();
  }

  return $output;
}

/**
 * Return an HTML form for profile configuration.
 */
function wysiwyg_editor_profile_form($edit) {
  $edit = (object)$edit;

  // Only display the roles that currently don't have a Wysiwyg Editor profile. One
  // profile per role.
  $orig_roles = user_roles(FALSE, 'access wysiwyg editor');
  $roles = $orig_roles;
  if (arg(3) == 'add') {
    $result = db_query('SELECT DISTINCT(rid) FROM {wysiwyg_editor_role}');
    while ($data = db_fetch_object($result)) {
      if (!in_array($data->rid, array_keys((array) $edit->rids)) && !form_get_errors()) {
        unset($roles[$data->rid]);
      }
    }
    if (!$orig_roles) {
      drupal_set_message(t('You must <a href="!access-control-url">assign</a> at least one role with the \'access wysiwyg editor\' permission before creating a profile.', array('!access-control-url' => url('admin/user/access'))), 'error');
    }
    else if (!$roles) {
      drupal_set_message(t('You will not be allowed to create a new profile since all user roles have already been assigned profiles. Either remove an existing Wysiwyg Editor profile from at least one role or assign another role the \'access wysiwyg editor\' permission.'), 'error');
    }
    else if (count($orig_roles) != count($roles)) {
      drupal_set_message(t('Not all user roles are shown since they already have Wysiwyg Editor profiles. You must first unassign profiles in order to add them to a new one.'));
    }
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
    '#default_value' => isset($edit->name) ? $edit->name : NULL,
    '#size' => 40,
    '#maxlength' => 128,
    '#description' => t('Enter a name for this profile. This name is only visible within the Wysiwyg Editor administration page.'),
    '#required' => TRUE,
  );

  $form['basic']['rids'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Roles allowed to use this profile'),
    '#default_value' => isset($edit->rids) ? array_keys((array)$edit->rids) : NULL,
    '#options' => $roles,
    '#description' => t('Check at least one role. Only roles with \'access wysiwyg editor\' permission will be shown here.'),
    '#required' => TRUE,
  );

  $form['basic']['default'] = array(
    '#type' => 'select',
    '#title' => t('Default state'),
    '#default_value' => isset($edit->settings['default']) ? $edit->settings['default'] : 'false',
    '#options' => array('false' => t('Disabled'), 'true' => t('Enabled')),
    '#description' => t('Default editor state for users in this profile. Users will be able to override this state if the next option is enabled.'),
  );

  $form['basic']['user_choose'] = array(
    '#type' => 'select',
    '#title' => t('Allow users to choose default'),
    '#default_value' => isset($edit->settings['user_choose']) ? $edit->settings['user_choose'] : 'false',
    '#options' => array('false' => t('Disabled'), 'true' => t('Enabled')),
    '#description' => t('If allowed, users will be able to choose their own Wysiwyg Editor default state by visiting their profile page.'),
  );

  $form['basic']['show_toggle'] = array(
    '#type' => 'select',
    '#title' => t('Show disable/enable rich text editor toggle'),
    '#default_value' => isset($edit->settings['show_toggle']) ? $edit->settings['show_toggle'] : 'true',
    '#options' => array('false' => t('Disabled'), 'true' => t('Enabled')),
    '#description' => t('Whether or not to show the disable/enable rich text editor toggle below the textarea. If false, editor defaults to the global default or user default (see above).'),
  );

  // This line upgrades previous versions of Wysiwyg Editor for user who previously selected a theme other than advanced.
  if (isset($edit->settings['theme']) && $edit->settings['theme'] != 'advanced') {
    $edit->settings['theme'] = 'advanced';
  }

  $form['basic']['theme'] = array(
    '#type' => 'hidden',
    '#value' => isset($edit->settings['theme']) ? $edit->settings['theme'] : 'advanced',
  );

  $form['basic']['language'] = array(
    '#type' => 'select',
    '#title' => t('Language'),
    '#default_value' => isset($edit->settings['language']) ? $edit->settings['language'] : 'en',
    '#options' => drupal_map_assoc(array('ar', 'ca', 'cs', 'cy', 'da', 'de', 'el', 'en', 'es', 'fa', 'fi', 'fr', 'fr_ca', 'he', 'hu', 'is', 'it', 'ja', 'ko', 'nb', 'nl', 'nn', 'pl', 'pt', 'pt_br', 'ru', 'ru_KOI8-R', 'ru_UTF-8', 'si', 'sk', 'sv', 'th', 'zh_cn', 'zh_tw', 'zh_tw_utf8')),
    '#description' => t('The language for the Wysiwyg Editor interface. Language codes based on the <a href="http://www.loc.gov/standards/iso639-2/englangn.html">ISO-639-2</a> format.'),
  );

  $form['basic']['safari_message'] = array(
    '#type' => 'select',
    '#title' => t('Safari browser warning'),
    '#default_value' => isset($edit->settings['safari_message']) ? $edit->settings['safari_message'] : 'false',
    '#options' => array('false' => t('Disabled'), 'true' => t('Enabled')),
    '#description' => t('TinyMCE support for the Safari web browser is experimental and a warning message is displayed when that browser is detected. You can disable this message here.'),
  );

  $form['visibility'] = array(
    '#type' => 'fieldset',
    '#title' => t('Visibility'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $access = user_access('use PHP for block visibility');

  // If the visibility is set to PHP mode but the user doesn't have this block permission, don't allow them to edit nor see this PHP code
  if (isset($edit->settings['access']) && $edit->settings['access'] == 2 && !$access) {
    $form['visibility'] = array();
    $form['visibility']['access'] = array(
      '#type' => 'value',
      '#value' => 2,
    );
    $form['visibility']['access_pages'] = array(
      '#type' => 'value',
      '#value' => (isset($edit->settings['access_pages']) ? $edit->settings['access_pages'] : NULL),
      '#wysiwyg' => FALSE,
    );
  }
  else {
    $options = array(t('Show on every page except the listed pages.'), t('Show on only the listed pages.'));
    $description = t("Enter one page per line as Drupal paths. The '*' character is a wildcard. Example paths are '!blog' for the blog page and !blog-wildcard for every personal blog. !front is the front page.", array('!blog' => theme('placeholder', 'blog'), '!blog-wildcard' => theme('placeholder', 'blog/*'), '!front' => theme('placeholder', '<front>')));

    if ($access) {
      $options[] = t('Show if the following PHP code returns <code>TRUE</code> (PHP-mode, experts only).');
      $description .= t('If the PHP-mode is chosen, enter PHP code between !php. Note that executing incorrect PHP-code can break your Drupal site.', array('!php' => theme('placeholder', '<?php ?>')));
    }
    $form['visibility']['access'] = array(
      '#type' => 'radios',
      '#title' => t('Show Wysiwyg Editor on specific pages'),
      '#default_value' => isset($edit->settings['access']) ? $edit->settings['access'] : 1,
      '#options' => $options,
    );
    $form['visibility']['access_pages'] = array(
      '#type' => 'textarea',
      '#title' => t('Pages'),
      '#default_value' => isset($edit->settings['access_pages']) ? $edit->settings['access_pages'] : wysiwyg_editor_help('admin/settings/wysiwyg/profile#pages'),
      '#description' => $description,
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

  $metadata = _wysiwyg_editor_get_buttons(FALSE);
  // Generate the button list.
  foreach ($metadata as $name => $meta) {
    if (isset($meta['buttons']) && is_array($meta['buttons'])) {
      foreach ($meta['buttons'] as $button) {
        if ($name != 'default') {
          $img_src = drupal_get_path('module', 'wysiwyg_editor') ."/tinymce/jscripts/tiny_mce/plugins/$name/images/$name.gif";

          //correct for plugins that have more than one button
          if (!file_exists($img_src)) {
            $img_src = drupal_get_path('module', 'wysiwyg_editor') ."/tinymce/jscripts/tiny_mce/plugins/$name/images/$button.gif";
          }
        }
        else {
          $img_src = drupal_get_path('module', 'wysiwyg_editor') ."/tinymce/jscripts/tiny_mce/themes/advanced/images/$button.gif";
        }

        $b = file_exists($img_src) ? '<img src="'. base_path() . $img_src .'" title="'. $button .'" style="border: 1px solid grey; vertical-align: middle;" />' : $button;

        if ($name == 'default') {
          $title = $b;
        }
        else {
          $title = isset($metadata[$name]['longname']) ? $metadata[$name]['longname'] : $name;
          if (isset($metadata[$name]['infourl'])) {
            $title = '<a href="'. $metadata[$name]['infourl'] .'" target="_blank">'. $title .'</a>';
          }
          $title = $b .' &#8211; '. $title;
        }
        $form_value = isset($edit->settings['buttons'][$name .'-'. $button]) ? $edit->settings['buttons'][$name .'-'. $button] : NULL;
        $form['buttons'][$name .'-'. $button] = array('#type' => 'checkbox', '#title' => $title, '#default_value' => $form_value);
      }
    }
    else {
      $title = $metadata[$name]['longname'] ? $metadata[$name]['longname'] : $name;
      if ($metadata[$name]['infourl']) {
        $title = '<a href="'. $metadata[$name]['infourl'] .'" target="_blank">'. $title .'</a>';
      }
      $form_value = isset($edit->settings['buttons'][$name]) ? $edit->settings['buttons'][$name] : NULL;
      $form['buttons'][$name] = array('#type' => 'checkbox', '#title' => $title, '#default_value' => $form_value);
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
    '#default_value' => isset($edit->settings['toolbar_loc']) ? $edit->settings['toolbar_loc'] : NULL,
    '#options' => array('bottom' => t('Bottom'), 'top' => t('Top')),
    '#description' => t('Show toolbar at the top or bottom of the editor area?'),
  );

  $form['appearance']['toolbar_align'] = array(
    '#type' => 'select',
    '#title' => t('Toolbar alignment'),
    '#default_value' => isset($edit->settings['toolbar_align']) ? $edit->settings['toolbar_align'] : NULL,
    '#options' => array('center' => t('Center'), 'left' => t('Left'), 'right' => t('Right')),
    '#description' => t('Align tool icons left, center, or right within the toolbar.'),
  );

  $form['appearance']['path_loc'] = array(
    '#type' => 'select',
    '#title' => t('Path location'),
    '#default_value' => isset($edit->settings['path_loc']) ? $edit->settings['path_loc'] : 'bottom',
    '#options' => array('none' => t('None'), 'top' => t('Top'), 'bottom' => t('Bottom')),
    '#description' => t('Path to html elements (i.e. "body>table>tr>td"). Show at top, bottom, or not at all.'),
  );

  $form['appearance']['resizing'] = array(
    '#type' => 'select',
    '#title' => t('Enable resizing button'),
    '#default_value' => isset($edit->settings['resizing']) ? $edit->settings['resizing'] : 'true',
    '#options' => array('false' => t('Disabled'), 'true' => t('Enabled')),
    '#description' => t(' This option gives you the ability to enable/disable the resizing button. If enabled the <strong>Path location toolbar</strong> must be set to "top" or "bottom" in order to display the resize icon.'),
  );

  $form['appearance']['block_formats'] = array(
    '#type' => 'textfield',
    '#title' => t('Block formats'),
    '#default_value' => isset($edit->settings['block_formats']) ? $edit->settings['block_formats'] : 'p,address,pre,h1,h2,h3,h4,h5,h6',
    '#size' => 40,
    '#maxlength' => 250,
    '#description' => t('Comma separated list of HTML block formats. You can only remove elements, not add.'),
  );

  $form['output'] = array(
    '#type' => 'fieldset',
    '#title' => t('Cleanup and output'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['output']['verify_html'] = array(
    '#type' => 'select',
    '#title' => t('Verify HTML'),
    '#default_value' => isset($edit->settings['verify_html']) ? $edit->settings['verify_html'] : NULL,
    '#options' => array('false' => t('Disabled'), 'true' => t('Enabled')),
    '#description' => t('Should the HTML contents be verified or not? Verifying will strip &lt;head&gt tags, so choose false if you will be editing full page HTML.'),
  );

  $form['output']['preformatted'] = array(
    '#type' => 'select',
    '#title' => t('Preformatted'),
    '#default_value' => isset($edit->settings['preformatted']) ? $edit->settings['preformatted'] : NULL,
    '#options' => array('false' => t('Disabled'), 'true' => t('Enabled')),
    '#description' => t('If this option is set to true, the editor will insert TAB characters on tab and preserve other whitespace characters just like a PRE HTML element does.'),
  );

  $form['output']['convert_fonts_to_spans'] = array(
    '#type' => 'select',
    '#title' => t('Convert &lt;font&gt; tags to styles'),
    '#default_value' => isset($edit->settings['convert_fonts_to_spans']) ? $edit->settings['convert_fonts_to_spans'] : NULL,
    '#options' => array('true' => t('Enabled'), 'false' => t('Disabled')),
    '#description' => t('If you set this option to true, font size, font family, font color and font background color will be replaced by inline styles.'),
  );

  $form['output']['remove_linebreaks'] = array(
    '#type' => 'select',
    '#title' => t('Remove linebreaks'),
    '#default_value' => isset($edit->settings['remove_linebreaks']) ? $edit->settings['remove_linebreaks'] : NULL,
    '#options' => array('true' => t('Enabled'), 'false' => t('Disabled')),
    '#description' => t('Set this option to false to prevent TinyMCE from removing linebreaks from existing nodes.  True avoids conflicts with some filters.'),
  );

  $form['output']['apply_source_formatting'] = array(
    '#type' => 'select',
    '#title' => t('Apply source formatting'),
    '#default_value' => isset($edit->settings['apply_source_formatting']) ? $edit->settings['apply_source_formatting'] : NULL,
    '#options' => array('true' => t('Enabled'), 'false' => t('Disabled')),
    '#description' => t('This option makes TinyMCE apply source formatting.  Set this to true for a cleaner HTML source.  Choose false to avoid conflicts with some filters.'),
  );

  $form['css'] = array(
    '#type' => 'fieldset',
    '#title' => t('CSS'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['css']['css_setting'] = array(
    '#type' => 'select',
    '#title' => t('Editor CSS'),
    '#default_value' => isset($edit->settings['css_setting']) ? $edit->settings['css_setting'] : 'theme',
    '#options' => array('theme' => t('Use theme CSS'), 'self' => t('Define CSS'), 'none' => t('TinyMCE default CSS')),
    '#description' => t('Defines the CSS to be used in the editor area.<br />use theme css - load style.css from current site theme.<br/>define css - enter path for css file below.<br />TinyMCE default - uses default CSS from editor.'),
  );

  $form['css']['css_path'] = array(
    '#type' => 'textfield',
    '#title' => t('CSS path'),
    '#default_value' => isset($edit->settings['css_path']) ? $edit->settings['css_path'] : NULL,
    '#size' => 40,
    '#maxlength' => 255,
    '#description' => t('Enter path to CSS file (<em>example: "css/editor.css"</em>) or a list of css files seperated by a comma (<em>example: /themes/garland/style.css,http://domain.com/customMCE.css</em>).<br />Macros: %h (host name: http://www.example.com/), %t (path to theme: theme/yourtheme/)<br />Be sure to select "define css" above.'),
  );

  $form['css']['css_classes'] = array(
    '#type' => 'textfield',
    '#title' => t('CSS classes'),
    '#default_value' => isset($edit->settings['css_classes']) ? $edit->settings['css_classes'] : NULL,
    '#size' => 40,
    '#maxlength' => 255,
    '#description' => t('Adds CSS classes to the "styles" droplist. Format is: &lt;title&gt;=&lt;class&gt;;<br/> Example: Header 1=header1;Header 2=header2;Header 3=header3 (note: no trailing \';\')<br />Leave blank to automatically import list of CSS classes from style sheet.'),
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
function wysiwyg_editor_profile_form_submit($form_id, &$form_values) {
  // Count enabled plugins for this profile.
  $form_values['buttons'] = array_filter($form_values['buttons']);
  $plugin_count = count($form_values['buttons']);
  // Filter enabled roles for this profile.
  $form_values['rids'] = array_filter($form_values['rids']);

  // Delete existing profile(s) with the current profile name.
  db_query("DELETE FROM {wysiwyg_editor_profile} WHERE name = '%s' OR name = '%s'", $form_values['name'], $form_values['old_name']);
  db_query("DELETE FROM {wysiwyg_editor_role} WHERE name = '%s' OR name = '%s'", $form_values['name'], $form_values['old_name']);

  // Insert new profile data.
  db_query("INSERT INTO {wysiwyg_editor_profile} (name, settings, plugin_count) VALUES ('%s', '%s', %d)", $form_values['name'], serialize($form_values), $plugin_count);
  foreach ($form_values['rids'] as $rid => $value) {
    db_query("INSERT INTO {wysiwyg_editor_role} (name, rid) VALUES ('%s', %d)", $form_values['name'], $rid);
  }

  if (isset($form_values['old_name'])) {
    drupal_set_message(t('Wysiwyg Editor profile has been updated.'));
  }
  else {
    drupal_set_message(t('Wysiwyg Editor profile has been created.'));
  }
  drupal_goto('admin/settings/wysiwyg/profile');
}

/**
 * Layout for the buttons in the Wysiwyg Editor profile form
 */
function theme_wysiwyg_editor_admin_button_table($form) {
  $buttons = array();

  // Flatten forms array
  foreach (element_children($form) as $key) {
    $buttons[] = drupal_render($form[$key]);
  }

  //split checkboxes into rows with 3 columns
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
  $profiles = wysiwyg_editor_profile_load();
  if ($profiles) {
    $roles = user_roles();
    $header = array(t('Profile'), t('Roles'), t('Operations'));
    foreach ($profiles as $p) {
      $rows[] = array(array('data' => $p->name, 'valign' => 'top'), array('data' => implode("<br />\n", $p->rids)), array('data' => l(t('Edit'), 'admin/settings/wysiwyg/profile/edit/'. urlencode($p->name)) .' '. l(t('Delete'), 'admin/settings/wysiwyg/profile/delete/'. urlencode($p->name)), 'valign' => 'top'));
    }
    $output .= theme('table', $header, $rows);
    $output .= t('<p><a href="!create-profile-url">Create new profile</a></p>', array('!create-profile-url' => url('admin/settings/wysiwyg/profile/add')));
  }
  else {
    drupal_set_message(t('No profiles found. Click here to <a href="!create-profile-url">create a new profile</a>.', array('!create-profile-url' => url('admin/settings/wysiwyg/profile/add'))));
  }

  return $output;
}

/**
 * Remove a profile from the database.
 */
function wysiwyg_editor_profile_delete($name) {
  db_query("DELETE FROM {wysiwyg_editor_profile} WHERE name = '%s'", $name);
  db_query("DELETE FROM {wysiwyg_editor_role} WHERE name = '%s'", $name);
}

