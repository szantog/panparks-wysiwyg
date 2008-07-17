/* $Id$ */

-- SUMMARY --

Wysiwyg Editor allows to use what-you-see-is-what-you-get, rich-text, and other
client-side editors for editing contents.  This module requires third-party
editor scripts.

Currently supported editors:
- TinyMCE v2.1.x

For a full description visit the project page:
  http://drupal.org/project/wysiwyg
Bug reports, feature suggestions and latest developments:
  http://drupal.org/project/wysiwyg (component "Wysiwyg Editor")


-- REQUIREMENTS --

* One of the currently supported third-party editors.


-- INSTALLATION --

* Install as usual, see http://drupal.org/node/70151 for further information.
  Note: As of now, you need not install Wysiwyg module.  Wysiwyg Editor module
        is sufficient.

* Download TinyMCE 2.1.x from http://tinymce.moxiecode.com and extract the
  contents of the downloaded archive in a new subfolder 'tinymce' of this
  module.


-- CONFIGURATION --

* Configure user permissions in administer >> User management >> Access control
  >> Wysiwyg Editor.

* Setup editor profiles in administer >> Site configuration >> Wysiwyg Editor.

* Go to administer >> Site configuration >> Input formats and

  - either configure the Full HTML format, assign it to trusted roles, and
    disable 'HTML filter', 'Line break converter', and (optionally) 'URL filter'.

  - or add a new input format, assign it to trusted roles, and ensure that above
    mentioned input filters are disabled.


-- USAGE --

* If enabled in a profile, each user is able to select whether an editor is
  loaded by default, or if she needs to enable it manually.

* TinyMCE keyboard shortcuts:
  Ctrl+Z  Undo
  Ctrl+Y  Redo
  Ctrl+B  Bold
  Ctrl+I  Italic
  Ctrl+U  Underline


-- CONTACT --

Current maintainers:
* Daniel F. Kudwien (sun) - dev@unleashedmind.com

Previous maintainers:
* kreynen http://drupal.org/user/48877
* Allie Micka http://drupal.org/user/15091
* m3avrck http://drupal.org/user/12932
* nedjo http://drupal.org/user/4481
* Steve McKenzie http://drupal.org/user/45890
* ufku http://drupal.org/user/9910
* Matt Westgate <drupal AT asitis DOT org> and
* Jeff Robbins <robbins AT jjeff DOT com>
* Richard Bennett <richard.b AT gritechnologies DOT com>


This project has been sponsored by:
* UNLEASHED MIND
  Specialized in consulting and planning of Drupal powered sites, UNLEASHED
  MIND offers installation, development, theming, customization, and hosting
  to get you started. Visit http://www.unleashedmind.com for more information.

